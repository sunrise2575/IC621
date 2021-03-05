#pragma once

#include <Prelude.h>
#include _CH_VECTOR_
#include _CH_PARTICLE_
#include _CH_PARTICLEMANAGER_
#include _CH_PMFUNCTION_
#include _CH_FRAMEGL_
#include _CH_TREE_
#include _CH_WAVE_

//쉽게 바꿔보면서 놀 수 있게; Debug 폴더에서 실행해야 정상 동작함.
Object3D Teapot("Teapot.obj");
int CalcTime = 30;//밀리초단위
ParticleManager pm(UNIT_TIME,PARTICLE_TYPE_NUM);
pm_forcefield_cube Test1(pm);
pm_forcefield_cube Test2(pm);
pm_forcefield_cube Test3(pm);
void defaultOutOfAreaFunc(Particle& particle, phys_t& unit_time);
void JustForcefromWall(Particle& particle, phys_t& unit_time)
{
	Vector3 maxPosition = pm.GetMaxPosition() + WALL_PUSH_DISTANCE;
	Vector3 minPosition = pm.GetMinPosition() - WALL_PUSH_DISTANCE;
	Vector3& Position = particle.position;

	if (minPosition.z + WALL_PUSH_DISTANCE > Position.z) {
		particle.force.z +=
			WALL_PUSH_FORCE * pow(1 - ((Position.z - minPosition.z) / WALL_PUSH_DISTANCE), 2) * UNIT_TIME;
	}

	if (maxPosition.z - WALL_PUSH_DISTANCE < Position.z) {
		particle.force.z -=
			WALL_PUSH_FORCE * pow(1 + ((Position.z - maxPosition.z) / WALL_PUSH_DISTANCE), 2) * UNIT_TIME;
	}

	if (minPosition.y + WALL_PUSH_DISTANCE > Position.y) {
		particle.force.y +=
			WALL_PUSH_FORCE * pow(1 - ((Position.y - minPosition.y) / WALL_PUSH_DISTANCE), 2) * UNIT_TIME;
	}

	if (maxPosition.y - WALL_PUSH_DISTANCE < Position.y) {
		particle.force.y -=
			WALL_PUSH_FORCE * pow(1 + ((Position.y - maxPosition.y) / WALL_PUSH_DISTANCE), 2) * UNIT_TIME;
	}

	if (minPosition.x + WALL_PUSH_DISTANCE > Position.x) {
		particle.force.x +=
			WALL_PUSH_FORCE * pow(1 - ((Position.x - minPosition.x) / WALL_PUSH_DISTANCE), 2) * UNIT_TIME;
	}

	if (maxPosition.x - WALL_PUSH_DISTANCE < Position.x) {
		particle.force.x -=
			WALL_PUSH_FORCE * pow(1 + ((Position.x - maxPosition.x) / WALL_PUSH_DISTANCE), 2) * UNIT_TIME;
	}
	// 가상 벽 근처의 입자에게 힘을 가한다.
}

void nothing(__BindedList<Particle>& particle, phys_t& unit_time)
{
	unsigned numOfParticle = particle.count();
	for (unsigned i = 0; i < numOfParticle; i++)
	{
		particle[i].velocity = 0;
	}
}
//Particle get_particle(int i) { return pm[i]; }

