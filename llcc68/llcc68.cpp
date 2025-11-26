/**
 * @author SERDAR PEHLIVAN
 * @date 14/11/2025
 * @version 1.0
 *
 */

#include "llcc68.h"
#include "opcodes.h"
#include <cassert>

using LoRa::LLCC68;

LLCC68::LLCC68(const LLCC68_pins &pins,
					   const LLCC68_config &config,
					   std::unique_ptr<LoRa_SPI> spi,
					   std::unique_ptr<LoRa_IO> io,
					   std::unique_ptr<Device> device)
	: last_error{ErrorCode::NO_ERROR}, pins{pins}, config{config}, _spi{std::move(spi)}, _io{std::move(io)}, _device{std::move(device)}
{
	if (!_spi->is_bit_order_msb_first())
	{
		_spi->set_bit_order(true);
	}
}

void LoRa::LLCC68::reset()
{
	// TODO: Double check delay values
	_io->write(pins.nreset, IO_LOW);
	_device->delay(2);
	_io->write(pins.nreset, IO_HIGH);
	_device->delay(20);
}

void LoRa::LLCC68::sleep(SleepConfig sleepConfig)
{
	set_sleep(sleepConfig);
}

void LoRa::LLCC68::set_standby(LLCC68_Constants::StandbyConfig standbyConfig)
{
	wait_busy();

	_spi->begin_transfer();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_STANDBY));
	_spi->transfer(static_cast<uint8_t>(standbyConfig));
	_spi->end_transfer();
}

uint32_t LoRa::LLCC68::calculate_rf_frequency(uint32_t desired_freq)
{
	return std::floor(static_cast<double>(desired_freq) / LLCC68_Constants::freq_step);
}

void LoRa::LLCC68::set_sleep(SleepConfig sleepConfig)
{
	wait_busy();

	_spi->begin_transfer();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_SLEEP));
	_spi->transfer(*reinterpret_cast<uint8_t *>(&sleepConfig));
	_spi->end_transfer();
}

void LoRa::LLCC68::set_packet_type(LLCC68_Constants::PacketType protocol)
{
	if (protocol != LLCC68_Constants::PacketType::LORA)
	{
		// GFSK is not supported yet, and exceptions are not enabled
		return;
	}

	wait_busy();

	_spi->begin_transfer();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_PACKET_TYPE));
	_spi->transfer(static_cast<uint8_t>(protocol));
	_spi->end_transfer();
}

void LoRa::LLCC68::set_lora_modulation_params(LLCC68_Constants::SF sf, LLCC68_Constants::BW bw, LLCC68_Constants::CR cr, LLCC68_Constants::LDRO ldOpt)
{
	wait_busy();

	_spi->begin_transfer();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_MODULATION_PARAMS));
	_spi->transfer(static_cast<uint8_t>(sf));
	_spi->transfer(static_cast<uint8_t>(bw));
	_spi->transfer(static_cast<uint8_t>(cr));
	_spi->transfer(static_cast<uint8_t>(ldOpt));
	_spi->end_transfer();
}

void LoRa::LLCC68::set_tx(int32_t timeout)
{
	wait_busy();

	timeout = timeout & 0x00FFFFFF;

	_spi->begin_transfer();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_TX));
	_spi->transfer(static_cast<uint8_t>((timeout & 0x00FF0000) >> 16));
	_spi->transfer(static_cast<uint8_t>((timeout & 0x0000FF00) >> 8));
	_spi->transfer(static_cast<uint8_t>(timeout & 0x000000FF));
	_spi->end_transfer();
}

void LoRa::LLCC68::set_rx(int32_t timeout)
{
	wait_busy();

	timeout = timeout & 0x00FFFFFF;

	_spi->begin_transfer();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_RX));
	_spi->transfer(static_cast<uint8_t>((timeout & 0x00FF0000) >> 16));
	_spi->transfer(static_cast<uint8_t>((timeout & 0x0000FF00) >> 8));
	_spi->transfer(static_cast<uint8_t>(timeout & 0x000000FF));
	_spi->end_transfer();
}

void LoRa::LLCC68::set_regulator_mode(
	LLCC68_Constants::RegModeParam regMode)
{
	wait_busy();

	_spi->begin_transfer();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_REGULATOR_MODE));
	_spi->transfer(static_cast<uint8_t>(regMode));
	_spi->end_transfer();
}

void LoRa::LLCC68::set_pa_config(uint8_t paDutyCycle, uint8_t hpMax)
{
	wait_busy();

	uint8_t deviceSel = 0x00; // Reserved value
	uint8_t paLut = 0x01;	  // Reserved value

	_spi->begin_transfer();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_PA_CONFIG));
	_spi->transfer(paDutyCycle);
	_spi->transfer(hpMax);
	_spi->transfer(deviceSel);
	_spi->transfer(paLut);
	_spi->end_transfer();
}

