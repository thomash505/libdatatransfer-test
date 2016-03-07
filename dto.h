#ifndef DTO_H
#define DTO_H

#include <stdint.h>

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

#endif // DTO_H

