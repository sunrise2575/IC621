#pragma once

#include <Prelude.h>

#include _CH_VECTOR_
#include _CH_BINDEDLIST_
#include _CH_UTILITY_
#include _CH_PARTICLE_

//void defaultOutOfAreaFunc(Particle& particle, phys_t& unit_time);
void defaultParticleUpdateFunc(__BindedList<Particle>& targetParticles, phys_t& unit_time);
/*
class ParticleManager;
struct FuncRack
{
std::function<void(ParticleManager& TargetPM)> CustomFunc;//실행될 함수
unsigned int funcID;//함수 식별용 아이디
unsigned int instanceID;//함수가 등록될 때 마다 인터페이스와 펑션 렉 양쪽에 저장되는 ID. order와 ID를 참조하여 삭제 수행
int order;//정렬 순서. 낮을수록 먼저 실행됨
};
std::ostream& operator<<(std::ostream& out, const FuncRack& vector) {

return out << '(' << vector.funcID << ", " << vector.instanceID << ", " << vector.order << ')';
}
*/

class ParticleManager {
private:

	//--------------------------------------//
	//	Particle Type Management			//
	//--------------------------------------//

	unsigned short	maxType; //최대 입자 종류 개수
	unsigned int*	maxParticle; //최대 입자 개수
	unsigned short*	maxParticleInGrid;//격자 내 최대 입자 개수
	bool**			particleInteraction;//종류별 상호 작용 여부 (bool [from][to] mean [from] affect force [to]) 
	std::function<void(__BindedList<Particle>& targetParticles, phys_t& unitTime)>* processAfterCalc;
	//Force 계산 후 Force와 기타 과정에 대해 처리할 지정 함수. 단 vel -> pos 대입은 시뮬레이터에서 수행함.


	//--------------------------------------//
	//	Simulation Area Control				//
	//--------------------------------------//

	phys_t unitTime; // 1 계산 당 지나는 시간

	Vector3 maxPosition;//
	Vector3 minPosition;//시뮬레이션 영역 표시
	phys_t	gapPosition;//안전을 위한 시뮬레이션 갭. 
	std::function<void(Particle& targetParticle, phys_t& unitTime)> processOutOfArea;//갭 영역에 들어온 입자들에게 적용될 펑선

	__vector3<unsigned short> numberOfGrid;//x,y,z방향의 총 격자 개수
	unsigned int totalGrid;//numberOfGrid의 x*y*z와 같음. total Grid Number


	//--------------------------------------//
	//	Particle Data Structure				//
	//--------------------------------------//

	__BindedList<Particle>** particles;
	__BindedList<unsigned int>*** grid;
	unsigned int ** gridNear; // 연산 시 검사할 인근 그리드.
	unsigned int ** gridOut; // Gap Area Grids;

	//--------------------------------------//
	//	Calculation							//
	//--------------------------------------//

	void InitializeParticle();
	void CalculateDensity(unsigned start, unsigned end);
	void CalculateForce(unsigned start, unsigned end);
	void AfterCalc();
	void RecheckGrid(unsigned short typeIndex, Particle& particle);
	void ProcessOut();


	//--------------------------------------//
	//	Fixed Time Interface				//
	//--------------------------------------//

	void CustomProcess();// for user control interface

	struct FuncRack
	{
		std::function<void(ParticleManager& TargetPM)> CustomFunc;//실행될 함수
		unsigned int ID;//함수가 등록될 때 마다 인터페이스와 펑션 렉 양쪽에 저장되는 ID. order와 ID를 참조하여 삭제 수행
		int order;//정렬 순서. 낮을수록 먼저 실행됨
	};

	__BindedList<FuncRack> *CustomFunction;//외부에서 등록한 함수들이 등록되는 배열


public:

	//--------------------------------------//
	//	Constructor and Destructor			//
	//--------------------------------------//

	ParticleManager(phys_t UnitTime = UNIT_TIME, unsigned short maxParticleType = PARTICLE_TYPE_NUM, unsigned short maxInterfaceNum = 1000, Vector3 MaxPosition = Vector3(MAX_X, MAX_Y, MAX_Z), Vector3 MinPosition = Vector3(MIN_X, MIN_Y, MIN_Z), phys_t GapPosition = KERNEL_DISTANCE);
	//ParticleManager(const ParticleManager& copy);
	//Particle operator=(const ParticleManager& copy);
	~ParticleManager();

	//--------------------------------------//
	//	Status Function						//
	//--------------------------------------//

	bool isCreated();
	unsigned short GetMaxType();

	//--------------------------------------//
	//	Simulation Area						//
	//--------------------------------------//

	void SetUnitTime(phys_t time);
	void SetMaxPosition(Vector3 Max);
	void SetMinPosition(Vector3 Min);
	void SetGapPosition(phys_t Gap);

	phys_t GetUnitTime();
	Vector3 GetMaxPosition();
	Vector3 GetMinPosition();
	phys_t GetGapPosition();

	void SetOutOfAreaFunction(void OutOfAreaFunc(Particle& targetParticle, phys_t& unitTime));
	std::function<void(Particle& targetParticle, phys_t& unitTime)> GetOutOfAreaFunction();

	unsigned short GetGridIndex(Vector3 Position);
	__vector3<unsigned short> GetGridIndex3(Vector3 Position);
	unsigned int ConvertGridIndex(__vector3<unsigned short> index);
	__vector3<unsigned short> ConvertGridIndex(unsigned int index);

	__vector3<unsigned short> GetNumberOfGrid();
	unsigned GetTotalNumberOfGrid();


	//--------------------------------------//
	//	Particle Type Management			//
	//--------------------------------------//

	void EnrollType(unsigned short typeIndex, unsigned int maxParticleNum, unsigned short maxParticleNumInGrid, void ParticleUpdateFunction(__BindedList<Particle>& targetParticles, phys_t& unitTime) = defaultParticleUpdateFunc);

	void SetInteraction(bool isAllInteraction, unsigned short index);
	void SetInteraction(bool isInteraction, unsigned short from, unsigned short to);
	void SetInteraction(bool isInteraction, unsigned short typeIndex, bool ForceToThem, unsigned short from, unsigned short to);
	void SetMaxParticleNum(unsigned int MaxParticleNum, unsigned short typeIndex);
	void SetMaxParticleNumInGrid(unsigned short MaxParticleNumInGrid, unsigned short typeIndex);
	void SetParticleUpdateFunction(void ParticleUpdateFunction(__BindedList<Particle>& targetParticles, phys_t& unitTime), unsigned short typeIndex);