void LoRa::LLCC68::set_dio3_as_tcxo_ctrl(
	LLCC68_Constants::TCXO_VOLTAGE tcxoVoltage, int32_t delay)
{
	wait_busy();

	if (delay == -1)
		delay = 0x64;

	delay = delay & 0x00FFFFFF;

	_spi->begin_transfer();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_DIO3_AS_TCXO_CTRL));
	_spi->transfer(static_cast<uint8_t>((delay & 0x00FF0000) >> 16));
	_spi->transfer(static_cast<uint8_t>((delay & 0x0000FF00) >> 8));
	_spi->transfer(static_cast<uint8_t>(delay & 0x000000FF));
	_spi->end_transfer();
}

void LoRa::LLCC68::set_dio2_as_rf_switch_ctrl(
	LLCC68_Constants::Enable enable)
{
	wait_busy();

	_spi->begin_transfer();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_DIO2_AS_RF_SWITCH_CTRL));
	_spi->transfer(static_cast<uint8_t>(enable));
	_spi->end_transfer();
}

void LoRa::LLCC68::set_rf_frequency(uint32_t rf_freq)
{
	wait_busy();

	_spi->begin_transfer();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_RF_FREQUENCY));
	_spi->transfer(static_cast<uint8_t>((rf_freq & 0xFF000000) >> 24));
	_spi->transfer(static_cast<uint8_t>((rf_freq & 0x00FF0000) >> 16));
	_spi->transfer(static_cast<uint8_t>((rf_freq & 0x0000FF00) >> 8));
	_spi->transfer(static_cast<uint8_t>(rf_freq & 0x000000FF));
	_spi->end_transfer();
}

void LoRa::LLCC68::set_tx_params(int8_t power_dbm,
									 LLCC68_Constants::RampTime rampTime)
{
	assert((power_dbm >= -9) && (power_dbm <= 22));
	wait_busy();

	_spi->begin_transfer();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_TX_PARAMS));
	_spi->transfer(power_dbm);
	_spi->transfer(static_cast<uint8_t>(rampTime));
	_spi->end_transfer();
}

void LoRa::LLCC68::set_lora_packet_params(
	uint16_t preambleLength, LLCC68_Constants::HeaderType headerType, uint8_t payloadLength, LLCC68_Constants::CRC_Type crcType, LLCC68_Constants::InvertIQ invertIq)
{
	wait_busy();

	_spi->begin_transfer();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_PACKET_PARAMS));
	_spi->transfer(static_cast<uint8_t>((preambleLength & 0xFF00) >> 8));
	_spi->transfer(static_cast<uint8_t>(preambleLength & 0x00FF));
	_spi->transfer(static_cast<uint8_t>(headerType));
	_spi->transfer(payloadLength);
	_spi->transfer(static_cast<uint8_t>(crcType));
	_spi->transfer(static_cast<uint8_t>(invertIq));
	_spi->end_transfer();
}

void LoRa::LLCC68::set_buffer_base_address(uint8_t tx_base_addr,
											   uint8_t rx_base_addr)
{
	wait_busy();

	_spi->begin_transfer();
	_spi->transfer(static_cast<uint8_t>(OPCODE::SET_BUFFER_BASE_ADDRESS));
	_spi->transfer(tx_base_addr);
	_spi->transfer(rx_base_addr);
	_spi->end_transfer();
}

void LoRa::LLCC68::wait_for_irq_tx_done(int dio_pin)
{
	using LoRa::LLCC68_Constants;

	uint8_t n_timeout = 0;

	while (!_io->read(dio_pin)) // TODO: Check datasheet
	{
		// IRQ mask should be checked for tx done flag
		n_timeout++;
		_device->delay(1);
		if (n_timeout > 200)
		{
			last_error = ErrorCode::TIMED_OUT;
			return;
		}
	}
}

void LoRa::LLCC68::wait_busy(int32_t timeout)
{
	int32_t ts = 0;
	bool timeout_enabled = !(timeout < 0);

	if (timeout_enabled)
	{
		ts = _device->timestamp();
	}

	while (is_busy())
	{
		if (timeout_enabled && ((_device->timestamp() - ts) > timeout))
		{
			last_error = ErrorCode::TIMED_OUT;
			return;
		}
		_device->delay(1);
	}
}

bool LoRa::LLCC68::is_busy()
{
	constexpr int n = 8;
	int i{n}, v{0};

	while (i--)
	{
		v += _io->read(pins.busy);
	}

	return (v >= (n / 2)) ? true : false;
}

void LoRa::LLCC68::write_register(uint16_t address, uint8_t *data, uint8_t n)
{
	if (n == 0)
	{
		return;
	}

	wait_busy();

	_spi->begin_transfer();
	_spi->transfer(static_cast<uint8_t>(OPCODE::WRITE_REGISTER));
	_spi->transfer(static_cast<uint8_t>((address & 0xFF00) >> 8));
	_spi->transfer(static_cast<uint8_t>((address & 0x00FF)));
	_spi->transfer(data, n);
	_spi->end_transfer();
}

