/**
 * @author SERDAR PEHLIVAN
 * @date 14/11/2025
 * @version 1.0
 *
 */

#ifndef __LLCC68_H__
#define __LLCC68_H__

#include <cstdint>
#include <memory>

#include "..\device.h"
#include "..\exception.h"
#include "..\lora_io.h"
#include "..\lora_spi.h"
#include "constants.h"
#include "opcodes.h"

namespace LoRa
{
	class LLCC68
	{
	public:
		virtual void send_packet(const uint8_t *packet, uint8_t size) = 0;
		void reset();
		void sleep(SleepConfig sleepConfig);
		/* Device defaults to 0x1D0F */
		void set_crc_poly(uint16_t crc16);
		inline ErrorCode get_last_error() const { return last_error; }

		/* rf_freq = ((desired_freq * (2^25)) / 32) */
		static uint32_t calculate_rf_frequency(uint32_t desired_freq);

		LLCC68(LLCC68 &&) = default;
		LLCC68 &operator=(LLCC68 &&) = default;

		LLCC68(const LLCC68 &) = delete;
		LLCC68 &operator=(const LLCC68 &) = delete;

		virtual ~LLCC68() = default;

	protected:
		LLCC68(const LLCC68_pins &pins, const LLCC68_config &config, std::unique_ptr<LoRa_SPI> spi, std::unique_ptr<LoRa_IO> io, std::unique_ptr<Device> device);

		virtual bool init_llcc68() = 0;

		void set_sleep(SleepConfig sleepConfig);
		void set_standby(LLCC68_Constants::StandbyConfig standbyConfig);
		/**
		 * @brief Set device in TX mode.
		 * @param timeout 24-bit timeout value multiplied by 15.625us. Pass 0 to disable timeout. */
		void set_tx(int32_t timeout = 6400);
		/**
		 * @brief Set device in RX mode
		 * @param timeout 24-bit timeout value multiplied by 15.625us. Pass 0 to disable timeout.
		 */
		void set_rx(int32_t timeout = 6400);
		void set_regulator_mode(LLCC68_Constants::RegModeParam regMode);
		/**
		 * @brief PA stands for power amplifier
		 */
		void set_pa_config(uint8_t paDutyCycle, uint8_t hpMax);
		void set_rx_tx_fallback_mode(LLCC68_Constants::FallbackMode fallbackMode);

		/**
		 * @brief Writes a continuous memory area, starting from given address.
		 * @param address Starting address.
		 * @param data Data to write.
		 * @param n Amount of bytes to write.
		 */
		void write_register(uint16_t address, uint8_t *data, uint8_t n);
		/**
		 * @brief Reads a continuous memory area, starting from given address.
		 * @param address Starting address.
		 * @param buffer Buffer to store read values. Make sure buffer size is at least n bytes.
		 * @param n Amount of bytes to read.
		 */
		void read_register(uint16_t address, uint8_t *buffer, uint8_t n);
		/**
		 * @brief Write n bytes to payload buffer.
		 * @param data Make sure data has at least n elements.
		 * @param n Amount of bytes to write. Device maximum is 255.
		 * @param offset Address offest within the device buffer.
		 */
		void write_buffer(const uint8_t *data, uint8_t n, uint8_t offset = 0);
		/**
		 * @brief Read n bytes from the payload buffer.
		 * @param buffer Byte array to hold read values. Make sure buffer is at least n bytes.
		 * @param n Amount of bytes to read.
		 * @param offset Address offset within the device buffer.
		 */
		void read_buffer(uint8_t *buffer, uint8_t n, uint8_t offset = 0);

		void set_dio_irq_params(IrqMask irqMask, IrqMask dio1_mask, IrqMask dio2_mask, IrqMask dio3_mask);
		IrqStatus get_irq_status();
		void clear_irq_status(LLCC68_Constants::ClearIrqParam clearIrqParam);
		void set_dio2_as_rf_switch_ctrl(LLCC68_Constants::Enable enable);
		void set_dio3_as_tcxo_ctrl(LLCC68_Constants::TCXO_VOLTAGE tcxoVoltage, int32_t delay);

		void set_rf_frequency(uint32_t rf_freq);
		void set_packet_type(LLCC68_Constants::PacketType protocol);
		/**
		 *  @param power_dbm must be between -9 and 22.
		 * */
		void set_tx_params(int8_t power_dbm, LLCC68_Constants::RampTime rampTime);
		void set_lora_modulation_params(LLCC68_Constants::SF sf, LLCC68_Constants::BW bw, LLCC68_Constants::CR cr, LLCC68_Constants::LDRO ldOpt);
		/**
		 * @param preambleLength number of LoRa symbols as preamble. Datasheet recommends at least 12 if using faster bitrates.
		 */
		void set_lora_packet_params(uint16_t preambleLength, LLCC68_Constants::HeaderType headerType, uint8_t payloadLength, LLCC68_Constants::CRC_Type crcType, LLCC68_Constants::InvertIQ invertIq);
		void set_buffer_base_address(uint8_t tx_base_addr, uint8_t rx_base_addr);

		void wait_for_irq_tx_done(int dio_pin);
		void wait_busy(int32_t timeout = -1);
		bool is_busy();

		ErrorCode last_error;
		LLCC68_pins pins;	  // Pin definitions
		LLCC68_config config; // Device config parameters
		std::unique_ptr<LoRa_SPI> _spi;
		std::unique_ptr<LoRa_IO> _io;
		std::unique_ptr<Device> _device;
	};
}

#endif //__LLCC68_H__