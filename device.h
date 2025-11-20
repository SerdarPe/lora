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
	typedef void (*_delay)(int32_t ms);
	typedef int32_t (*_timestamp)();
	typedef int64_t (*_timestamp_64)();

	class Device
	{
	public:
		Device(_delay fn_delay, _timestamp fn_ts, _timestamp_64 fn_ts_64)
			: fn_delay{fn_delay}, fn_timestamp{fn_ts}, fn_timestamp_64{fn_ts_64} {};
		inline void delay(int32_t ms) const { fn_delay(ms); };
		inline int32_t timestamp(void) const { return fn_timestamp(); };
		inline int64_t timestamp_64(void) const { return fn_timestamp_64(); };

	private:
		_delay fn_delay;
		_timestamp fn_timestamp;
		_timestamp_64 fn_timestamp_64;
	};
}

#endif // __LORA_DEVICE_H__