void LoRa::LLCC68::read_register(uint16_t address, uint8_t *buffer, uint8_t n)
{
	if (n == 0)
	{
		return;
	}

	wait_busy();

	_spi->begin_transfer();
	_spi->transfer(static_cast<uint8_t>(OPCODE::READ_REGISTER));
	_spi->transfer(static_cast<uint8_t>((address & 0xFF00) >> 8));
	_spi->transfer(static_cast<uint8_t>((address & 0x00FF)));
	_spi->transfer(static_cast<uint8_t>(OPCODE::NOP)); /* Now the next transfer will retrieve the first data */
	while (n--)
	{
		*buffer++ = _spi->transfer(static_cast<uint8_t>(OPCODE::NOP));
	}
	_spi->end_transfer();
}

void LoRa::LLCC68::write_buffer(const uint8_t *data, uint8_t n, uint8_t offset)
{
	if (n == 0)
	{
		return;
	}

	wait_busy();

	_spi->begin_transfer();
	_spi->transfer(static_cast<uint8_t>(OPCODE::WRITE_BUFFER));
	_spi->transfer(offset);
	_spi->transfer(data, n);
	_spi->end_transfer();
}

void LoRa::LLCC68::read_buffer(uint8_t *buffer, uint8_t n, uint8_t offset)
{
	if (n == 0)
	{
		return;
	}

	wait_busy();

	_spi->begin_transfer();
	_spi->transfer(static_cast<uint8_t>(OPCODE::READ_BUFFER));
	_spi->transfer(offset);
	_spi->transfer(static_cast<uint8_t>(OPCODE::NOP));
	while (n--)
	{
		*buffer++ = _spi->transfer(static_cast<uint8_t>(OPCODE::NOP));
	}
	_spi->end_transfer();
}

void LoRa::LLCC68::set_dio_irq_params(IrqMask irqMask, IrqMask dio1_mask,
										  IrqMask dio2_mask,
										  IrqMask dio3_mask)
{
	auto _irqMask = *reinterpret_cast<uint16_t *>(&irqMask);
	auto _dio1_mask = *reinterpret_cast<uint16_t *>(&dio1_mask);
	auto _dio2_mask = *reinterpret_cast<uint16_t *>(&dio2_mask);
	auto _dio3_mask = *reinterpret_cast<uint16_t *>(&dio3_mask);
	wait_busy();

	_spi->begin_transfer();
	_spi->transfer(static_cast<uint8_t>((_irqMask & 0xFF00) >> 8));
	_spi->transfer(static_cast<uint8_t>((_irqMask & 0x00FF)));
	_spi->transfer(static_cast<uint8_t>((_dio1_mask & 0xFF00) >> 8));
	_spi->transfer(static_cast<uint8_t>((_dio1_mask & 0x00FF)));
	_spi->transfer(static_cast<uint8_t>((_dio2_mask & 0xFF00) >> 8));
	_spi->transfer(static_cast<uint8_t>((_dio2_mask & 0x00FF)));
	_spi->transfer(static_cast<uint8_t>((_dio3_mask & 0xFF00) >> 8));
	_spi->transfer(static_cast<uint8_t>((_dio3_mask & 0x00FF)));
	_spi->end_transfer();
}

LoRa::IrqStatus LoRa::LLCC68::get_irq_status()
{
	uint16_t _irq_status = 0;
	uint8_t tmp = 0;

	wait_busy();

	_spi->begin_transfer();
	_spi->transfer(static_cast<uint8_t>(OPCODE::GET_IRQ_STATUS));
	_spi->transfer(static_cast<uint8_t>(OPCODE::NOP));
	tmp = _spi->transfer(static_cast<uint8_t>(OPCODE::NOP));
	_irq_status = _irq_status | static_cast<uint16_t>(tmp);
	_irq_status = _irq_status << 8;
	tmp = _spi->transfer(static_cast<uint8_t>(OPCODE::NOP));
	_irq_status = _irq_status | static_cast<uint16_t>(tmp);
	_spi->end_transfer();

	return *reinterpret_cast<IrqStatus *>(&_irq_status);
}

void LoRa::LLCC68::clear_irq_status(LLCC68_Constants::ClearIrqParam clearIrqParam)
{
	uint16_t _clearIrqParam = *reinterpret_cast<uint16_t *>(&clearIrqParam);
	wait_busy();
	
	_spi->begin_transfer();
	_spi->transfer(static_cast<uint8_t>(OPCODE::CLEAR_IRQ_STATUS));
	_spi->transfer(static_cast<uint8_t>((_clearIrqParam & 0xFF00) >> 8));
	_spi->transfer(static_cast<uint8_t>((_clearIrqParam & 0x00FF)));
	_spi->end_transfer();
}
