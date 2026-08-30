#include "hlw811x.h"

#if defined(USE_ESP32) || defined(USE_BK72XX)

#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace hlw811x {

static const char *const TAG = "hlw811x";

///////////////////////////////////////////////
/// Init HLW811x
///////////////////////////////////////////////
void HLW811xComponent::init_hlw811x_()
{
  HLW811x_Result_t result;

  if (handler == nullptr) {
      handler = new HLW811x_Handler_t();
      data = new DataPacket;

      ///////////////////////////////////////////////
      /// SPI config
      ///////////////////////////////////////////////
      HLW811x_SPIconfig(
        this->mosi_pin_->get_pin(),
        this->miso_pin_->get_pin(),
        this->clk_pin_->get_pin(),
        this->cs_pin_->get_pin()
      );

      ///////////////////////////////////////////////
      /// Initialize
      ///////////////////////////////////////////////
      HLW811x_Platform_Init_SPI(handler);

      result = HLW811x_Init(handler, HLW811X_DEVICE_HLW8112);
      if (result==HLW811X_OK) {
        ESP_LOGW(TAG, "HLW811x_Init => HLW811X_OK");
      } else if (result==HLW811X_FAIL) {
        ESP_LOGW(TAG, "HLW811x_Init => HLW811X_FAIL");
      } else if (result==HLW811X_INVALID_PARAM) {
        ESP_LOGW(TAG, "HLW811x_Init => HLW811X_INVALID_PARAM");
      } else {
        ESP_LOGW(TAG, "HLW811x_Init => unknown result : %d", result);
      }

      result = HLW811x_Begin (handler);
      handler->Platform.DelayMs((uint8_t)50);
      if (result==HLW811X_OK) {
        ESP_LOGW(TAG, "HLW811x_Begin => HLW811X_OK");
      } else if (result==HLW811X_FAIL) {
        ESP_LOGW(TAG, "HLW811x_Begin => HLW811X_FAIL");
      }
  }


  ///////////////////////////////////////////////
  /// Setup
  ///////////////////////////////////////////////
  result = HLW811x_SetResRatioIA(handler, res_ratio_ia_);
  if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_SetResRatioIA => HLW811X_FAIL");
  }

  result = HLW811x_SetResRatioIB(handler, res_ratio_ib_);
  if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_SetResRatioIB => HLW811X_FAIL");
  }

  result = HLW811x_SetResRatioU(handler, res_ratio_u_);
  if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_SetResRatioU => HLW811X_FAIL");
  }

  ///////////////////////////////////////////////
  /// Setup SYSCON
  ///////////////////////////////////////////////
  result = HLW811x_SetPGA(handler, HLW811X_PGA_1, HLW811X_PGA_16, HLW811X_PGA_16);
  if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_SetPGA => HLW811X_FAIL");
  }

  result = HLW811x_SetChannelOnOff(handler, HLW811X_ENDIS_ENABLE, HLW811X_ENDIS_ENABLE, HLW811X_ENDIS_ENABLE);
  if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_SetChannelOnOff => HLW811X_FAIL");
  }

  ///////////////////////////////////////////////
  /// Setup EMUCON
  ///////////////////////////////////////////////
  result = HLW811x_SetPFPulse(handler, HLW811X_ENDIS_ENABLE, HLW811X_ENDIS_ENABLE);
  if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_SetPFPulse => HLW811X_FAIL");
  }

  result = HLW811x_SetDigitalHighPassFilter(handler, HLW811X_ENDIS_ENABLE, HLW811X_ENDIS_ENABLE, HLW811X_ENDIS_ENABLE);
  if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_SetDigitalHighPassFilter => HLW811X_FAIL");
  }

  result = HLW811x_SetZeroCrossing(handler, HLW811X_ZERO_CROSSING_MODE_POSITIVE);
  if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_SetZeroCrossing => HLW811X_FAIL");
  }

  result = HLW811x_SetRMSCalcMode(handler, HLW811X_RMS_CALC_MODE_NORMAL);
  if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_SetRMSCalcMode => HLW811X_FAIL");
  }

  result = HLW811x_SetActivePowCalcMethod(handler, HLW811X_ACTIVE_POW_CALC_METHOD_POS_NEG_ALGEBRAIC);
  if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_SetActivePowCalcMethod => HLW811X_FAIL");
  }

  result = HLW811x_SetCompModule(handler, HLW811X_ENDIS_DISABLE);
  if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_SetCompModule => HLW811X_FAIL");
  }

  ///////////////////////////////////////////////
  /// Setup EMUCON2
  ///////////////////////////////////////////////
  result = HLW811x_SetZeroCrossingDetection(handler, HLW811X_ENDIS_ENABLE);
  if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_SetZeroCrossingDetection => HLW811X_FAIL");
  }

  result = HLW811x_SetWaveformData(handler, HLW811X_ENDIS_ENABLE);
  if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_SetWaveformData => HLW811X_FAIL");
  }

  result = HLW811x_SetPowerFactorFunctionality(handler, HLW811X_ENDIS_ENABLE);
  if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_SetPowerFactorFunctionality => HLW811X_FAIL");
  }

  result = HLW811x_ChannelBMeasurement(handler, HLW811X_CHANNEL_B_MEASUREMENT_IB );
  if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_ChannelBMeasurement => HLW811X_FAIL");
  }

  result = HLW811x_SetDataUpdateFreq(handler, HLW811X_DATA_UPDATE_FREQ_3_4HZ);
  if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_SetDataUpdateFreq => HLW811X_FAIL");
  }

  result = HLW811x_SetEnergyClearance(handler, HLW811X_ENDIS_DISABLE, HLW811X_ENDIS_DISABLE);
  if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_SetEnergyClearance => HLW811X_FAIL");
  }

  ///////////////////////////////////////////////
  /// Setup Misc registries
  ///////////////////////////////////////////////

  // INT1 and INT2 pins
  result = HLW811x_SetIntOutFunc(handler, HLW811X_INTOUT_FUNC_PULSE_PFA, HLW811X_INTOUT_FUNC_PULSE_PFB);
  if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_SetIntOutFunc => HLW811X_FAIL");
  }

  // Special Measurement Channel
  result = HLW811x_SetSpecialMeasurementChannel(handler,
    (this->swap_ab_)
      ?((this->channel_sel_==HLW811X_CURRENT_CHANNEL_A)?HLW811X_CURRENT_CHANNEL_B:HLW811X_CURRENT_CHANNEL_A)
      :(this->channel_sel_)
  );
  if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_SetSpecialMeasurementChannel => HLW811X_FAIL");
  }


  ///////////////////////////////////////////////
  /// Read SYSCON, EMUCON, EMUCON2
  ///////////////////////////////////////////////
  uint8_t Reg[2] = {0};

  result = HLW811x_ReadRegLL(handler, HLW811X_REG_ADDR_SYSCON, &Reg[0], 2);
  if (result==HLW811X_OK) {
    data->SYSCON = (Reg[0]<<8)+Reg[1];
  } else if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "Read SYSCON => HLW811X_FAIL");
  }

  result = HLW811x_ReadRegLL(handler, HLW811X_REG_ADDR_EMUCON, &Reg[0], 2);
  if (result==HLW811X_OK) {
    data->EMUCON = (Reg[0]<<8)+Reg[1];
  } else if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "Read EMUCON => HLW811X_FAIL");
  }

  result = HLW811x_ReadRegLL(handler, HLW811X_REG_ADDR_EMUCON2, &Reg[0], 2);
  if (result==HLW811X_OK) {
    data->EMUCON2 = (Reg[0]<<8)+Reg[1];
  } else if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "Read EMUCON2 => HLW811X_FAIL");
  }

  handler->Platform.DelayMs((uint8_t)200); // wait a while
}