	bool GetInteraction(unsigned short from, unsigned short to);
	unsigned int GetMaxParticleNum(unsigned short typeIndex);
	unsigned short GetMaxParticleNumInGrid(unsigned short typeIndex);
	std::function<void(__BindedList<Particle>& targetParticles, phys_t& unitTime)> GetParticleUpdateFunction(unsigned short typeIndex);

	void DeleteType(unsigned short typeIndex);

	//--------------------------------------//
	//	Calculation and Storage				//
	//--------------------------------------//

	void Start();
	void Calculate();
	void End();

	void CreateParticle(unsigned short typeIndex, Particle& particle);
	void RemoveParticle(unsigned short typeIndex, unsigned int particleIndex);

	__BindedList<Particle>& GetParticles(unsigned short typeIndex);
	//__BindedList<Particle>& operator[](const unsigned short typeIndex);//Exactly Same GetParticles();

	__BindedList<unsigned int>& GetGrid(unsigned short typeIndex, unsigned int gridIndex);
	__BindedList<unsigned int>& GetGrid(unsigned short typeIndex, Vector3 position);
	unsigned int*& GetNearGridIndex(unsigned int gridIndex);
	unsigned int*& GetNearGridIndex(Vector3 position);

	//--------------------------------------//
	//	Data Probe							//
	//--------------------------------------//

	phys_t GetSimpleDensity(unsigned short typeIndex);
	phys_t GetSimpleDensity(unsigned short typeIndex, Vector3 MaxPosition, Vector3 MinPosition);

	phys_t GetProbeDensity(unsigned short typeIndex, phys_t ProbeDistance = KERNEL_DISTANCE / 4.0);
	phys_t GetProbeDensity(unsigned short typeIndex, Vector3 Position);
	phys_t GetProbeDensity(unsigned short typeIndex, Vector3 MaxPosition, Vector3 MinPosition, phys_t ProbeDistance = KERNEL_DISTANCE / 4.0);

	phys_t ConvertDensityToPressure(phys_t Density);

	//--------------------------------------//
	//	Data I/O							//
	//--------------------------------------//

	//void write(const char* filename);
	//void read(const char* fileDir);

	//--------------------------------------//
	//	Fixed Time Interface				//
	//--------------------------------------//

	void PushCustomFunc(std::function<void(ParticleManager& TargetPM)> Func, unsigned ID, int Order);
	void PopCustomFunc(unsigned ID, int Order);

};


class PMFunction
{
private:
	PMFunction(const PMFunction& func);
	PMFunction& operator=(const PMFunction& func);

	static unsigned IDcount;//카운터

protected:

	unsigned int ID;//고유 번호
	int order;//등록된 순서;
	bool isEnrolled;//등록되었을 시 true
	ParticleManager* PM;//현재 등록된 PM.

	virtual void Func(ParticleManager& TargetPM) = NULL;
	PMFunction();
	~PMFunction();

public:

	const unsigned int GetID() const;
	void push(ParticleManager& TargetPM, int Order);
	void pop();

};

// PMFunction part
unsigned PMFunction::IDcount = 0;
PMFunction::PMFunction(const PMFunction& func){}
PMFunction& PMFunction::operator=(const PMFunction& func){ return *this; }//Do nothing

PMFunction::PMFunction()
{
	isEnrolled = false;
	order = 0;
	ID = IDcount;
	ParticleManager* PM = NULL;
	IDcount++;//id counter를 올린다.
}

PMFunction::~PMFunction()
{
	if (isEnrolled)
	{
		try
		{
			pop();
		}
		catch (...)
		{
			//do nothing
		}
	}
}

void PMFunction::push(ParticleManager& TargetPM, int Order)
{
	if (!isEnrolled)
	{
		isEnrolled = true;
		ParticleManager* PM = &TargetPM;
		order = Order;
		TargetPM.PushCustomFunc(std::bind(&PMFunction::Func, this, std::placeholders::_1), ID, Order);
	}
}

void PMFunction::pop()
{
	if (isEnrolled)
	{
		(*PM).PopCustomFunc(ID, order);
		isEnrolled = false;
		order = 0;
	}
}
const unsigned  PMFunction::GetID() const
{
	return ID;
}
//PM Part

//--------------------------------------//
//	Default Function					//
//--------------------------------------//

void defaultParticleUpdateFunc(__BindedList<Particle>& targetParticles, phys_t& unit_time)
{
	unsigned numOfParticle = targetParticles.count();
	for (unsigned i = 0; i < numOfParticle; i++)
	{
		Particle& particle = targetParticles[i];
		particle.velocity += (particle.force / particle.mass)*unit_time;
	}
}

//--------------------------------------//
//	Constructor and Destructor			//
//--------------------------------------//


ParticleManager::ParticleManager(phys_t UnitTime, unsigned short maxParticleType, unsigned short maxInterfaceNum, Vector3 MaxPosition, Vector3 MinPosition, phys_t GapPosition)
{
	if (maxParticleType == 0) { printf("WARNING :: Can't Create Max Number of Particle Type = 0 Particle Manager."); this->~ParticleManager(); }
	else {
		unitTime = UnitTime;

		maxType = maxParticleType;
		maxParticle = new unsigned int[maxParticleType];
		for (unsigned short i = 0; i < maxParticleType; i++)
		{
			maxParticle[i] = 0;
		}

		maxParticleInGrid = new unsigned short[maxParticleType];
		for (unsigned short i = 0; i < maxParticleType; i++)
		{
			maxParticleInGrid[i] = 0;
		}

		particleInteraction = new bool*[maxParticleType];
		for (unsigned short i = 0; i < maxParticleType; i++)
		{
			particleInteraction[i] = new bool[maxParticleType];
			for (unsigned short j = 0; j < maxParticleType; j++) {
				particleInteraction[i][j] = false;
			}
		}

		processAfterCalc = new std::function<void(__BindedList<Particle>&, phys_t&)>[maxParticleType];
		for (unsigned short i = 0; i < maxParticleType; i++)
		{
			processAfterCalc[i] = nullptr;
		}

		maxPosition = MaxPosition;
		minPosition = MinPosition;
		gapPosition = GapPosition;
		processOutOfArea = nullptr;

		numberOfGrid = (gapPosition + (maxPosition - minPosition) / 2) / KERNEL_DISTANCE;
		numberOfGrid += 1;
		numberOfGrid *= 2;
		totalGrid = numberOfGrid.x*numberOfGrid.y*numberOfGrid.z;

		__BindedList<Particle>** particles = NULL;
		__BindedList<unsigned int>*** grid = NULL;
		gridNear = NULL;

		gridOut = NULL;

		CustomFunction = new __BindedList<FuncRack>{ maxInterfaceNum };
	}
}

