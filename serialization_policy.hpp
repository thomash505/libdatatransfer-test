#ifndef SERIALIZATION_POLICY_HPP
#define SERIALIZATION_POLICY_HPP

#include <datatransfer/binary_serialization.hpp>
#include "dto.h"

struct serialization_policy
{
	enum { INVALID_MESSAGE = 0,
		   TEST_MESSAGE_1,
		   TEST_MESSAGE_2,
		   END_OF_MESSAGES
		 };
	enum { NUMBER_OF_MESSAGES = END_OF_MESSAGES - 1 };
	enum { MAX_MESSAGE_SIZE = 1024 };

	template <typename input_output_stream>
	struct serialization
	{
		using write_policy = datatransfer::binary_serialization::write_policy<input_output_stream>;
		using read_policy = datatransfer::binary_serialization::read_policy<typename input_output_stream::char_type, MAX_MESSAGE_SIZE>;
		using checksum_policy = datatransfer::binary_serialization::checksum_policy;
		using size_policy = datatransfer::binary_serialization::size_policy;
	};

	template <int N>
	struct data
	{
		using type = void;
		static constexpr size_t length = 0;
	};

	static constexpr bool valid(int N) { return N > 0 && N < END_OF_MESSAGES; }
};

template<>
struct serialization_policy::data<serialization_policy::TEST_MESSAGE_1>
{
	using type = DTO;
	static constexpr size_t length = sizeof(type);
};

template<>
struct serialization_policy::data<serialization_policy::TEST_MESSAGE_2>
{
	using type = EigenTest;
	static constexpr size_t length = sizeof(type);
};

#endif // SERIALIZATION_POLICY_HPP

