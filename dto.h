#ifndef DTO_H
#define DTO_H

#include <stdint.h>
#include <Eigen/Core>

struct DTO
{
	uint64_t data;

	DTO() : data(0x0123456789ABCDEF) {}

	template <typename policy>
	void method(typename policy::stream_type& stream)
	{
		policy p(stream);
		p % data;
	}
};

struct EigenTest
{
	Eigen::Vector3f vec3;
	Eigen::Vector4f vec4;

	template <typename policy>
	void method(typename policy::stream_type& stream)
	{
		policy p(stream);
		p % vec3;
		p % vec4;
	}
};

#endif // DTO_H

