#pragma once

#include <Prelude.h>
#include _CH_PARTICLEMANAGER_

/*
대부분의 동작.

1. 영역의 산출 (그리드 단위)
2. 그리드 분리 기록 (세부 검사가 필요한 그리드, 그냥 포함되는 그리드)
3. 내부에서 수행할 활동.

이걸 이용해서 공통된 프레임 워크를 짤 수 있을 듯 하지만. 아직은 잘 모르겠으므로 걍 기본적인 것들만 짬


struct PM_EffectArea
{
unsigned int* BorderGrid;
unsigned int* InsideGrid;
void(*func)(Particle& particle);
};

*/

#define _PM_CREATE_ORDER_ -100
#define _PM_DELETE_ORDER_ 0
#define _PM_FORCE_ORDER_ 100

class pm_forcefield_cube;
class pm_emitter_cube;
class pm_drower_cube;

#include "PM function\PMForcefieldCube.h"