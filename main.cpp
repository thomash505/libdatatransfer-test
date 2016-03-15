#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <future>
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
	using p2p_connector_type = p2p_connector<std::mutex, mock::UART, serialization_policy>;
	mock::UART uart1;
	mock::UART uart2;
	mock::CommunicationLink<mock::UART, char> commlink(uart1, uart2);
	static bool exit = false;
	bool stop = false;

	static DTO test;

	p2p_connector_type p2p_tx(uart1);
	p2p_connector_type p2p_rx(uart2);

	auto read_func = [&](p2p_connector_type& p2p)
	{
		while (!stop)
		{
			p2p.read();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	};

	auto p2p_tx_read_future = std::async(std::launch::async, [&] () { read_func(p2p_tx); });
	auto p2p_rx_read_future = std::async(std::launch::async, [&] () { read_func(p2p_rx); });

	auto handler = [] (const DTO& dto)
	{
		REQUIRE(dto.data == test.data);
		exit = true;
	};

	p2p_rx.registerMessageHandler<serialization_policy::TEST_MESSAGE_1>(handler);
	p2p_tx.send<serialization_policy::TEST_MESSAGE_1>(test);

	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	REQUIRE(exit == true);
	uart1.close();
	uart2.close();

	stop = true;
	p2p_tx_read_future.wait();
	p2p_rx_read_future.wait();
}

TEST_CASE()
{
	using namespace datatransfer;
	using p2p_connector_type = p2p_connector<std::mutex, mock::UART, serialization_policy>;
	mock::UART uart1;
	mock::UART uart2;
	mock::CommunicationLink<mock::UART, char> commlink(uart1, uart2);
	static bool exit = false;
	bool stop = false;

	static EigenTest test;
	Eigen::Map<Eigen::Vector3f>(test.vec3) = Eigen::Vector3f::Random();
	Eigen::Map<Eigen::Vector4f>(test.vec4) = Eigen::Vector4f::Random();

	p2p_connector_type p2p_tx(uart1);
	p2p_connector_type p2p_rx(uart2);

	auto read_func = [&](p2p_connector_type& p2p)
	{
		while (!stop)
		{
			p2p.read();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	};

	auto p2p_tx_read_future = std::async(std::launch::async, [&] () { read_func(p2p_tx); });
	auto p2p_rx_read_future = std::async(std::launch::async, [&] () { read_func(p2p_rx); });

	auto handler = [] (const EigenTest& data)
	{
		REQUIRE(Eigen::Map<const Eigen::Vector3f>(data.vec3) == Eigen::Map<Eigen::Vector3f>(test.vec3));
		REQUIRE(Eigen::Map<const Eigen::Vector4f>(data.vec4) == Eigen::Map<Eigen::Vector4f>(test.vec4));
		exit = true;
	};

	p2p_rx.registerMessageHandler<serialization_policy::TEST_MESSAGE_2>(handler);
	p2p_tx.send<serialization_policy::TEST_MESSAGE_2>(test);

	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	REQUIRE(exit == true);
	uart1.close();
	uart2.close();

	stop = true;
	p2p_tx_read_future.wait();
	p2p_rx_read_future.wait();
}