///////////////////////////////////////////////
/// Poll HLW811x
///////////////////////////////////////////////
void HLW811xComponent::poll_hlw811x_() {
  HLW811x_Result_t result;
  float value;

  result = HLW811x_GetRmsU (handler, &value);
  if (result==HLW811X_OK) {
    data->rmsU = value;
  } else if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_GetRmsU => HLW811X_FAIL");
  }

  result = HLW811x_GetFreqU (handler, &value);
  if (result==HLW811X_OK) {
    data->freqU = value;
  } else if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_GetFreqU => HLW811X_FAIL");
  }

  result = HLW811x_GetPhaseAngle (handler, &value, 50);
  if (result==HLW811X_OK) {
    data->phaseAngle = value;
  } else if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_GetPhaseAngle => HLW811X_FAIL");
  }

  result = HLW811x_GetPowerFactor (handler, &value);
  if (result==HLW811X_OK) {
    data->powerFactor = value;
  } else if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_GetPowerFactor => HLW811X_FAIL");
  }

  result = HLW811x_GetPowerS (handler, &value);
  if (result==HLW811X_OK) {
    data->powerS = value;
  } else if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_GetPowerS => HLW811X_FAIL");
  }

  result = HLW811x_GetRmsIA (handler, &value);
  if (result==HLW811X_OK) {
    if (!this->swap_ab_)
      data->rmsIA = value;
    else
      data->rmsIB = value;
  } else if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_GetRmsIA => HLW811X_FAIL");
  }

  result = HLW811x_GetRmsIB (handler, &value);
  if (result==HLW811X_OK) {
    if (!this->swap_ab_)
      data->rmsIB = value;
    else
      data->rmsIA = value;
  } else if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_GetRmsIB => HLW811X_FAIL");
  }

  result = HLW811x_GetPowerPA (handler, &value);
  if (result==HLW811X_OK) {
    if (!this->swap_ab_)
      data->powerPA = value;
    else
      data->powerPB = value;
  } else if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_GetPowerPA => HLW811X_FAIL");
  }

  result = HLW811x_GetPowerPB (handler, &value);
  if (result==HLW811X_OK) {
    if (!this->swap_ab_)
      data->powerPB = value;
    else
      data->powerPA = value;
  } else if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_GetPowerPB => HLW811X_FAIL");
  }

  result = HLW811x_GetEnergyA (handler, &value);
  if (result==HLW811X_OK) {
    if (!this->swap_ab_)
      data->energyA = value;
    else
      data->energyB = value;
  } else if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_GetEnergyA => HLW811X_FAIL");
  }

  result = HLW811x_GetEnergyB (handler, &value);
  if (result==HLW811X_OK) {
    if (!this->swap_ab_)
      data->energyB = value;
    else
      data->energyA = value;
  } else if (result==HLW811X_FAIL) {
    ESP_LOGW(TAG, "HLW811x_GetEnergyB => HLW811X_FAIL");
  }
}

