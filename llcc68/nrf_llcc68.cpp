/**
 * @author SERDAR PEHLIVAN
 * @date 14/11/2025
 * @version 1.0
 *
 */

#include "nrf_llcc68.h"
#include "opcodes.h"
#include <cassert>

using LoRa::NRF_LLCC68;

NRF_LLCC68::NRF_LLCC68(const LLCC68_pins &pins,
					   const LLCC68_config &config, 
					   std::unique_ptr<LoRa_SPI> spi,
					   const LoRa_IO &io_config, 
					   const Device &device)
	: last_error{ErrorCode::NO_ERROR}, pins{pins}, config{config}, _spi{std::move(spi)}, _io{io_config}, _device{device}
{
	if (!_spi->is_bit_order_msb_first())
	{
		_spi->set_bit_order(true);
	}

	init_llcc68();
}

void LoRa::NRF_LLCC68::send_packet(const uint8_t *packet, uint8_t size)
{
	if (size == 0)
	{
		return;
	}

	/**
	 * TODO:
	 * Get device status.
	 * Wait for RX.
	 */

	wait_busy();
	set_standby(LLCC68_Constants::StandbyConfig::STDBY_RC);
	write_buffer(packet, size);

	IrqMask irqMask{1};
	IrqMask dio1_mask{1};
	IrqMask no_mask;
	set_dio_irq_params(irqMask, dio1_mask, no_mask, no_mask);

	set_tx();
	wait_for_irq_tx_done();
	// TODO: Check for device error
	clear_irq_status(LLCC68_Constants::ClearIrqParam::TxDone);

	set_rx(0x00FFFFFF); // Not sure about when to return to rx mode
						// No IRQ set
}

void LoRa::NRF_LLCC68::reset()
{
	_io.write(pins.nreset, IO_LOW);
	_device.delay(2); // Official demo value
	_io.write(pins.nreset, IO_HIGH);
	_device.delay(20); // Official demo value
}

void LoRa::NRF_LLCC68::sleep(SleepConfig sleepConfig)
{
	set_sleep(sleepConfig);
}

void LoRa::NRF_LLCC68::set_standby(LLCC68_Constants::StandbyConfig s)
{
	wait_busy();

	nss_low();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_STANDBY));
	_spi->transfer(static_cast<uint8_t>(s));
	nss_high();
}

uint32_t LoRa::NRF_LLCC68::calculate_rf_frequency(uint32_t desired_freq)
{
	return std::floor(static_cast<double>(desired_freq) / LLCC68_Constants::freq_step);
}

bool LoRa::NRF_LLCC68::init_llcc68()
{
	using LoRa::LLCC68_Constants;

	if (config.packet_type != LLCC68_Constants::PacketType::LORA)
	{
		/* GFSK is not supported yet */
		last_error = ErrorCode::UNSUPPORTED;
		return false;
	}

	set_standby(LLCC68_Constants::StandbyConfig::STDBY_RC);
	set_regulator_mode(LLCC68_Constants::RegModeParam::DC_DC_LDO);
	set_pa_config(config.pa_config.paDutyCycle, config.pa_config.hpMax);
	if (config.use_TCXO)
	{
		set_dio3_as_tcxo_ctrl(config.tcxo_settings.tcxoVoltage, config.tcxo_settings.delay);
	}
	set_dio2_as_rf_switch_ctrl(config.use_DIO2_as_rf_switch_ctrl);

	set_packet_type(config.packet_type);
	set_rf_frequency(calculate_rf_frequency(config.rf_freq));
	set_tx_params(config.tx_params.power_dbm, config.tx_params.rampTime);
	set_buffer_base_address(0, 0); // TODO: Check datasheet
	if (config.packet_type == LLCC68_Constants::PacketType::LORA)
	{
		set_lora_modulation_params(config.modulation_params._lora.lora_sf,
								   config.modulation_params._lora.bandwidth,
								   config.modulation_params._lora.code_rate,
								   config.modulation_params._lora.ldro);
		set_lora_packet_params(config.packet_params._lora.preambleLength,
							   config.packet_params._lora.headerType,
							   config.packet_params._lora.payloadLength,
							   config.packet_params._lora.crcType,
							   config.packet_params._lora.invertIq);
	}

	return true;
}

void LoRa::NRF_LLCC68::set_sleep(SleepConfig sleepConfig)
{
	wait_busy();

	nss_low();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_SLEEP));
	_spi->transfer(*reinterpret_cast<uint8_t *>(&sleepConfig));
	nss_high();
}

void LoRa::NRF_LLCC68::set_packet_type(LLCC68_Constants::PacketType protocol)
{
	if (protocol != LLCC68_Constants::PacketType::LORA)
	{
		// GFSK is not supported yet, and exceptions are not enabled
		return;
	}

	wait_busy();

	nss_low();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_PACKET_TYPE));
	_spi->transfer(static_cast<uint8_t>(protocol));
	nss_high();
}

