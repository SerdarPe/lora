/**
 * @author SERDAR PEHLIVAN
 * @date 18/11/2025
 * @version 1.0
 * 
 */

#ifndef __LORA_IO_H__
#define __LORA_IO_H__

#include <cstdint>

namespace LoRa
{
	typedef uint8_t (*_io_read)(const int pin);
	typedef void (*_io_write)(const int pin, const uint8_t value);

	constexpr uint8_t IO_LOW = 0;
	constexpr uint8_t IO_HIGH = 1;

	class LoRa_IO
	{
	public:
		LoRa_IO(_io_read fn_read, _io_write fn_write)
			: fn_read{fn_read}, fn_write(fn_write) {};
		inline uint8_t read(const int pin) { return fn_read(pin); };
		inline void write(const int pin, const uint8_t value) { return fn_write(pin, value); };

	private:
		_io_read fn_read;
		_io_write fn_write;
	};
}

#endif // __LORA_IO_H__