ParticleManager::~ParticleManager()
{
	delete[] maxParticle;
	delete[] maxParticleInGrid;
	for (unsigned short i = 0; i < maxType; i++)
	{
		delete[] particleInteraction[i];
	}
	delete[] particleInteraction;
	delete[] processAfterCalc;
	delete CustomFunction;

	if (isCreated())
	{
		End();
		/*
		for (unsigned short i = 0; i < maxType; i++)
		{
		delete[] particles[i];
		}
		delete[] particles;

		for (unsigned short i = 0; i < maxType; i++)
		{
		for (unsigned short j = 0; j < totalGrid; j++)
		{
		delete grid[i][j];
		delete[] gridNear[j];
		}
		delete[] grid[i];
		}
		delete[] grid;
		delete[] gridNear;

		delete[] gridOut[0];
		delete[] gridOut[1];
		delete[] gridOut;*/
	}
}


//--------------------------------------//
//	Status Function						//
//--------------------------------------//

bool ParticleManager::isCreated() { return particles != NULL; }
unsigned short ParticleManager::GetMaxType() { return maxType; }

//--------------------------------------//
//	Simulation Area						//
//--------------------------------------//

void ParticleManager::SetUnitTime(phys_t time)
{
	if (isCreated()) { printf("ERROR :: can't change Type values after storage create"); }
	else{ unitTime = time; }
}

void ParticleManager::SetMaxPosition(Vector3 Max)
{
	if (isCreated()) { printf("ERROR :: can't change Type values after storage create"); }
	else
	{
		maxPosition = Max;
		numberOfGrid = (gapPosition + (maxPosition - minPosition) / 2) / KERNEL_DISTANCE;
		numberOfGrid += 1;
		numberOfGrid *= 2;
		totalGrid = numberOfGrid.x*numberOfGrid.y*numberOfGrid.z;
	}
}
void ParticleManager::SetMinPosition(Vector3 Min)
{
	if (isCreated()) { printf("ERROR :: can't change Type values after storage create"); }
	else
	{
		minPosition = Min;
		numberOfGrid = (gapPosition + (maxPosition - minPosition) / 2) / KERNEL_DISTANCE;
		numberOfGrid += 1;
		numberOfGrid *= 2;
		totalGrid = numberOfGrid.x*numberOfGrid.y*numberOfGrid.z;
	}
}
void ParticleManager::SetGapPosition(phys_t Gap)
{
	if (isCreated()) { printf("ERROR :: can't change Type values after storage create"); }
	else
	{
		gapPosition = Gap;
		numberOfGrid = (gapPosition + (maxPosition - minPosition) / 2) / KERNEL_DISTANCE;
		numberOfGrid += 1;
		numberOfGrid *= 2;
		totalGrid = numberOfGrid.x*numberOfGrid.y*numberOfGrid.z;
	}
}

phys_t ParticleManager::GetUnitTime() { return unitTime; }

Vector3 ParticleManager::GetMaxPosition() { return maxPosition; }
Vector3 ParticleManager::GetMinPosition() { return minPosition; }
phys_t ParticleManager::GetGapPosition() { return gapPosition; }

void ParticleManager::SetOutOfAreaFunction(void OutOfAreaFunc(Particle& targetParticle, phys_t& unitTime)) { processOutOfArea = std::bind(OutOfAreaFunc, std::placeholders::_1, std::placeholders::_2); }
std::function<void(Particle&, phys_t&)> ParticleManager::GetOutOfAreaFunction(){ return processOutOfArea; }

unsigned short ParticleManager::GetGridIndex(Vector3 Position)
{
	unsigned short Result = 0;
	Position -= ((maxPosition + minPosition) / 2);
	Position /= KERNEL_DISTANCE;
	Position += (Vector3)(numberOfGrid / 2);
	Result = ConvertGridIndex((__vector3<unsigned short>)Position);
	return Result;
/*	if (Result < totalGrid) { return Result; }
	else
	{
		if (Position.x < 0) { Position.x = 0; }
		else if (Position.x >= numberOfGrid.x) { Position.x = numberOfGrid.x - 1; }
		if (Position.y < 0) { Position.y = 0; }
		else if (Position.y >= numberOfGrid.y) { Position.y = numberOfGrid.y - 1; }
		if (Position.z < 0) { Position.z = 0; }
		else if (Position.z >= numberOfGrid.z) { Position.z = numberOfGrid.z - 1; }
		return ConvertGridIndex((__vector3<unsigned short>)Position);
	}*/
};

__vector3<unsigned short> ParticleManager::GetGridIndex3(Vector3 Position)
{
	unsigned short Result = 0;
	Position -= ((maxPosition + minPosition) / 2);
	Position /= KERNEL_DISTANCE;
	Position += (Vector3)(numberOfGrid / 2);
	if (Position.x < 0) { Position.x = 0; }
	else if (Position.x >= numberOfGrid.x) { Position.x = numberOfGrid.x - 1; }
	if (Position.y < 0) { Position.y = 0; }
	else if (Position.y >= numberOfGrid.y) { Position.y = numberOfGrid.y - 1; }
	if (Position.z < 0) { Position.z = 0; }
	else if (Position.z >= numberOfGrid.z) { Position.z = numberOfGrid.z - 1; }
	return (__vector3<unsigned short>)Position;
};

unsigned int ParticleManager::ConvertGridIndex(__vector3<unsigned short> index)
{
	return Util::Index3(index, numberOfGrid.x, numberOfGrid.y);
	//return index.x + (index.y + (index.z * numberOfGrid.y))* numberOfGrid.x;
}

