#ifndef DTO_H
#define DTO_H

#include <stdint.h>
#include <Eigen/Core>

struct DTO
{
	uint64_t data;

	DTO() : data(0x0123456789ABCDEF) {}

	template <typename policy>
	void method(policy& p)
	{
		p % data;
	}
};

struct EigenTest
{
	float vec3[3];
	float vec4[4];

	template <typename policy>
	void method(policy& p)
	{
		p % vec3;
		p % vec4;
	}
};

#endif // DTO_H

