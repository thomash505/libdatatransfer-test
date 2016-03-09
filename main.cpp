#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <iostream>
#include <stdint.h>
#include <datatransfer/p2p_connector.hpp>
#include "mock/communicationlink.hpp"
#include "mock/uart.hpp"
#include "serialization_policy.hpp"

using namespace std;

struct WaitPolicy
{
	void wait(int ms)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	}
};

TEST_CASE()
{
	using namespace datatransfer;
	mock::UART uart1;
	mock::UART uart2;
	mock::CommunicationLink<mock::UART, char> commlink(uart1, uart2);
	bool exit = false;

	DTO test;

	p2p_connector<std::mutex, mock::UART, WaitPolicy, std::thread, serialization_policy> p2p_tx(uart1);
	p2p_connector<std::mutex, mock::UART, WaitPolicy, std::thread, serialization_policy> p2p_rx(uart2);

	auto handler = [&] (const void* dto)
	{
		REQUIRE(reinterpret_cast<const DTO*>(dto)->data == test.data);
		exit = true;
	};

	p2p_rx.registerMessageHandler<serialization_policy::TEST_MESSAGE_1>(handler);
	p2p_tx.send<serialization_policy::TEST_MESSAGE_1>(test);

	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	REQUIRE(exit == true);
	uart1.close();
	uart2.close();
}

TEST_CASE()
{
	using namespace datatransfer;
	mock::UART uart1;
	mock::UART uart2;
	mock::CommunicationLink<mock::UART, char> commlink(uart1, uart2);
	bool exit = false;

	EigenTest test;
	test.vec3 = Eigen::Vector3f::Random();
	test.vec4 = Eigen::Vector4f::Random();

	p2p_connector<std::mutex, mock::UART, WaitPolicy, std::thread, serialization_policy> p2p_tx(uart1);
	p2p_connector<std::mutex, mock::UART, WaitPolicy, std::thread, serialization_policy> p2p_rx(uart2);

	auto handler = [&] (const void* dto)
	{
		REQUIRE(reinterpret_cast<const EigenTest*>(dto)->vec3 == test.vec3);
		REQUIRE(reinterpret_cast<const EigenTest*>(dto)->vec4 == test.vec4);
		exit = true;
	};

	p2p_rx.registerMessageHandler<serialization_policy::TEST_MESSAGE_2>(handler);
	p2p_tx.send<serialization_policy::TEST_MESSAGE_2>(test);

	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	REQUIRE(exit == true);
	uart1.close();
	uart2.close();
}