void LoRa::NRF_LLCC68::set_lora_modulation_params(LLCC68_Constants::SF sf, LLCC68_Constants::BW bw, LLCC68_Constants::CR cr, LLCC68_Constants::LDRO ldOpt)
{
	wait_busy();

	nss_low();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_MODULATION_PARAMS));
	_spi->transfer(static_cast<uint8_t>(sf));
	_spi->transfer(static_cast<uint8_t>(bw));
	_spi->transfer(static_cast<uint8_t>(cr));
	_spi->transfer(static_cast<uint8_t>(ldOpt));
	nss_high();
}

void LoRa::NRF_LLCC68::set_tx(int32_t timeout)
{
	wait_busy();

	timeout = timeout & 0x00FFFFFF;

	nss_low();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_TX));
	_spi->transfer(static_cast<uint8_t>((timeout & 0x00FF0000) >> 16));
	_spi->transfer(static_cast<uint8_t>((timeout & 0x0000FF00) >> 8));
	_spi->transfer(static_cast<uint8_t>(timeout & 0x000000FF));
	nss_high();
}

void LoRa::NRF_LLCC68::set_rx(int32_t timeout)
{
	wait_busy();

	timeout = timeout & 0x00FFFFFF;

	nss_low();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_RX));
	_spi->transfer(static_cast<uint8_t>((timeout & 0x00FF0000) >> 16));
	_spi->transfer(static_cast<uint8_t>((timeout & 0x0000FF00) >> 8));
	_spi->transfer(static_cast<uint8_t>(timeout & 0x000000FF));
	nss_high();
}

void LoRa::NRF_LLCC68::set_regulator_mode(
	LLCC68_Constants::RegModeParam regMode)
{
	wait_busy();

	nss_low();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_REGULATOR_MODE));
	_spi->transfer(static_cast<uint8_t>(regMode));
	nss_high();
}

void LoRa::NRF_LLCC68::set_pa_config(uint8_t paDutyCycle, uint8_t hpMax)
{
	wait_busy();

	uint8_t deviceSel = 0x00; // Reserved value
	uint8_t paLut = 0x01;	  // Reserved value

	nss_low();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_PA_CONFIG));
	_spi->transfer(paDutyCycle);
	_spi->transfer(hpMax);
	_spi->transfer(deviceSel);
	_spi->transfer(paLut);
	nss_high();
}

void LoRa::NRF_LLCC68::set_dio3_as_tcxo_ctrl(
	LLCC68_Constants::TCXO_VOLTAGE tcxoVoltage, int32_t delay)
{
	wait_busy();

	if (delay == -1)
		delay = 0x64;

	delay = delay & 0x00FFFFFF;

	nss_low();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_DIO3_AS_TCXO_CTRL));
	_spi->transfer(static_cast<uint8_t>((delay & 0x00FF0000) >> 16));
	_spi->transfer(static_cast<uint8_t>((delay & 0x0000FF00) >> 8));
	_spi->transfer(static_cast<uint8_t>(delay & 0x000000FF));
	nss_high();
}

void LoRa::NRF_LLCC68::set_dio2_as_rf_switch_ctrl(
	LLCC68_Constants::Enable enable)
{
	wait_busy();

	nss_low();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_DIO2_AS_RF_SWITCH_CTRL));
	_spi->transfer(static_cast<uint8_t>(enable));
	nss_high();
}

void LoRa::NRF_LLCC68::set_rf_frequency(uint32_t rf_freq)
{
	wait_busy();

	nss_low();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_RF_FREQUENCY));
	_spi->transfer(rf_freq);
	nss_high();
}

void LoRa::NRF_LLCC68::set_tx_params(int8_t power_dbm,
									 LLCC68_Constants::RampTime rampTime)
{
	assert((power_dbm >= -9) && (power_dbm <= 22));
	wait_busy();

	nss_low();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_TX_PARAMS));
	_spi->transfer(power_dbm);
	_spi->transfer(static_cast<uint8_t>(rampTime));
	nss_high();
}

void LoRa::NRF_LLCC68::set_lora_packet_params(
	uint16_t preambleLength, LLCC68_Constants::HeaderType headerType, uint8_t payloadLength, LLCC68_Constants::CRC_Type crcType, LLCC68_Constants::InvertIQ invertIq)
{
	wait_busy();

	nss_low();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_PACKET_PARAMS));
	_spi->transfer(static_cast<uint8_t>((preambleLength & 0xFF00) >> 8));
	_spi->transfer(static_cast<uint8_t>(preambleLength & 0x00FF));
	_spi->transfer(static_cast<uint8_t>(headerType));
	_spi->transfer(payloadLength);
	_spi->transfer(static_cast<uint8_t>(crcType));
	_spi->transfer(static_cast<uint8_t>(invertIq));
	nss_high();
}

void LoRa::NRF_LLCC68::set_buffer_base_address(uint8_t tx_base_addr,
											   uint8_t rx_base_addr)
{
	wait_busy();

	nss_low();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_BUFFER_BASE_ADDRESS));
	_spi->transfer(tx_base_addr);
	_spi->transfer(rx_base_addr);
	nss_high();
}