__vector3<unsigned short> ParticleManager::ConvertGridIndex(unsigned int index)
{
	return Util::v3Index(index, numberOfGrid.x, numberOfGrid.y);
	/*__vector3<unsigned short> Result;
	Result.x = index % numberOfGrid.x;
	Result.y = (index / numberOfGrid.x) % numberOfGrid.y;
	Result.z = index / numberOfGrid.x / numberOfGrid.y;
	return Result;*/
}
__vector3<unsigned short> ParticleManager::GetNumberOfGrid() { return numberOfGrid; }
unsigned ParticleManager::GetTotalNumberOfGrid() { return totalGrid; }


//--------------------------------------//
//	Particle Type Management			//
//--------------------------------------//

void ParticleManager::EnrollType(unsigned short typeIndex, unsigned int maxParticleNum, unsigned short maxParticleNumInGrid, void ParticleUpdateFunction(__BindedList<Particle>& targetParticles, phys_t& unitTime))
{
	if (isCreated()) { printf("ERROR :: can't change Type values after storage create"); }
	else if (maxType <= typeIndex) { printf("ERROR :: exceed max particle type number"); }
	else
	{
		maxParticle[typeIndex] = maxParticleNum;
		maxParticleInGrid[typeIndex] = maxParticleNumInGrid;
		processAfterCalc[typeIndex] = std::bind(ParticleUpdateFunction, std::placeholders::_1, std::placeholders::_2);
		SetInteraction(false, typeIndex);
	}

}

void ParticleManager::SetInteraction(bool isAllInteraction, unsigned short index)
{
	if (isCreated()) { printf("ERROR :: can't change Type values after storage create"); }
	else if (maxType <= index) { printf("ERROR :: exceed max particle type number"); }
	else
	{
		for (unsigned short i = 0; i < maxType; i++)
		{
			particleInteraction[index][i] = isAllInteraction;
			particleInteraction[i][index] = isAllInteraction;
		}
	}
}

void ParticleManager::SetInteraction(bool isInteraction, unsigned short from, unsigned short to)
{
	if (isCreated()) { printf("ERROR :: can't change Type values after storage create"); }
	else if (maxType <= from && maxType <= to) { printf("ERROR :: exceed max particle type number"); }
	else { particleInteraction[from][to] = isInteraction; }
}

void ParticleManager::SetInteraction(bool isInteraction, unsigned short typeIndex, bool ForceToThem, unsigned short from, unsigned short to)
{
	if (isCreated()) { printf("ERROR :: can't change Type values after storage create"); }
	else if (maxType <= typeIndex && maxType <= from && maxType <= to) { printf("ERROR :: exceed max particle type number"); }
	else
	{
		for (unsigned short i = from; i <= to; i++)
		{
			if (ForceToThem) { particleInteraction[typeIndex][i] = isInteraction; }
			else { particleInteraction[i][typeIndex] = isInteraction; }
		}
	}
}

void ParticleManager::SetMaxParticleNum(unsigned int MaxParticleNum, unsigned short typeIndex)
{
	if (isCreated()) { printf("ERROR :: can't change Type values after storage create"); }
	else if (maxType <= typeIndex) { printf("ERROR :: exceed max particle type number"); }
	else { maxParticle[typeIndex] = MaxParticleNum; }
}

void ParticleManager::SetMaxParticleNumInGrid(unsigned short MaxParticleNumInGrid, unsigned short typeIndex)
{
	if (isCreated()) { printf("ERROR :: can't change Type values after storage create"); }
	else if (maxType <= typeIndex) { printf("ERROR :: exceed max particle type number"); }
	else { maxParticleInGrid[typeIndex] = MaxParticleNumInGrid; }
}

void ParticleManager::SetParticleUpdateFunction(void ParticleUpdateFunction(__BindedList<Particle>& targetParticles, phys_t& unitTime), unsigned short typeIndex)
{
	if (isCreated()) { printf("ERROR :: can't change Type values after storage create"); }
	else if (maxType <= typeIndex) { printf("ERROR :: exceed max particle type number"); }
	else { processAfterCalc[typeIndex] = std::bind(ParticleUpdateFunction, std::placeholders::_1, std::placeholders::_2); }
}

bool ParticleManager::GetInteraction(unsigned short from, unsigned short to){ return particleInteraction[from][to]; }
unsigned int ParticleManager::GetMaxParticleNum(unsigned short typeIndex){ return maxParticle[typeIndex]; }
unsigned short ParticleManager::GetMaxParticleNumInGrid(unsigned short typeIndex){ return maxParticleInGrid[typeIndex]; }
std::function<void(__BindedList<Particle>&, phys_t&)> ParticleManager::GetParticleUpdateFunction(unsigned short typeIndex){ return processAfterCalc[typeIndex]; }

void ParticleManager::DeleteType(unsigned short typeIndex) { EnrollType(typeIndex, 0, 0, NULL); }

//--------------------------------------//
//	Calculation and Storage				//
//--------------------------------------//

