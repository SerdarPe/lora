/**
 * @author SERDAR PEHLIVAN
 * @date 18/11/2025
 * @version 1.0
 * 
 */

#ifndef __LORA_EXCEPTION_H__
#define __LORA_EXCEPTION_H__

namespace LoRa
{
	enum class ErrorCode
	{
		NO_ERROR = 0,
		TIMED_OUT,
		UNSUPPORTED
	};
}

#endif // __LORA_EXCEPTION_H__