void LoRa::NRF_LLCC68::wait_for_irq_tx_done()
{
	using LoRa::LLCC68_Constants;

	uint8_t n_timeout = 0;

	while (!_io.read(pins.dio1)) // TODO: Check datasheet
	{
		n_timeout++;
		_device.delay(1);
		if (n_timeout > 200)
		{
			last_error = ErrorCode::TIMED_OUT;
			return;
		}
	}
}

void LoRa::NRF_LLCC68::wait_busy(int32_t timeout)
{
	int32_t ts = 0;
	bool timeout_enabled = !(timeout < 0);

	if (timeout_enabled)
	{
		ts = _device.timestamp();
	}

	while (is_busy())
	{
		if (timeout_enabled && ((_device.timestamp() - ts) > timeout))
		{
			last_error = ErrorCode::TIMED_OUT;
			return;
		}
		_device.delay(1);
	}
}

bool LoRa::NRF_LLCC68::is_busy()
{
	constexpr int n = 8;
	int i{n}, v{0};

	while (i--)
	{
		v += _io.read(pins.nss);
	}

	return (v >= (n / 2)) ? true : false;
}

void LoRa::NRF_LLCC68::write_register(uint16_t address, uint8_t *data, uint8_t n)
{
	if (n == 0)
	{
		return;
	}

	wait_busy();

	nss_low();
	_spi->transfer(static_cast<uint8_t>(OPCODE::WRITE_REGISTER));
	_spi->transfer(static_cast<uint8_t>((address & 0xFF00) >> 8));
	_spi->transfer(static_cast<uint8_t>((address & 0x00FF)));
	_spi->transfer(data, n);
	nss_high();
}

void LoRa::NRF_LLCC68::read_register(uint16_t address, uint8_t *buffer, uint8_t n)
{
	if (n == 0)
	{
		return;
	}

	wait_busy();

	nss_low();
	_spi->transfer(static_cast<uint8_t>(OPCODE::READ_REGISTER));
	_spi->transfer(static_cast<uint8_t>((address & 0xFF00) >> 8));
	_spi->transfer(static_cast<uint8_t>((address & 0x00FF)));
	_spi->transfer(static_cast<uint8_t>(OPCODE::NOP)); /* Now the next transfer will retrieve the first data */
	while (n--)
	{
		*buffer++ = _spi->transfer(static_cast<uint8_t>(OPCODE::NOP));
	}
	nss_high();
}

void LoRa::NRF_LLCC68::write_buffer(const uint8_t *data, uint8_t n, uint8_t offset)
{
	if (n == 0)
	{
		return;
	}

	wait_busy();

	nss_low();
	_spi->transfer(static_cast<uint8_t>(OPCODE::WRITE_BUFFER));
	_spi->transfer(offset);
	_spi->transfer(data, n);
	nss_high();
}

void LoRa::NRF_LLCC68::read_buffer(uint8_t *buffer, uint8_t n, uint8_t offset)
{
	if (n == 0)
	{
		return;
	}

	wait_busy();

	nss_low();
	_spi->transfer(static_cast<uint8_t>(OPCODE::READ_BUFFER));
	_spi->transfer(offset);
	_spi->transfer(static_cast<uint8_t>(OPCODE::NOP));
	while (n--)
	{
		*buffer++ = _spi->transfer(static_cast<uint8_t>(OPCODE::NOP));
	}
	nss_high();
}

void LoRa::NRF_LLCC68::set_dio_irq_params(IrqMask irqMask, IrqMask dio1_mask,
										  IrqMask dio2_mask,
										  IrqMask dio3_mask)
{
	wait_busy();

	nss_low();
	_spi->transfer(*reinterpret_cast<uint16_t *>(&irqMask));
	_spi->transfer(*reinterpret_cast<uint16_t *>(&dio1_mask));
	_spi->transfer(*reinterpret_cast<uint16_t *>(&dio2_mask));
	_spi->transfer(*reinterpret_cast<uint16_t *>(&dio3_mask));
	nss_high();
}

LoRa::IrqStatus LoRa::NRF_LLCC68::get_irq_status()
{
	uint16_t _irq_status = 0;
	uint8_t tmp = 0;

	wait_busy();

	nss_low();
	_spi->transfer(static_cast<uint8_t>(OPCODE::GET_IRQ_STATUS));
	_spi->transfer(static_cast<uint8_t>(OPCODE::NOP));
	tmp = _spi->transfer(static_cast<uint8_t>(OPCODE::NOP));
	_irq_status = _irq_status | (tmp & 0x00FF);
	_irq_status = _irq_status << 8;
	tmp = _spi->transfer(static_cast<uint8_t>(OPCODE::NOP));
	_irq_status = _irq_status | (tmp & 0x00FF);
	nss_high();

	return *reinterpret_cast<IrqStatus *>(&_irq_status);
}

void LoRa::NRF_LLCC68::clear_irq_status(LLCC68_Constants::ClearIrqParam clearIrqParam)
{
	uint16_t _clearIrqParam = *reinterpret_cast<uint16_t *>(&clearIrqParam);
	wait_busy();

	nss_low();
	_spi->transfer(static_cast<uint8_t>(OPCODE::CLEAR_IRQ_STATUS));
	_spi->transfer(_clearIrqParam);
	nss_high();
}