void ParticleManager::Start()
{
	if (isCreated()) { printf("ERROR :: It is already created!"); }
	else if (maxPosition.x < minPosition.x || maxPosition.y < minPosition.y || maxPosition.z < minPosition.z) { printf("ERROR :: Impossible to create minus volume space ( min > max )"); }
	else if (gapPosition < 0) { printf("ERROR :: gap can not be negative value"); }

	particles = new __BindedList<Particle>*[maxType];
	for (unsigned short i = 0; i < maxType; i++)
	{
		particles[i] = new __BindedList<Particle>{ maxParticle[i] };
	}

	grid = new __BindedList<unsigned int>**[maxType];
	gridNear = new unsigned int*[totalGrid];
	for (unsigned short i = 0; i < maxType; i++)
	{
		grid[i] = new __BindedList<unsigned int>*[totalGrid];
		for (unsigned int j = 0; j < totalGrid; j++)
		{
			grid[i][j] = new __BindedList<unsigned int>{ maxParticleInGrid[i] };

			unsigned int neighborIndex[27];
			unsigned char neighborCount = 0;
			__vector3<unsigned short> temp3dIndex = ConvertGridIndex(j) - 1;
			for (unsigned char neighborhood = 0; neighborhood < 27; neighborhood++)
			{
				neighborIndex[neighborCount] = ConvertGridIndex(temp3dIndex + Util::v3Index(neighborhood, 3, 3));
				if (neighborIndex[neighborCount] < totalGrid && neighborIndex[neighborCount] > j){ neighborCount++; }
				//그전에 계산한 그리드와, 존재하지 않는 그리드는 제외하고 기록
			}
			gridNear[j] = new unsigned int[neighborCount + 1];
			gridNear[j][0] = neighborCount;
			for (int k = 0; k < neighborCount; k++)
			{
				gridNear[j][k + 1] = neighborIndex[k];
			}
		}
	}

	__vector3<unsigned short> maxIndex = GetGridIndex3(maxPosition);
	__vector3<unsigned short> minIndex = GetGridIndex3(minPosition);

	gridOut = new unsigned int*[2]; // first : Border grid, Sec: Outer Grid;

	gridOut[0] = new unsigned int[1 + innerMulty((maxIndex - minIndex) + 1) - innerMulty((maxIndex - minIndex) - 1)];
	gridOut[1] = new unsigned int[1 + totalGrid - innerMulty((maxIndex - minIndex) + 1)];
	gridOut[0][0] = 0;// it indicate size of array;
	gridOut[1][0] = 0;

	for (unsigned short gridZ = 0; gridZ < numberOfGrid.z; gridZ++)
	{
		for (unsigned short gridY = 0; gridY < numberOfGrid.y; gridY++)
		{
			for (unsigned short gridX = 0; gridX < numberOfGrid.x; gridX++)
			{

				if (gridZ > minIndex.z && gridZ < maxIndex.z && gridY > minIndex.y && gridY < maxIndex.y && gridX > minIndex.x && gridX < maxIndex.x)
				{
					gridX = maxIndex.x - 1;
				}
				else if (gridZ < minIndex.z || gridZ > maxIndex.z ||
					gridY < minIndex.y || gridY > maxIndex.y ||
					gridX < minIndex.x || gridX > maxIndex.x)
				{//outer
					gridOut[1][1 + gridOut[1][0]] = ConvertGridIndex(__vector3<unsigned short> {gridX, gridY, gridZ});
					gridOut[1][0]++;
				}
				else
				{//border
					gridOut[0][1 + gridOut[0][0]] = ConvertGridIndex(__vector3<unsigned short> {gridX, gridY, gridZ});
					gridOut[0][0]++;
				}
			}
		}
	}
}

void ParticleManager::End()
{
	if (!isCreated()) { printf("ERROR :: can not delete! It is not created!"); }
	else {
		for (unsigned short i = 0; i < maxType; i++)
		{
			delete particles[i];
		}
		delete[] particles;

		for (unsigned short i = 0; i < maxType; i++)
		{
			for (unsigned int j = 0; j < totalGrid; j++)
			{
				delete grid[i][j];
				delete[] gridNear[j];
			}
			delete[] grid[i];
		}
		delete[] grid;
		delete[] gridNear;
	}
	delete[] gridOut[0];
	delete[] gridOut[1];
	delete[] gridOut;
}

void ParticleManager::Calculate()
{
	if (!isCreated()) { printf("ERROR :: can't calculation. not created yet"); }
	else
	{
		InitializeParticle();
		CustomProcess();
	
		//CalculateDensity(0,totalGrid);
		//CalculateForce(0,totalGrid);
		
		/*std::thread t1{ &ParticleManager::CalculateDensity, this, 0, totalGrid };
		t1.join();
		std::thread t2{ &ParticleManager::CalculateForce, this, 0, totalGrid };
		t2.join();*/

		const char numOfThread = 1;
		//std::thread* threads[numOfThread];

		{
			std::vector<std::thread> threads;
			for (int i = 0; i < numOfThread; ++i)
				threads.push_back(std::thread{ &ParticleManager::CalculateDensity, this, 
					(unsigned int)(i*totalGrid / numOfThread), 
					(unsigned int)((i + 1)*totalGrid / numOfThread) });
			for (auto& thr : threads)
				thr.join();
		}
		
		{
			std::vector<std::thread> threads;
			for (int i = 0; i < numOfThread; ++i)
				threads.push_back(std::thread{ &ParticleManager::CalculateForce, this, 
					(unsigned int)(i*totalGrid / numOfThread),
					(unsigned int)((i + 1)*totalGrid / numOfThread) });
			for (auto& thr : threads)
				thr.join();
		}
		ProcessOut();
		AfterCalc();
	}
}

void ParticleManager::InitializeParticle()
{
	//1. 초기화
	for (unsigned short i = 0; i < maxType; i++)
	{
		unsigned int tCount = (*particles[i]).count();
		for (unsigned int j = 0; j < tCount; j++)
		{
			(*particles[i])[j].density = (*particles[i])[j].mass * getKernelValue(0);
			(*particles[i])[j].force = 0;
		}
	}
}

