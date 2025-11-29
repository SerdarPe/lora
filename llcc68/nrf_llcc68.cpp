#include "nrf_llcc68.h"

LoRa::NRF_LLCC68::~NRF_LLCC68() = default;

void LoRa::NRF_LLCC68::send_packet(const uint8_t *packet, uint8_t size) {
  if (size == 0) {
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
  wait_for_irq_tx_done(pins.dio1);
  // TODO: Check for device error
  clear_irq_status(LLCC68_Constants::ClearIrqParam::TxDone);

  set_rx(0x00FFFFFF); // Not sure about when to return to rx mode
                      // No IRQ set
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