void HLW811xComponent::setup() {
  this->init_hlw811x_();
}

void HLW811xComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "HLW811x:");
  LOG_SENSOR("  ", "Voltage", this->voltage_sensor_);
  LOG_SENSOR("  ", "Frequency", this->frequency_sensor_);
  LOG_SENSOR("  ", "Power factor", this->power_factor_sensor_);
  LOG_SENSOR("  ", "Phase angle", this->phase_angle_sensor_);
  LOG_SENSOR("  ", "Apparent power", this->apparent_power_sensor_);
  LOG_SENSOR("  ", "Current A", this->current_a_sensor_);
  LOG_SENSOR("  ", "Current B", this->current_b_sensor_);
  LOG_SENSOR("  ", "Active Power A", this->active_power_a_sensor_);
  LOG_SENSOR("  ", "Active Power B", this->active_power_b_sensor_);
  LOG_SENSOR("  ", "Energy A", this->energy_a_sensor_);
  LOG_SENSOR("  ", "Energy B", this->energy_b_sensor_);
  LOG_TEXT_SENSOR("  ", "Registers", this->log_registers_text_sensor_);
  LOG_TEXT_SENSOR("  ", "Settings", this->log_settings_text_sensor_);
  LOG_UPDATE_INTERVAL(this);
  LOG_PIN("  MISO Pin: ", this->miso_pin_);
  LOG_PIN("  MOSI Pin: ", this->mosi_pin_);
  LOG_PIN("  CLK  Pin: ", this->clk_pin_);
  LOG_PIN("  CS   Pin: ", this->cs_pin_);
}