void ParticleManager::CalculateDensity(unsigned start, unsigned end)
{
	//0. 임시 변수 선언
	phys_t tempDist;

	//2. 동일 그리드 내 연산
	for (unsigned short typeFrom = 0; typeFrom < maxType; typeFrom++)
	{
		for (unsigned short typeTo = 0; typeTo < maxType; typeTo++)
		{//모든 종류에 대해
			if (particleInteraction[typeFrom][typeTo] || particleInteraction[typeTo][typeFrom])
			{//연산하는 관계일 경우
				for (unsigned int fromGrid = start; fromGrid < end; fromGrid++)
				{//모든 그리드에 대해
					unsigned int fpCount = (grid[typeFrom][fromGrid])->count();
					for (unsigned int fromParticle = 0; fromParticle < fpCount; fromParticle++)
					{//모든 파티클에 대해
						unsigned int& tempFrom = (*grid[typeFrom][fromGrid])[fromParticle];
						Particle& fParticle = (*particles[typeFrom])[tempFrom];
						if (typeFrom <= typeTo)
						{
							unsigned int tpCount = (*grid[typeTo][fromGrid]).count();
							unsigned int toParticle = 0;
							if (typeTo == typeFrom) { toParticle = 1 + fromParticle; }
							for (; toParticle < tpCount; toParticle++)
							{//다른 모든 파티클과
								unsigned int& tempTo = (*grid[typeTo][fromGrid])[toParticle];
								Particle& tParticle = (*particles[typeTo])[tempTo];
								tempDist = abs2(fParticle.position - tParticle.position);
								if (tempDist < KERNEL_DISTANCE_2)
								{//거리내에 있을 경우
									fParticle.density += tParticle.mass * getKernelValue(tempDist);
									tParticle.density += fParticle.mass * getKernelValue(tempDist);
									//연산
								}
							}
						}
						for (unsigned char neighborhood = 0; neighborhood < gridNear[fromGrid][0]; neighborhood++)
						{
							unsigned int tpCount = (*grid[typeTo][gridNear[fromGrid][neighborhood + 1]]).count();
							//모든 이웃 격자에 대해서
							for (unsigned int toParticle = 0; toParticle < tpCount; toParticle++)
							{//모든 다른 파티클과
								unsigned int& tempTo = (*grid[typeTo][gridNear[fromGrid][neighborhood + 1]])[toParticle];
								Particle& tParticle = (*particles[typeTo])[tempTo];
								tempDist = abs2(fParticle.position - tParticle.position);
								if (tempDist < KERNEL_DISTANCE_2)
								{//일정 거리 이하일 경우
									fParticle.density += tParticle.mass * getKernelValue(tempDist);
									tParticle.density += fParticle.mass * getKernelValue(tempDist);
									//연산
								}
							}
						}
					}
				}
			}
		}
	}
}


void ParticleManager::CalculateForce(unsigned start, unsigned end)
{
	//0. 임시 변수 선언
	Vector3 tempForce;



	//2. 자기 격자 내 연산
	for (unsigned short typeFrom = 0; typeFrom < maxType; typeFrom++)
	{
		for (unsigned short typeTo = 0; typeTo < maxType; typeTo++)
		{//모든 종류에 대해
			if (particleInteraction[typeFrom][typeTo] || particleInteraction[typeTo][typeFrom])
			{//연산하는 관계일 경우
				for (unsigned int fromGrid = start; fromGrid < end; fromGrid++)
				{
					unsigned int fpCount = (grid[typeFrom][fromGrid])->count();
					for (unsigned int fromParticle = 0; fromParticle < fpCount; fromParticle++)
					{//모든 파티클을
						unsigned int& tempFrom = (*grid[typeFrom][fromGrid])[fromParticle];
						Particle& fParticle = (*particles[typeFrom])[tempFrom];
						if (typeFrom <= typeTo)
						{
							unsigned int toParticle = 0;
							if (typeTo == typeFrom) { toParticle = fromParticle + 1; } //以묒꺽 ?놁씠. ?먭린 ?먯떊???꾨땲寃?
							for (; toParticle < (*grid[typeTo][fromGrid]).count(); toParticle++)
							{//다른 모든 파티클과
								unsigned int& tempTo = (*grid[typeTo][fromGrid])[toParticle];
								Particle& tParticle = (*particles[typeTo])[tempTo];
								phys_t dist2 = abs2((*particles[typeFrom])[tempFrom].position - (*particles[typeTo])[tempTo].position);
								if (dist2 < KERNEL_DISTANCE_2)
								{//일정 거리 이하일 경우 + 힘을 주는 관계일 경우

									tempForce = getKernelValue(dist2) * getForceValue(tParticle, fParticle);
									if (particleInteraction[typeTo][typeFrom])
									{
										fParticle.force -= tempForce * (*particles[typeTo])[tempTo].mass / (*particles[typeFrom])[tempFrom].density;
									}
									if (particleInteraction[typeFrom][typeTo])
									{
										tParticle.force += tempForce * (*particles[typeFrom])[tempFrom].mass / (*particles[typeTo])[tempTo].density;
									}
									//힘 연산
								}
							}
						}
						//3.타 그리드와 연산
						for (unsigned char neighborhood = 0; neighborhood < gridNear[fromGrid][0]; neighborhood++)
						{
							//모든 그리드의 이웃 격자에 대해
							for (unsigned int toParticle = 0; toParticle < (*grid[typeTo][gridNear[fromGrid][neighborhood + 1]]).count(); toParticle++)
							{//To 파티클에 대해
								unsigned int& tempTo = (*grid[typeTo][gridNear[fromGrid][neighborhood + 1]])[toParticle];
								Particle& tParticle = (*particles[typeTo])[tempTo];
								phys_t dist2 = abs2((*particles[typeFrom])[tempFrom].position - (*particles[typeTo])[tempTo].position);
								if (dist2 < KERNEL_DISTANCE_2)
								{//일정 거리 이내면 + 힘을 주는 관계면

									tempForce = getKernelValue(dist2) * getForceValue(tParticle, fParticle);
									if (particleInteraction[typeTo][typeFrom])
									{
										fParticle.force -= tempForce * (*particles[typeTo])[tempTo].mass / (*particles[typeFrom])[tempFrom].density;
									}
									if (particleInteraction[typeFrom][typeTo])
									{
										tParticle.force += tempForce * (*particles[typeFrom])[tempFrom].mass / (*particles[typeTo])[tempTo].density;
									}
									//힘연산
								}
							}
						}
					}
				}
			}
		}
	}
}

void ParticleManager::AfterCalc()
{
	for (unsigned short typeIndex = 0; typeIndex < maxType; typeIndex++)
	{
		processAfterCalc[typeIndex]((*particles[typeIndex]), this->unitTime);
		//processForce

		unsigned int pCount = (*particles[typeIndex]).count();
		for (unsigned short particleIndex = 0; particleIndex < pCount; particleIndex++)
		{

			Particle& temp = (*particles[typeIndex])[particleIndex];
			temp.position += temp.velocity*unitTime;
			//UpdatePosition

			RecheckGrid(typeIndex, (*particles[typeIndex])[particleIndex]);
			//RecheckGrid
		}
	}
}



