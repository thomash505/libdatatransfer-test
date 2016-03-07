#ifndef MOCK_UART_HPP
#define MOCK_UART_HPP

#include <mutex>
#include <queue>
#include <deque>
#include <thread>

namespace mock {

class UART
{
public:
	using char_type = uint8_t;

private:
	std::mutex _mtx;
	std::queue<char_type> _write_queue;
	std::deque<char_type> _read_queue;
	uint8_t _last_read;
	bool _exit;

public:
	UART() : _exit(false) {}

	bool good() const { return true; }
	void close() { _exit = true; }
	void flush() {}

	int16_t transmit_character()
	{
		std::lock_guard<std::mutex> lock(_mtx);

		if (_write_queue.empty())
			return -1;
		else
		{
			char_type c = _write_queue.front();
			_write_queue.pop();
			return c;
		}
	}

	void receive_character(char_type c)
	{
		std::lock_guard<std::mutex> lock(_mtx);

		_read_queue.push_back(c);
	}

	void write(const char_type* buf, int len)
	{
		std::lock_guard<std::mutex> lock(_mtx);

		for (int i = 0; i < len; i++)
		{
			_write_queue.push(buf[i]);
		}
	}

	int16_t get()
	{
		read(reinterpret_cast<char_type*>(&_last_read), 1);

		return _last_read;
	}

	void ungetc()
	{
		putback(_last_read);
	}

	void putback(char_type c)
	{
		_read_queue.push_front(c);
	}

	int read(char_type* buf, int n)
	{
		for (int i = 0; (i < n) && !_exit;)
		{
			std::lock_guard<std::mutex> lock(_mtx);

			if (_read_queue.empty())
			{
				_mtx.unlock();
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				_mtx.lock();
			}
			else
			{
				buf[i++] = _read_queue.front();
				_read_queue.pop_front();
			}
		}

		_last_read = buf[n-1];

		return n;
	}
};

}

#endif // MOCK_UART_HPP

