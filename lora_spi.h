/**
 * @author SERDAR PEHLIVAN
 * @date 15/11/2025
 * @version 1.0
 *
 * This file contains abstract SPI interface.
 */

#ifndef __LoRa_SPI_H__
#define __LoRa_SPI_H__

#include <cstdint>

namespace LoRa
{
	class LoRa_SPI
	{
	public:
		/**
		 * @brief Writes and reads a byte.
		 * @param value 
		 * @return 
		 */
		virtual uint8_t transfer(uint8_t value) = 0;
		/**
		 * @brief Writes the data and put back the read values in the data
		 * @param data 
		 * @param size 
		 */
		virtual void transfer(uint8_t *data, uint8_t size) = 0;
		/**
		 * @brief Writes the data but doesn't modify the data. In other words, discards the read values.
		 * @param data 
		 * @param size 
		 */
		virtual void transfer(const uint8_t *data, uint8_t size) = 0;
		virtual int8_t transfer(int8_t value) = 0;
		virtual void transfer(int8_t *data, uint8_t size) = 0;
		virtual void transfer(const int8_t *data, uint8_t size) = 0;
		uint16_t transfer(uint16_t value);
		uint32_t transfer(uint32_t value);
		uint64_t transfer(uint64_t value);
		int16_t transfer(int16_t value);
		int32_t transfer(int32_t value);
		int64_t transfer(int64_t value);
		virtual void set_bit_order(bool msb_first = true) = 0;
		inline bool is_bit_order_msb_first() const { return bit_order_msb_first; };
		bool is_device_little_endian();

		virtual ~LoRa_SPI() = default;
	protected:
		bool bit_order_msb_first = true;
	};
}

#endif // __LoRa_SPI_H__