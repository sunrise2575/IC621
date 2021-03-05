#pragma once
#include <Prelude.h>
#include _CH_PMFUNCTION_

class pm_forcefield_cube : protected PMFunction
{
private:
	Vector3 max;
	Vector3 min;
	Vector3 force;

	unsigned int* BorderGrid;
	unsigned int* InsideGrid;

	bool* AffectType;
	unsigned char numType;

	bool onoff;

	inline void FuncforParticle(Particle& particle);

protected:
	virtual void Func(ParticleManager& TargetPM);

public:

	pm_forcefield_cube(ParticleManager& TargetPM);
	~pm_forcefield_cube();
	pm_forcefield_cube(const pm_forcefield_cube& copy);
	pm_forcefield_cube& operator=(const pm_forcefield_cube& copy);

	void Switch();
	void Switch(bool isWork);

	void SetArea(Vector3 Max, Vector3 Min);
	void SetForce(Vector3 Force);

	void Enroll();
	void Remove();
};

pm_forcefield_cube::pm_forcefield_cube(ParticleManager& TargetPM)
{
	max = 0;
	min = 0;
	force = 0;

	PM = &TargetPM;

	numType = PM->GetMaxType();
	AffectType = new bool[numType];

	for (unsigned short i = 0; i < numType; i++)
	{
		AffectType[i] = true;
	}

	BorderGrid = NULL;
	InsideGrid = NULL;

	onoff = false;
}
pm_forcefield_cube::~pm_forcefield_cube()
{
	try
	{
		Remove();
	}
	catch (...)
	{
		//do nothing
	}

	delete[] AffectType;

	if (BorderGrid != NULL)
	{
		delete[] BorderGrid;
		delete[] InsideGrid;
	}
}
pm_forcefield_cube::pm_forcefield_cube(const pm_forcefield_cube& copy)
{
	max = copy.max;
	min = copy.min;
	force = copy.force;
	PM = copy.PM;
	numType = copy.numType;
	AffectType = new bool[numType];
	for (unsigned short i = 0; i < numType; i++)
	{
		AffectType[i] = copy.AffectType[i];
	}

	if (copy.BorderGrid != NULL)
	{
		BorderGrid = new unsigned[copy.BorderGrid[0]];
		for (unsigned i = 0; i < copy.BorderGrid[0]; i++)
		{
			BorderGrid[i + 1] = copy.BorderGrid[i + 1];
		}
		InsideGrid = new unsigned[copy.InsideGrid[0]];

		for (unsigned i = 0; i < copy.InsideGrid[0]; i++)
		{
			InsideGrid[i + 1] = copy.InsideGrid[i + 1];
		}
	}
	onoff = false;

	//std::cout << "Origin |" << copy.GetID() << ", Copyed |" << GetID() << std::endl;
}
pm_forcefield_cube& pm_forcefield_cube::operator=(const pm_forcefield_cube& copy)
{
	max = copy.max;
	min = copy.min;
	force = copy.force;
	PM = copy.PM;
	numType = copy.numType;
	delete[] AffectType;
	AffectType = new bool[numType];
	for (unsigned short i = 0; i < numType; i++)
	{
		AffectType[i] = copy.AffectType[i];
	}

	if (BorderGrid != NULL)
	{
		delete[] BorderGrid;
		delete[] InsideGrid;
		BorderGrid = NULL;
		InsideGrid = NULL;
	}
	if (copy.BorderGrid != NULL)
	{
		BorderGrid = new unsigned[copy.BorderGrid[0]];
		for (unsigned i = 0; i < copy.BorderGrid[0]; i++)
		{
			BorderGrid[i + 1] = copy.BorderGrid[i + 1];
		}
		InsideGrid = new unsigned[copy.InsideGrid[0]];

		for (unsigned i = 0; i < copy.InsideGrid[0]; i++)
		{
			InsideGrid[i + 1] = copy.InsideGrid[i + 1];
		}
	}

	onoff = false;

	//std::cout << "Origin |" << copy.GetID() << ", Copyed |" << GetID() << std::endl;

	return *this;
}