void ParticleManager::RecheckGrid(unsigned short typeIndex, Particle& particle)
{
	if (particle.gridIndex != GetGridIndex(particle.position))
	{
		unsigned int particleIndex = (*grid[typeIndex][particle.gridIndex])[particle.Index];
		(*grid[typeIndex][particle.gridIndex]).pop(particle.Index);
		if (particle.Index != (*grid[typeIndex][particle.gridIndex]).count())
		{
			Particle& changedParticle = (*particles[typeIndex])[(*grid[typeIndex][particle.gridIndex])[particle.Index]];
			changedParticle.Index = particle.Index;
		}

		particle.gridIndex = GetGridIndex(particle.position);
		particle.Index = (*grid[typeIndex][particle.gridIndex]).count();
		(*grid[typeIndex][particle.gridIndex]).push(particleIndex);
	}
}

void ParticleManager::ProcessOut()
{
	if (processOutOfArea != nullptr)
	{

		for (unsigned short typeIndex = 0; typeIndex < maxType; typeIndex++)
		{
			for (unsigned int outGrid = 1; outGrid - 1 < gridOut[1][0]; outGrid++)
			{
				__BindedList<unsigned int>& targetGrid = (*grid[typeIndex][gridOut[1][outGrid]]);
				unsigned short particleNum = targetGrid.count();
				for (unsigned short particleInGrid = 0; particleInGrid < particleNum; particleInGrid++)
				{
					Particle& Parti = (*particles[typeIndex])[targetGrid[particleInGrid]];
					processOutOfArea(Parti, this->unitTime);
					
					if (!(maxPosition+WALL_PUSH_DISTANCE> Parti.position && 
						minPosition - WALL_PUSH_DISTANCE < Parti.position))
					{
						RemoveParticle(typeIndex, targetGrid[particleInGrid]);
						--particleNum;
						--particleInGrid;
					}
				}
			}
			for (unsigned int borderGrid = 1; borderGrid - 1 < gridOut[0][0]; borderGrid++)
			{
				__BindedList<unsigned int>& targetGrid = (*grid[typeIndex][gridOut[0][borderGrid]]);
				unsigned short particleNum = targetGrid.count();

				for (unsigned short particleInGrid = 0; particleInGrid < particleNum; particleInGrid++)
				{
					Particle& Parti = (*particles[typeIndex])[targetGrid[particleInGrid]];

					if (Parti.position.z < minPosition.z || Parti.position.z > maxPosition.z ||
						Parti.position.y < minPosition.y || Parti.position.y > maxPosition.y ||
						Parti.position.x < minPosition.x || Parti.position.x > maxPosition.x)
					{
						processOutOfArea(Parti, this->unitTime);
					}
				}
			}
		}
	}

}

void ParticleManager::CustomProcess()
{
	unsigned numOfFunc = (*CustomFunction).count();
	for (unsigned i = 0; i < numOfFunc; i++)
	{
		(*CustomFunction)[i].CustomFunc(*this);
	}
}

void ParticleManager::CreateParticle(unsigned short typeIndex, Particle& particle)
{
	if (!isCreated()) { printf("ERROR :: can't create particle. not created yet."); }
	else if (maxType <= typeIndex) { printf("ERROR :: exceed max particle type number"); }
	else if ((*particles[typeIndex]).count() == (*particles[typeIndex]).maxCount()) { printf("ERROR :: exceed max particle number"); }
	else
	{
		particle.gridIndex = GetGridIndex(particle.position);
		particle.Index = (*grid[typeIndex][particle.gridIndex]).count();
		(*particles[typeIndex]).push(particle);
		(*grid[typeIndex][particle.gridIndex]).push((*particles[typeIndex]).count() - 1);
	}
}
void ParticleManager::RemoveParticle(unsigned short typeIndex, unsigned int particleIndex)
{
	if (!isCreated()) { printf("ERROR :: can't create particle. not created yet."); }
	else if (maxType <= typeIndex) { printf("ERROR :: exceed max particle type number"); }
	else if ((*particles[typeIndex]).count() == 0) { printf("ERROR :: there is no particle"); }
	else
	{
		Particle& particle = (*particles[typeIndex])[particleIndex];
		(*grid[typeIndex][particle.gridIndex]).pop(particle.Index);
		if ((*grid[typeIndex][particle.gridIndex]).count() != particle.Index)
		{
			(*particles[typeIndex])[(*grid[typeIndex][particle.gridIndex])[particle.Index]].Index = particle.Index;
		}

		(*particles[typeIndex]).pop(particleIndex);
		if ((*particles[typeIndex]).count() != particleIndex)
		{
			Particle& changedParticle = (*particles[typeIndex])[particleIndex];
			(*grid[typeIndex][changedParticle.gridIndex])[changedParticle.Index] = particleIndex;
		}
	}
}

__BindedList<Particle>& ParticleManager::GetParticles(unsigned short typeIndex)
{
	if (!isCreated()) { printf("ERROR :: not created yet."); }
	else if (maxType <= typeIndex) { printf("ERROR :: exceed max particle type number"); }
	else
	{
		return (*particles[typeIndex]);
	}
	//return __BindedList<unsigned int>{0};
}

__BindedList<unsigned int>& ParticleManager::GetGrid(unsigned short typeIndex, unsigned int gridIndex)
{
	if (!isCreated()) { printf("ERROR :: not created yet."); }
	else if (maxType <= typeIndex) { printf("ERROR :: exceed max particle type number"); }
	else
	{
		return (*grid[typeIndex][gridIndex]);
	}
	return __BindedList<unsigned int>{0};
}
__BindedList<unsigned int>& ParticleManager::GetGrid(unsigned short typeIndex, Vector3 position)
{
	return GetGrid(typeIndex, GetGridIndex(position));
}
unsigned int*& ParticleManager::GetNearGridIndex(unsigned int gridIndex)
{
	if (!isCreated()) { printf("ERROR :: not created yet."); }
	else if (gridIndex >= totalGrid) { printf("ERROR :: exceed max grid number"); }
	else
	{
		return gridNear[gridIndex];
	}
}
unsigned int*& ParticleManager::GetNearGridIndex(Vector3 position)
{
	return GetNearGridIndex(GetGridIndex(position));
}

//--------------------------------------//
//	Data Probe							//
//--------------------------------------//