void initialize()
{
	pm.SetMaxPosition(pm.GetMaxPosition()+Vector3(0,0,-100));
	pm.SetMinPosition(pm.GetMinPosition() + Vector3(0, -10, 0));
	pm.EnrollType(0, NUMBER_OF_PARTICLES + 20000, 500);
	pm.SetInteraction(true, 0, 0);
	pm.SetOutOfAreaFunction(JustForcefromWall);

	pm.EnrollType(1, 15000, 1000, nothing);
	pm.SetInteraction(true, 1, 0);
	pm.Start();
	for (int i = 0; i < NUMBER_OF_PARTICLES; i++)
	{
		Particle temp;
		temp.position = v3rand<phys_t>(pm.GetMaxPosition(), pm.GetMinPosition());
		temp.velocity = Vector3(0, 0, 0);
		temp.viscosity = 1;
		temp.mass = PARTICLE_MASS;
		pm.CreateParticle(0, temp);
	}

	for (int i = 0; i < 4*600; i++)
	{
		Particle temp;
		temp.position = (Vector3)(Util::v3Index(i, 30, 2) ) + Vector3(-15,-3, -10);
		temp.velocity = Vector3(0, 0, 0);
		temp.viscosity = 0.1;
		temp.mass = WALL_MASS;
		pm.CreateParticle(1, temp);
	}

	for (int i = 0; i < 240*4; i++)
	{
		Particle temp;
		temp.position = (Vector3)(Util::v3Index(i, 30, 1)) + Vector3(-15, -(i/240.0)-5, -65-50);
		temp.velocity = Vector3(0, 0, 0);
		temp.viscosity = 0.1;
		temp.mass = WALL_MASS;
		pm.CreateParticle(1, temp);
	}
	for (int i = 0; i < 240*4; i++)
	{
		Particle temp;
		temp.position = (Vector3)(Util::v3Index(i, 30, 1) ) + Vector3(-15, +(i / 240.0) - 13, -65-50);
		temp.velocity = Vector3(0, 0, 0);
		temp.viscosity = 0.1;
		temp.mass = WALL_MASS;
		pm.CreateParticle(1, temp);
	}
	for (int i = 0; i < 600*4; i++)
	{
		Particle temp;
		temp.position = (Vector3)(Util::v3Index(i, 30, 2) ) + Vector3(-15, -20, -10);
		temp.velocity = Vector3(0, 0, 0);
		temp.viscosity = 0.1;
		temp.mass = WALL_MASS;
		pm.CreateParticle(1, temp);
	}
/*	for (int i = 0; i < 600*4; i++)
	{
		Particle temp;
		temp.position = (Vector3)(Util::v3Index(i, 30, 2)) + Vector3(-15, -40, -50);
		temp.velocity = Vector3(0, 0, 0);
		temp.viscosity = 0.1;
		temp.mass = WALL_MASS;
		pm.CreateParticle(1, temp);
	}
	for (int i = 0; i < 600*4; i++)
	{
		Particle temp;
		temp.position = (Vector3)(Util::v3Index(i, 30, 20) ) + Vector3(-15, -40, -10);
		temp.velocity = Vector3(0, 0, 0);
		temp.viscosity = 0.1;
		temp.mass = WALL_MASS;
		pm.CreateParticle(1, temp);
	}*/
	Test1.SetArea(Vector3(20, 0, 50), Vector3(-20, -20, 10));
	Test1.SetForce(Vector3(0, 0, -0.01));
	Test1.Switch(true);
	Test2.SetArea(Vector3(20, 20, 50), Vector3(-20, 0, 30));
	Test2.SetForce(Vector3(0, -0.01, 0));
	Test2.Switch(true);
	Test3.SetArea(Vector3(20, -20, 50), Vector3(-20, -40, 30));
	Test3.SetForce(Vector3(0, 0.01, 0));
	Test3.Switch(true);
	Test1.Enroll();
	Test2.Enroll();
	Test3.Enroll();
}



void defaultOutOfAreaFunc(Particle& particle, phys_t& unit_time)
{
	Vector3 maxPosition = pm.GetMaxPosition();
	Vector3 minPosition = pm.GetMinPosition();

	while (particle.position.x > maxPosition.x || particle.position.x < minPosition.x)
	{
		if (particle.position.x > maxPosition.x)
		{
			particle.position.x = 2 * maxPosition.x - particle.position.x;
			particle.velocity.x = -particle.velocity.x * ELASTICITY;
		}
		else
		{
			particle.position.x = 2 * minPosition.x - particle.position.x;
			particle.velocity.x = -particle.velocity.x * ELASTICITY;
		}
	}
	while (particle.position.y > maxPosition.y || particle.position.y < minPosition.y)
	{
		if (particle.position.y > maxPosition.y)
		{
			particle.position.y = 2 * maxPosition.y - particle.position.y;
			particle.velocity.y = -particle.velocity.y * ELASTICITY;
		}
		else
		{
			particle.position.y = 2 * minPosition.y - particle.position.y;
			particle.velocity.y = -particle.velocity.y * ELASTICITY;
		}
	}
	while (particle.position.z > maxPosition.z || particle.position.z < minPosition.z)
	{
		if (particle.position.z > maxPosition.z)
		{
			particle.position.z = 2 * maxPosition.z - particle.position.z;
			particle.velocity.z = -particle.velocity.z * ELASTICITY;
		}
		else
		{
			particle.position.z = 2 * minPosition.z - particle.position.z;
			particle.velocity.z = -(particle.velocity.z) * ELASTICITY;// +(Z_MOVE - Z_DELTA) / UNIT_TIME;
		}
	}
	// 입자를 공간 안에 가둔다
}

void calculation()
{ 
	pm.Calculate();
	Particle test;
	for (int i = 0; i < 10; i++)
	{
		test.position = Vector3(rand() % 10000 / 10000.0 * 40 - 20, rand() % 10000 / 10000.0 * 20 - 20, rand() % 10000 / 10000.0 * 20 + 30);
		test.mass = PARTICLE_MASS;
		pm.CreateParticle(0, test);
	}
}

void main() {
	initialize();
	//pm.createWall(0, 0, WALL_MASS);
	

	//system("PAUSE");
	GLStartingFunc();
}