void pm_forcefield_cube::Func(ParticleManager& TargetPM)
{
	if (onoff)
	{
		for (unsigned short Type = 0; Type < numType; Type++)
		{
			if (AffectType[Type])
			{
				for (unsigned i = 0; i < BorderGrid[0]; i++)
				{
					__BindedList<unsigned>& CurrentGrid = TargetPM.GetGrid(Type, BorderGrid[i + 1]);
					unsigned int count = CurrentGrid.count();
					for (unsigned j = 0; j < count; j++)
					{
						Particle& CurrentParticle = TargetPM.GetParticles(Type)[CurrentGrid[j]];
						if (min < CurrentParticle.position && max > CurrentParticle.position)
						{
							FuncforParticle(CurrentParticle);
						}//조건 검사
					}
				}
				for (unsigned i = 0; i < InsideGrid[0]; i++)
				{
					__BindedList<unsigned>& CurrentGrid = TargetPM.GetGrid(Type, InsideGrid[i + 1]);
					unsigned int count = CurrentGrid.count();
					for (unsigned j = 0; j < count; j++)
					{
						Particle& CurrentParticle = TargetPM.GetParticles(Type)[CurrentGrid[j]];
						FuncforParticle(CurrentParticle);
					}
				}
			}
		}
	}
}
void pm_forcefield_cube::FuncforParticle(Particle& particle)
{
	particle.force += force*PM->GetUnitTime();
}
void pm_forcefield_cube::Switch()
{
	onoff = !onoff;
}
void pm_forcefield_cube::Switch(bool isWork)
{
	onoff = isWork;
}

void pm_forcefield_cube::SetArea(Vector3 Max, Vector3 Min)
{
	if (max.x < min.x || max.y < min.y || max.z < min.z) { printf("ERROR :: Not vaild area setting"); }
	else
	{
		max = Max;
		min = Min;

		__vector3<unsigned short> max3 = PM->GetGridIndex3(max);
		__vector3<unsigned short> min3 = PM->GetGridIndex3(min);

		if (BorderGrid != NULL)
		{
			delete[] BorderGrid;
			delete[] InsideGrid;
		}

		if (max3 == min3)
		{
			BorderGrid = new unsigned int[2];
			BorderGrid[0] = 1;
			BorderGrid[1] = PM->ConvertGridIndex(max3);
			InsideGrid = new unsigned int{ 0 };
		}
		else
		{
			BorderGrid = new unsigned int[(innerMulty(max3 - min3 + 1) - innerMulty(max3 - min3 - 1)) + 1];
			BorderGrid[0] = 0;
			InsideGrid = new unsigned int[1 + innerMulty(max3 - min3 - 1)];
			InsideGrid[0] = 0;

			for (unsigned short x = min3.x; x <= max3.x; x++)
			{
				for (unsigned short y = min3.y; y <= max3.y; y++)
				{
					for (unsigned short z = min3.z; z <= max3.z; z++)
					{
						if (x == min3.x || x == max3.x
							|| y == min3.y || y == max3.y
							|| z == min3.z || z == max3.z)
						{
							BorderGrid[1 + BorderGrid[0]] = PM->ConvertGridIndex(__vector3<unsigned short>(x, y, z));
							BorderGrid[0]++;
						}
						else
						{
							InsideGrid[1 + InsideGrid[0]] = PM->ConvertGridIndex(__vector3<unsigned short>(x, y, z));
							InsideGrid[0]++;
						}
					}
				}
			}
		}//영역 등록
	}
}
void pm_forcefield_cube::SetForce(Vector3 Force)
{
	force = Force;
}

void pm_forcefield_cube::Enroll()
{
	push(*PM, _PM_FORCE_ORDER_);
}
void pm_forcefield_cube::Remove()
{
	pop();
}