phys_t ParticleManager::GetSimpleDensity(unsigned short typeIndex) { return GetSimpleDensity(typeIndex, maxPosition, minPosition); }
phys_t ParticleManager::GetSimpleDensity(unsigned short typeIndex, Vector3 MaxPosition, Vector3 MinPosition)
{
	phys_t __density = 0.0;
	phys_t __volume = 0;

	if (MaxPosition == MinPosition)
	{
		return -1;
	}
	else
	{
		__vector3<unsigned short> maxIndex = GetGridIndex3(maxPosition);
		__vector3<unsigned short> minIndex = GetGridIndex3(minPosition);
		for (unsigned short gridZ = minIndex.z; gridZ <= maxIndex.z; gridZ++)
		{
			for (unsigned short gridY = minIndex.y; gridY < maxIndex.y; gridY++)
			{
				for (unsigned short gridX = minIndex.x; gridX < maxIndex.x; gridX++)
				{
					unsigned int gIndex = ConvertGridIndex(__vector3<unsigned short> {gridX, gridY, gridZ});
					unsigned short gCount = (*grid[typeIndex][gIndex]).count();
					if (gridZ == minIndex.z || gridZ == maxIndex.z || gridY == minIndex.y || gridY == maxIndex.y || gridX == minIndex.x || gridX == maxIndex.x)
					{
						for (unsigned short pIndex = 0; pIndex < gCount; pIndex++)
						{
							Particle& tParticle = (*particles[typeIndex])[(*grid[typeIndex][gIndex])[pIndex]];
							if (MinPosition.x < tParticle.position.x&&tParticle.position.x < MaxPosition.x&&
								MinPosition.y < tParticle.position.y&&tParticle.position.y < MaxPosition.y&&
								MinPosition.z < tParticle.position.z&&tParticle.position.z < MaxPosition.z)
							{
								__density += tParticle.mass;
							}
						}
					}
					else
					{
						for (unsigned short pIndex = 0; pIndex < gCount; pIndex++)
						{
							Particle& tParticle = (*particles[typeIndex])[(*grid[typeIndex][gIndex])[pIndex]];
							__density += tParticle.mass;
						}
					}
				}
			}
		}
	}
	__volume = (MaxPosition - MinPosition).x * (MaxPosition - MinPosition).y * (MaxPosition - MinPosition).z;
	return __density / __volume;
}

phys_t ParticleManager::GetProbeDensity(unsigned short typeIndex, phys_t ProbeDistance) { return GetProbeDensity(typeIndex, maxPosition, minPosition, ProbeDistance); }
phys_t ParticleManager::GetProbeDensity(unsigned short typeIndex, Vector3 Position)
{
	phys_t resultDensity = 0;

	unsigned short temp3dIndex = GetGridIndex(Position);

	unsigned int& neighborCount = gridNear[temp3dIndex][0];



	for (unsigned char neighborhood = 0; neighborhood < neighborCount; neighborhood++)
	{
		for (unsigned short typeTo = 0; typeTo < maxType; typeTo++)
		{
			if (particleInteraction[typeIndex][typeTo] || particleInteraction[typeTo][typeIndex])
			{
				for (unsigned int toParticle = 0; toParticle < (*grid[typeTo][gridNear[temp3dIndex][neighborhood + 1]]).count(); toParticle++)
				{
					unsigned int& tempTo = (*grid[typeTo][gridNear[temp3dIndex][neighborhood + 1]])[toParticle];
					const phys_t& dist = abs2(Position - (*particles[typeTo])[tempTo].position);
					if (dist < KERNEL_DISTANCE_2)
					{
						resultDensity += (*particles[typeTo])[tempTo].mass * getKernelValue(dist);
					}
				}
			}
		}
	}
	return resultDensity;
}

phys_t ParticleManager::GetProbeDensity(unsigned short typeIndex, Vector3 MaxPosition, Vector3 MinPosition, phys_t ProbeDistance)
{
	phys_t __density = 0.0;
	unsigned int __probeNum = 0;

	if (MaxPosition == MinPosition)
	{
		return GetProbeDensity(typeIndex, MaxPosition, ProbeDistance);
	}
	else
	{
		__vector3<unsigned short> numberProbe = (__vector3<unsigned short>)((MaxPosition - MinPosition) / 2 / ProbeDistance);
		Vector3 startPoint = ((MaxPosition + MinPosition) / 2) - ((Vector3)numberProbe*ProbeDistance);
		for (int _x = 0; _x < numberProbe.x * 2 + 1; _x++)
		{
			for (int _y = 0; _y < numberProbe.y * 2 + 1; _y++)
			{
				for (int _z = 0; _z < numberProbe.z * 2 + 1; _z++)
				{
					__density += GetProbeDensity(typeIndex, startPoint + Vector3(_x, _y, _z) * ProbeDistance);
				}
			}
		}
		numberProbe *= 2;
		numberProbe += 1;
		__probeNum = numberProbe.x * numberProbe.y * numberProbe.z;

		return __density / __probeNum;
	}
}

phys_t ParticleManager::ConvertDensityToPressure(phys_t Density)
{
	phys_t __pressure = 0.0;
	__pressure = (pow(Density / STEADY_STATE_DENSITY, 7) - 1) * 1.119;
	return __pressure;
}

//--------------------------------------//
//	Fixed Time Interface				//
//--------------------------------------//

void ParticleManager::PushCustomFunc(std::function<void(ParticleManager& TargetPM)> Func, unsigned ID, int Order)
{
	//printf("Call Push");
	unsigned index = (*CustomFunction).count();
	while (index > 0 && Order < (*CustomFunction)[index - 1].order)
	{
		index--;
	}//그냥 비교 써치

	FuncRack newElement;
	newElement.CustomFunc = Func;
	newElement.ID = ID;
	newElement.order = Order;
	(*CustomFunction).push(newElement, index);

	//(*CustomFunction).print();
}


void ParticleManager::PopCustomFunc(unsigned ID, int Order)
{
	unsigned index = (*CustomFunction).count() - 1;
	while (index >= 0)
	{
		if (Order == (*CustomFunction)[index].order && ID == (*CustomFunction)[index].ID)
		{
			(*CustomFunction).pop_noswap(index);
			break;
		}
		else if (Order > (*CustomFunction)[index].order)
		{
			throw BindedList_NoSuchEntity;
			break;
		}
		index--;
	}//그냥 비교 써치

	//(*CustomFunction).print();
}