void HLW811xComponent::update() {
  if (handler != nullptr) {

    this->poll_hlw811x_();

    if (voltage_sensor_ != nullptr) {
      voltage_sensor_->publish_state(data->rmsU);
    }
    if (frequency_sensor_ != nullptr) {
      frequency_sensor_->publish_state(data->freqU);
    }
    if (power_factor_sensor_ != nullptr) {
      power_factor_sensor_->publish_state(data->powerFactor);
    }
    if (phase_angle_sensor_ != nullptr) {
      phase_angle_sensor_->publish_state(data->phaseAngle);
    }
    if (apparent_power_sensor_ != nullptr) {
      apparent_power_sensor_->publish_state(data->powerS);
    }
    if (current_a_sensor_ != nullptr) {
      current_a_sensor_->publish_state(data->rmsIA);
    }
    if (current_b_sensor_ != nullptr) {
      current_b_sensor_->publish_state(data->rmsIB);
    }
    if (active_power_a_sensor_ != nullptr) {
      active_power_a_sensor_->publish_state(data->powerPA);
    }
    if (active_power_b_sensor_ != nullptr) {
      active_power_b_sensor_->publish_state(data->powerPB);
    }
    if (energy_a_sensor_ != nullptr) {
      energy_a_sensor_->publish_state(data->energyA);
    }
    if (energy_b_sensor_ != nullptr) {
      energy_b_sensor_->publish_state(data->energyB);
    }

    if (log_registers_text_sensor_ != nullptr) {
      char json_registers[256];
      snprintf(json_registers, sizeof (json_registers),
        "{\"RmsIAC\":%u,"
        "\"RmsIBC\":%u,"
        "\"RmsUC\":%u,"
        "\"PowerPAC\":%u,"
        "\"PowerPBC\":%u,"
        "\"PowerSC\":%u,"
        "\"EnergyAC\":%u,"
        "\"EnergyBC\":%u,"
        "\"SYSCON\":%u,"
        "\"EMUCON\":%u,"
        "\"EMUCON2\":%u}",
        handler->CoefReg.RmsIAC,
        handler->CoefReg.RmsIBC,
        handler->CoefReg.RmsUC,
        handler->CoefReg.PowerPAC,
        handler->CoefReg.PowerPBC,
        handler->CoefReg.PowerSC,
        handler->CoefReg.EnergyAC,
        handler->CoefReg.EnergyBC,
        data->SYSCON,
        data->EMUCON,
        data->EMUCON2
      );
      log_registers_text_sensor_->publish_state(json_registers);
    }

    if (log_settings_text_sensor_ != nullptr) {
      char json_settings[256];
      snprintf(json_settings, sizeof (json_settings),
        "{\"HFconst\":%d,"
        "\"PGA.IA\":%d,"
        "\"PGA.IB\":%d,"
        "\"PGA.U\":%d,"
        "\"KIA\":%.2f,"
        "\"KIB\":%.2f,"
        "\"KU\":%.2f,"
        "\"SwapAB\":\"%s\","
        "\"ChannelSel\":\"%s\"}",
        (int)handler->HFconst,
        (int)handler->PGA.IA,
        (int)handler->PGA.IB,
        (int)handler->PGA.U,
        handler->ResCoef.KIA,
        handler->ResCoef.KIB,
        handler->ResCoef.KU,
        YESNO(this->swap_ab_),
        (this->swap_ab_)
          ?((this->channel_sel_==HLW811X_CURRENT_CHANNEL_A)?"A":"B")
          :((this->channel_sel_==HLW811X_CURRENT_CHANNEL_A)?"B":"A")
      );
      log_settings_text_sensor_->publish_state(json_settings);
    }
  }
}

