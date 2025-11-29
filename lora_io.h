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
	constexpr uint8_t IO_LOW = 0;
	constexpr uint8_t IO_HIGH = 1;

	/**
	 * @brief Device specific GPIO functions.
	 */
	class LoRa_IO
	{
	public:
		virtual uint8_t read(const int pin) = 0;
		virtual void write(const int pin, const uint8_t value) = 0;

		virtual ~LoRa_IO() = default;

	protected:
		LoRa_IO() {};
	};
}

#endif // __LORA_IO_H__