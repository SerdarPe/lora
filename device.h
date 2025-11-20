/**
 * @author SERDAR PEHLIVAN
 * @date 18/11/2025
 * @version 1.0
 * 
 */

#ifndef __LORA_DEVICE_H__
#define __LORA_DEVICE_H__

#include <cstdint>

namespace LoRa
{
	/**
	 * @brief Device specific functions.
	 */
	class Device
	{
	public:
		virtual void delay(int32_t ms) = 0;
		virtual int32_t timestamp(void) = 0;
		virtual int64_t timestamp_64(void) = 0;
		
	};
}

#endif // __LORA_DEVICE_H__