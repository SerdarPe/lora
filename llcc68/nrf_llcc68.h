/**
 * @author SERDAR PEHLIVAN
 * @date 20/11/2025
 * @version 1.0
 *
 */

#ifndef __NRF_LLCC68_H__
#define __NRF_LLCC68_H__

#include "llcc68.h"

namespace LoRa
{
	class NRF_LLCC68 : LLCC68
	{
	public:
		NRF_LLCC68(const LLCC68_pins &pins, const LLCC68_config &config, std::unique_ptr<LoRa_SPI> spi, std::unique_ptr<LoRa_IO> io, std::unique_ptr<Device> device)
			: LLCC68(pins, config, std::move(spi), std::move(io), std::move(device)) {};

		virtual void send_packet(const uint8_t *packet, uint8_t size) override;

	protected:
		virtual bool init_llcc68() override;
	};
}

#endif // __NRF_LLCC68_H__