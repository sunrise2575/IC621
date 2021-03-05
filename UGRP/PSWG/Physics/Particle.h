#pragma once

#include <Prelude.h>
#include _CH_UTILITY_
#include _CH_VECTOR_

#define KERNEL_DISTANCE_2 (KERNEL_DISTANCE*KERNEL_DISTANCE)
#define KERNEL_DISTANCE_4 (KERNEL_DISTANCE_2*KERNEL_DISTANCE_2)
#define KERNEL_DISTANCE_8 (KERNEL_DISTANCE_4*KERNEL_DISTANCE_4)

struct Particle;
inline Vector3 getForceValue(Particle& to, Particle& from);
inline phys_t getKernelValue(phys_t distance_2);

struct Particle {
	Vector3 force{0}, velocity{0}, position{0};
	phys_t density{ 0 }, mass{ 0 }, viscosity{ 0 };
	unsigned gridIndex{ 0 };//속한 그리드의 Index
	unsigned short Index{ 0 };//속한 그리드 내에서 Index
	unsigned short groupNumber{ 0 };//그래픽이나 기타 상위 개념 처리를 위한 데이터
};

inline phys_t getKernelValue(phys_t distance_2)
{
	return 315 * pow((KERNEL_DISTANCE_2 - distance_2), 3) / (64 * 3.14 * KERNEL_DISTANCE_8*KERNEL_DISTANCE);
}

inline Vector3 getForceValue(Particle& to, Particle& from)
{
	phys_t dist = abs(to.position - from.position);
	Vector3 resultForce;
	resultForce = (15 * BULK_MODULUS * (to.density + from.density)*(KERNEL_DISTANCE / dist - 1)* (to.position - from.position)) - (20 * VISCOSITY_COEFFICIENT * (to.velocity - from.velocity) * (to.viscosity + from.viscosity));
	//resultForce *= to.mass / (PI * KERNEL_DISTANCE_4 * from.density) * (1 - (dist / KERNEL_DISTANCE));
	resultForce /= (PI * KERNEL_DISTANCE_4 ) * (1 - (dist / KERNEL_DISTANCE));
	return resultForce;
}

