/**
 * @author SERDAR PEHLIVAN
 * @date 15/11/2025
 * @version 1.0
 *
 */

#ifndef __LORA_OPCODES_H__
#define __LORA_OPCODES_H__

#include <cstdint>

#include "constants.h"

namespace LoRa
{
	/* LLCC68 opcodes. Read DS_LLCC68_V1.0.pdf */
	enum OPCODE : uint8_t
	{
		NOP = 0x00,
		SET_SLEEP = 0x84,
		SET_STANDBY = 0x80,
		SET_FS = 0xC1,
		SET_TX = 0x83,
		SET_RX = 0x82,
		STOP_TIMER_ON_PREAMBLE = 0x9F,
		SET_RX_DUTY_CYCLE = 0x94,
		SET_CAD = 0xC5,
		SET_TX_CONTINIOUS_WAVE = 0xD1,
		SET_TX_INFINITE_PREAMBLE = 0xD2,
		SET_REGULATOR_MODE = 0x96,
		CALIBRATE = 0x89,
		CALIBRATE_IMAGE = 0x98,
		SET_PA_CONFIG = 0x95,
		SET_RX_TX_FALLBACK_MODE = 0x93,
		WRITE_REGISTER = 0x0D,
		READ_REGISTER = 0x1D,
		WRITE_BUFFER = 0x0E,
		READ_BUFFER = 0x1E,
		SET_DIO_IRQ_PARAMS = 0x08,
		GET_IRQ_STATUS = 0x12,
		CLEAR_IRQ_STATUS = 0x02,
		SET_DIO2_AS_RF_SWITCH_CTRL = 0x9D,
		SET_DIO3_AS_TCXO_CTRL = 0x97,
		SET_RF_FREQUENCY = 0x86,
		SET_PACKET_TYPE = 0x8A,
		GET_PACKET_TYPE = 0x11,
		SET_TX_PARAMS = 0x8E,
		SET_MODULATION_PARAMS = 0x8B,
		SET_PACKET_PARAMS = 0x8C,
		SET_CAD_PARAMS = 0x88,
		SET_BUFFER_BASE_ADDRESS = 0x8F,
		SET_LORA_SYMB_NUM_TIMEOUT = 0xA0,
		GET_STATUS = 0xC0,
		GET_RSSI_INST = 0x15,
		GET_RX_BUFFER_STATUS = 0x13,
		GET_PACKET_STATUS = 0x14,
		GET_DEVICE_ERRORS = 0x17,
		CLEAR_DEVICE_ERRORS = 0x07,
		GET_STATS = 0x10,
		RESET_STATS = 0x00,

	};

	typedef struct
	{
		uint8_t nss;	// chip select
		uint8_t nreset; // reset
		uint8_t busy;	// busy, active low
		uint8_t dio1;	// DIO_1
		uint8_t dio2;	// DIO_2
		uint8_t dio3;	// DIO_3

	} LLCC68_pins;

	typedef struct
	{
		uint32_t rf_freq;

		bool use_TCXO;
		LLCC68_Constants::Enable use_DIO2_as_rf_switch_ctrl;

		LLCC68_Constants::PacketType packet_type; /* Only LoRa is supported as of now */
		int8_t tx_power;

		union
		{
			struct
			{
				int32_t not_used;

			} _gfsk; /* FSK is not supported yet */

			struct
			{
				LLCC68_Constants::SF lora_sf;
				LLCC68_Constants::BW bandwidth;
				LLCC68_Constants::CR code_rate;
				LLCC68_Constants::LDRO ldro;

			} _lora;

		} modulation_params;

		union
		{
			struct
			{
				int32_t not_used;

			} _gfsk;

			struct
			{
				uint16_t preambleLength;
				LLCC68_Constants::HeaderType headerType;
				uint8_t payloadLength;
				LLCC68_Constants::CRC_Type crcType;
				LLCC68_Constants::InvertIQ invertIq;

			} _lora;

		} packet_params;

		struct
		{
			uint8_t paDutyCycle;
			uint8_t hpMax;

		} pa_config; /* Internal Power Amplifier config. Refer to the docs for optimal settitngs */

		struct
		{
			LLCC68_Constants::TCXO_VOLTAGE tcxoVoltage;
			int32_t delay; /* Only bits 24:0 is used. */

		} tcxo_settings;

		struct
		{
			int8_t power_dbm; /* Set between -9 and +22, integer type */
			LLCC68_Constants::RampTime rampTime;

		} tx_params;

	} LLCC68_config;

	typedef struct
	{
		uint8_t reserved : 5;
		LLCC68_Constants::SleepConfig_StartType start_type : 1; /**/
		uint8_t rfu : 1;										/* Always set to 0 */
		LLCC68_Constants::SleepConfig_Timeout rtc_timeout : 1;	/**/

	} SleepConfig;

	typedef struct
	{
		uint16_t tx_done : 1;
		uint16_t rx_done : 1;
		uint16_t preamble_detected : 1;
		uint16_t sync_word_valid : 1;
		uint16_t header_valid : 1;
		uint16_t header_err : 1;
		uint16_t crc_err : 1;
		uint16_t cad_done : 1;
		uint16_t cad_detected : 1;
		uint16_t timeout : 1;

	} IrqStatus, IrqMask;

} // namespace LoRa

#endif // __LORA_OPCODES_H__