void HLW811xComponent::publish_() {
  if (handler != nullptr) {
    this->update();
  }
}

void HLW811xComponent::display_status_() {
  if (handler != nullptr) {
    ///////////////////////////////////////////////
    /// HLW811x Calibration coefficients
    ///////////////////////////////////////////////
    ESP_LOGD(TAG, "RmsIAC      : %u", handler->CoefReg.RmsIAC);
    ESP_LOGD(TAG, "RmsIBC      : %u", handler->CoefReg.RmsIBC);
    ESP_LOGD(TAG, "RmsUC       : %u", handler->CoefReg.RmsUC);
    ESP_LOGD(TAG, "PowerPAC    : %u", handler->CoefReg.PowerPAC);
    ESP_LOGD(TAG, "PowerPBC    : %u", handler->CoefReg.PowerPBC);
    ESP_LOGD(TAG, "PowerSC     : %u", handler->CoefReg.PowerSC);
    ESP_LOGD(TAG, "EnergyAC    : %u", handler->CoefReg.EnergyAC);
    ESP_LOGD(TAG, "EnergyBC    : %u", handler->CoefReg.EnergyBC);

    ///////////////////////////////////////////////
    /// Read Register
    ///////////////////////////////////////////////
    ESP_LOGD(TAG, "SYSCON      : %d " BYTE_TO_BINARY_PATTERN "." BYTE_TO_BINARY_PATTERN,
      data->SYSCON,
      BYTE_TO_BINARY(data->SYSCON>>8),
      BYTE_TO_BINARY(data->SYSCON));
    ESP_LOGD(TAG, "EMUCON      : %d " BYTE_TO_BINARY_PATTERN "." BYTE_TO_BINARY_PATTERN,
      data->EMUCON,
      BYTE_TO_BINARY(data->EMUCON>>8),
      BYTE_TO_BINARY(data->EMUCON));
    ESP_LOGD(TAG, "EMUCON2     : %d " BYTE_TO_BINARY_PATTERN "." BYTE_TO_BINARY_PATTERN,
      data->EMUCON2,
      BYTE_TO_BINARY(data->EMUCON2>>8),
      BYTE_TO_BINARY(data->EMUCON2));
    }

    ///////////////////////////////////////////////
    /// HLW811x Specific parameters
    ///////////////////////////////////////////////
    ESP_LOGD(TAG, "HFconst     : %d", (int)handler->HFconst);
    ESP_LOGD(TAG, "PGA.IA      : %d " BYTE_TO_BINARY_PATTERN,
      (int)handler->PGA.IA,
      BYTE_TO_BINARY(handler->PGA.IA));
    ESP_LOGD(TAG, "PGA.IB      : %d " BYTE_TO_BINARY_PATTERN,
      (int)handler->PGA.IB,
      BYTE_TO_BINARY(handler->PGA.IB));
    ESP_LOGD(TAG, "PGA.U       : %d " BYTE_TO_BINARY_PATTERN,
      (int)handler->PGA.U,
      BYTE_TO_BINARY(handler->PGA.U));
    ESP_LOGD(TAG, "KIA (K1 IA) : %.2f", handler->ResCoef.KIA);
    ESP_LOGD(TAG, "KIB (K1 IB) : %.2f", handler->ResCoef.KIB);
    ESP_LOGD(TAG, "KU  (K2 U)  : %.2f", handler->ResCoef.KU);

    ///////////////////////////////////////////////
    /// HLW811x Custom parameters
    ///////////////////////////////////////////////
    ESP_LOGD(TAG, "SwapAB      : %s", YESNO(this->swap_ab_));
    ESP_LOGD(TAG, "Channel Sel : %s\n", (this->swap_ab_)
          ?((this->channel_sel_==HLW811X_CURRENT_CHANNEL_A)?"A":"B")
          :((this->channel_sel_==HLW811X_CURRENT_CHANNEL_A)?"B":"A"));
}

