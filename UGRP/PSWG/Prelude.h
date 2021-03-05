#pragma once

////////////////////
// BASIC HEADERS  //
////////////////////

// c99
#include <time.h>
#include <math.h>

// Input&output
#include <iostream>
#include <ostream>
#include <istream>

// STL
#include <vector>
#include <deque>
#include <functional>

// File
#include <fstream>

// Parsing
#include <string>
#include <sstream>

// Others
#include <random>
#include <cstdlib>
#include <utility>

// Thread
#include <thread>

////////////////////
// CUSTOM HEADERS //
////////////////////

// Graphics
#define _CH_TRANSITION_ <Graphics/Object/Transition.h>

#define _CH_GUI_ <Graphics/FrameGL.h>
#define _CH_GLUT_ <Graphics/API/glut.h>
#define _CH_MOUSE_ <Graphics/UserInput/Mouse.h>
#define _CH_GUIINPUT_ <Graphics/UserInput/GUIInput.h>

#define _CH_ADVANCEDCAMERA_ <Graphics/Draw/Camera/AdvancedCamera.h>
#define _CH_CAMERA_ <Graphics/Draw/Camera/Camera.h>

#define _CH_DRAW_ <Graphics/Draw/Draw.h>

#define _CH_FRAME_ <Graphics/Window/Frame/Frame.h>

#define _CH_FRAMEGL_ <Graphics/FrameGL.h>
#define _CH_OBJECT_ <Graphics/Object.h>

// Math
#define _CH_MATRIX_ <Math/Matrix.h>
#define _CH_QUATERNION_ <Math/Quaternion.h>
#define _CH_VECTOR_ <Math/Vector.h>
#define _CH_TREE_ <Math/Tree.h>
#define _CH_BINDEDLIST_ <Math/BindedList.h>

// Physics
#define _CH_PARTICLE_ <Physics/Particle.h>
#define _CH_PARTICLEMANAGER_ <Physics/ParticleManager.h>

#define _CH_PMFUNCTION_ <Physics/PMFunction.h>

#define _CH_UTILITY_ <Physics/Utility.h>

// Sound
#define _CH_WAVE_ <Sound/Wave.h>

////////////////////
// SIMULATION     //
////////////////////

#define UNIT_TIME 0.5

#define NUMBER_OF_PARTICLES 10000 //입자의 개수
#define SIMULATION_DESIRE_DENSITY 2*STEADY_STATE_DENSITY//대략적인 원하는 시뮬레이션 내 압력
#define KERNEL_DISTANCE 10 // m

#define STEADY_STATE_DENSITY 1.184 // kg/m^3
#define VISCOSITY_COEFFICIENT 18.27 // Pa*sec
#define BULK_MODULUS 101.000 // kPa
#define BALENCING_RATIO 1.1// 밀도 계산 시 임시 커널 도입으로 아래의 식으로 기압이 제대로 맞춰지지 않아 보정하기 위한 부분

#define NUMBER_OF_GRID_X 1 // 그리드 개수 (X축)
#define NUMBER_OF_GRID_Y 10 // 그리드 개수 (Y축)
#define NUMBER_OF_GRID_Z 30 // 그리드 개수 (Z축)

#define MAX_X (NUMBER_OF_GRID_X/2.0 * KERNEL_DISTANCE) // 입자의 X좌표 최댓값
#define MIN_X (-NUMBER_OF_GRID_X/2.0* KERNEL_DISTANCE) // 입자의 X좌표 최솟값
#define MAX_Y (NUMBER_OF_GRID_Y/2.0 * KERNEL_DISTANCE) // 입자의 Y좌표 최댓값
#define MIN_Y (-NUMBER_OF_GRID_Y/2.0 * KERNEL_DISTANCE) // 입자의 Y좌표 최솟값
#define MAX_Z (NUMBER_OF_GRID_Z/2.0 * KERNEL_DISTANCE) // 입자의 Z좌표 최댓값
#define MIN_Z (-NUMBER_OF_GRID_Z/2.0 * KERNEL_DISTANCE) // 입자의 Z좌표 최솟값

#define ELASTICITY 0.05// just test;
#define NUMBER_OF_WALLS 20000
#define WALL_PUSH_FORCE 2
#define WALL_PUSH_DISTANCE 10

#define PARTICLE_MASS 0.1 // (BALENCING_RATIO*SIMULATION_DESIRE_DENSITY*NUMBER_OF_GRID_X*NUMBER_OF_GRID_Y*NUMBER_OF_GRID_Z*pow(KERNEL_DISTANCE, 3)*1.0/NUMBER_OF_PARTICLES) // kg/m^3
#define WALL_MASS  0.05

////////////////////
// PARTICLE       //
////////////////////

#define PARTICLE_TYPE_NUM 2

////////////////////
// TYPEDEF        //
////////////////////

typedef double phys_t;

////////////////////
// ERRORS         //
////////////////////

enum Errors {
	Matrix2_SizeIsZero,
	Matrix2_SizeMismatched,

	BindedList_NoSuchEntity,
	BindedList_NoMoreSpace,
	BindedList_JumpingInsert,

	Grid_ParticleIndexOutOfRange,
	Grid_WallIndexOutOfRange,
	Grid_NoParticleType,
	Grid_NoSuchParticle,
	Grid_NoParticleInside,
	Particles_OutOfIndex,
	Particles_NoSuchParticle,
	Particle_NoSuchParticle
};