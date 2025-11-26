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
		virtual void begin_transfer() = 0;
		virtual void end_transfer() = 0;

		/**
		 * @brief Writes and reads a byte.
		 * @param value
		 * @return
		 */
		virtual uint8_t transfer(uint8_t value) = 0;
		/**
		 * @brief Writes the data and puts back the read values in the data
		 * @param data
		 * @param size
		 */
		virtual void transfer(uint8_t *data, uint8_t size) = 0;
		/**
		 * @brief Writes the data but doesn't modifies the data. In other words, discards the read values.
		 * @param data
		 * @param size
		 */
		virtual void transfer(const uint8_t *data, uint8_t size) = 0;

		virtual void set_bit_order(bool msb_first = true) = 0;
		inline bool is_bit_order_msb_first() const { return bit_order_msb_first; };

		virtual ~LoRa_SPI() = default;

	protected:
		LoRa_SPI(int pin_cipo, int pin_copi, int pin_sclk, int pin_cs)
			: pin_cipo{pin_cipo}, pin_copi{pin_copi}, pin_sclk{pin_sclk}, pin_cs{pin_cs} {};

		bool bit_order_msb_first = true;

		int pin_cipo;
		int pin_copi;
		int pin_sclk;
		int pin_cs;
	};
}

#endif // __LoRa_SPI_H__