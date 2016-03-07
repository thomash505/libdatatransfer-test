#ifndef MOCK_COMMUNICATIONLINK_H
#define MOCK_COMMUNICATIONLINK_H

#include <stdint.h>
#include <queue>
#include <thread>

namespace mock
{
	template <class IO, class SCALAR>
	class CommunicationLink
	{
	private:
		std::thread _thread;
		bool _exit;
		bool _started;
		IO& _io1;
		IO& _io2;

		void thread_main()
		{
			using namespace std::chrono;

			for (system_clock::time_point time = system_clock::now() += milliseconds(5); !_exit; time += milliseconds(5))
			{
				sync();
				std::this_thread::sleep_until(time);
			}
		}

	public:
		CommunicationLink(IO& io1, IO& io2)
			: _exit(false)
			, _started(false)
			, _io1(io1)
			, _io2(io2)
		{
			start();
		}

		void start()
		{
			if (!_started)
			{
				_started = true;
				_thread = std::thread(std::bind(&CommunicationLink::thread_main, this));
			}
		}

		void stop()
		{
			if (_started)
			{
				_exit = true;
				_thread.join();
				_exit = false;
				_started = false;
			}
		}

		void flush()
		{
			while (_io1.transmit_character() >= 0);
			while (_io2.transmit_character() >= 0);
		}

		~CommunicationLink()
		{
			if (_started)
			{
				_exit = true;
				_thread.join();
			}
		}

		void sync()
		{
			std::queue<SCALAR> queue1, queue2;

			for (int16_t c = _io1.transmit_character(); c >= 0; c = _io1.transmit_character())
			{
				queue1.push(c);
			}

			for (int16_t c = _io2.transmit_character(); c >= 0; c = _io2.transmit_character())
			{
				queue2.push(c);
			}

			for (; !queue1.empty(); queue1.pop())
			{
				_io2.receive_character(queue1.front());
			}

			for (; !queue2.empty(); queue2.pop())
			{
				_io1.receive_character(queue2.front());
			}
		}
	};
}

#endif // MOCK_COMMUNICATIONLINK_H