void HLW811xComponent::set_special_measurement_channel_(HLW811x_CurrentChannel_t channel) {
  this->set_channel_sel (channel);
  ESP_LOGW(TAG, "Channel set to %d ", (int)this->channel_sel_);

  if (handler != nullptr) {
      HLW811x_Result_t result;
      result = HLW811x_SetSpecialMeasurementChannel(handler,
        (this->swap_ab_)
          ?((this->channel_sel_==HLW811X_CURRENT_CHANNEL_A)?HLW811X_CURRENT_CHANNEL_B:HLW811X_CURRENT_CHANNEL_A)
          :(this->channel_sel_)
      );
      if (result==HLW811X_OK) {
      } else if (result==HLW811X_FAIL) {
        ESP_LOGW(TAG, "HLW811x_SetSpecialMeasurementChannel => HLW811X_FAIL");
      }
  }
}

void HLW811xComponent::reset_energy_(HLW811x_CurrentChannel_t channel) {
  if (handler != nullptr) {
    HLW811x_Result_t result;
    float value;

    // Reset after reading, channel A or B according to channel parameter
    if (!this->swap_ab_)
      result = HLW811x_SetEnergyClearance(handler,
        channel==HLW811X_CURRENT_CHANNEL_A?HLW811X_ENDIS_ENABLE:HLW811X_ENDIS_DISABLE,
        channel==HLW811X_CURRENT_CHANNEL_B?HLW811X_ENDIS_ENABLE:HLW811X_ENDIS_DISABLE);
    else
      result = HLW811x_SetEnergyClearance(handler,
        channel==HLW811X_CURRENT_CHANNEL_B?HLW811X_ENDIS_ENABLE:HLW811X_ENDIS_DISABLE,
        channel==HLW811X_CURRENT_CHANNEL_A?HLW811X_ENDIS_ENABLE:HLW811X_ENDIS_DISABLE);
    if (result==HLW811X_FAIL) {
      ESP_LOGW(TAG, "HLW811x_SetEnergyClearance => HLW811X_FAIL");
    }

    // Read & store Energy A
    result = HLW811x_GetEnergyA (handler, &value);
    if (result==HLW811X_OK) {
      if (!this->swap_ab_)
        data->energyA = value;
      else
        data->energyB = value;
    } else if (result==HLW811X_FAIL) {
      ESP_LOGW(TAG, "HLW811x_GetEnergyA => HLW811X_FAIL");
    }

    // Read & store Energy B
    result = HLW811x_GetEnergyB (handler, &value);
    if (result==HLW811X_OK) {
      if (!this->swap_ab_)
        data->energyB = value;
      else
        data->energyA = value;
    } else if (result==HLW811X_FAIL) {
      ESP_LOGW(TAG, "HLW811x_GetEnergyB => HLW811X_FAIL");
    }

    // Publish values
    this->update();

    // Do not reset after reading
    result = HLW811x_SetEnergyClearance(handler, HLW811X_ENDIS_DISABLE, HLW811X_ENDIS_DISABLE);
    if (result==HLW811X_FAIL) {
      ESP_LOGW(TAG, "HLW811x_SetEnergyClearance => HLW811X_FAIL");
    }
  }
}

}  // namespace hlw811x
}  // namespace esphome

#endif
