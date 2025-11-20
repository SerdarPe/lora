/**
 * @author SERDAR PEHLIVAN
 * @date 14/11/2025
 * @version 1.0
 *
 * LLCC68 Module constants
 */

#ifndef __LLCC68_CONTSTANTS_H__
#define __LLCC68_CONTSTANTS_H__

#include <cmath>
#include <cstdint>

namespace LoRa
{
	/**
	 *  Credits to: https://prosepoetrycode.potterpcs.net/2015/07/a-simple-constexpr-power-function-c/
	 *
	 * */
	template <typename T>
	static inline constexpr T ipow(T num, unsigned int pow)
	{
		return (pow >= sizeof(unsigned int) * 8) ? 0 : pow == 0 ? 1
																: num * ipow(num, pow - 1);
	}

	class LLCC68_Constants
	{
	public:
		static constexpr uint32_t pa_22_dbm = 0x04070001;
		static constexpr uint32_t pa_20_dbm = 0x03050001;
		static constexpr uint32_t pa_17_dbm = 0x02030001;
		static constexpr uint32_t pa_14_dbm = 0x02020001;

		/* F_XTAL = 32MHz */
		static constexpr double freq_step = 32e6 / ipow(2.0, 25.0);

		enum class TCXO_VOLTAGE : uint8_t
		{
			V_1_6 = 0x00,
			V_1_7 = 0x01,
			V_1_8 = 0x02,
			V_2_2 = 0x03,
			V_2_4 = 0x04,
			V_2_7 = 0x05,
			V_3_0 = 0x06,
			V_3_3 = 0x07

		};

		enum class PacketType : uint8_t
		{
			GFSK = 0,
			LORA = 1

		};

		enum class StandbyConfig : uint8_t
		{
			STDBY_RC = 0,
			STDBY_XOSC = 1

		};

		enum class RegModeParam : uint8_t
		{
			LDO = 0,
			DC_DC_LDO = 1

		};

		/* DIO2 As RF Switch */
		enum class Enable : uint8_t
		{
			FALSE = 0,
			TRUE = 1

		};

		enum class RampTime : uint8_t
		{
			SET_RAMP_10U = 0x00,
			SET_RAMP_20U = 0x01,
			SET_RAMP_40U = 0x02,
			SET_RAMP_80U = 0x03,
			SET_RAMP_200U = 0x04,
			SET_RAMP_800U = 0x05,
			SET_RAMP_1700U = 0x06,
			SET_RAMP_3400U = 0x07

		};

		enum class SF : uint8_t
		{
			SF5 = 0x05,
			SF6 = 0x06,
			SF7 = 0x07,
			SF8 = 0x08,
			SF9 = 0x09,
			SF10 = 0x0A,
			SF11 = 0x0B

		};

		enum class BW : uint8_t
		{
			LORA_BW_125 = 0x04,
			LORA_BW_250 = 0x05,
			LORA_BW_500 = 0x06

		};

		enum class CR : uint8_t
		{
			LORA_CR_4_5 = 0x01,
			LORA_CR_4_6 = 0x02,
			LORA_CR_4_7 = 0x03,
			LORA_CR_4_8 = 0x04

		};

		enum class LDRO : uint8_t
		{
			OFF = 0,
			ON = 1

		};

		/* LoRa Packet header type */
		enum class HeaderType : uint8_t
		{
			/* Variable length packet */
			EXPLICIT_HEADER = 0x00,
			/* Fixed length packet */
			IMPLICIT_HEADER = 0x01

		};

		enum class CRC_Type : uint8_t
		{
			CRC_OFF = 0x00,
			CRC_ON = 0x01

		};

		enum class InvertIQ : uint8_t
		{
			STANDARD_IQ = 0x00,
			INVERTED_IQ = 0x01

		};

		enum class SleepConfig_StartType : uint8_t
		{
			COLD_START = 0,
			WARM_START = 1
		};

		enum class SleepConfig_Timeout : uint8_t
		{
			TIMEOUT_DISABLED = 0,
			TIMEOUT_ENABLED = 1

		};

		enum class FallbackMode : uint8_t
		{
			FS = 0x40,
			STDBY_XOSC = 0x30,
			STDBY_RC = 0x20

		};

		enum class ClearIrqParam : uint16_t
		{
			TxDone = 1 << 0,
			RxDone = 1 << 1,
			PreambleDetected = 1 << 2,
			SyncWordValid = 1 << 3,
			HeaderValid = 1 << 4,
			HeaderErr = 1 << 5,
			CrcErr = 1 << 6,
			CadDone = 1 << 7,
			CadDetected = 1 << 8,
			Timeout = 1 << 9

		};
	};
}

#endif // __LLCC68_CONTSTANTS_H__