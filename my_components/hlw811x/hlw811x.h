#pragma once

#if defined(USE_ESP32) || defined(USE_BK72XX)

#include "esphome/core/automation.h"
#include "esphome/core/defines.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"

#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif
#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif

#include "HLW811x_regs.h"
#include "HLW811x_lib.h"

#if defined (USE_ESP32)
  #include "HLW811x_platform_esp32.h"
#elif defined (USE_BK72XX)
  #include "HLW811x_platform_bk72xx.h"
#endif

namespace esphome::hlw811x {

struct DataPacket {
  uint16_t SYSCON;
  uint16_t EMUCON;
  uint16_t EMUCON2;
  float rmsU;
  float freqU;
  float phaseAngle;
  float powerFactor;
  float powerS;
  float rmsIA;
  float rmsIB;
  float powerPA;
  float powerPB;
  float energyA;
  float energyB;

} __attribute__((packed));

class HLW811xComponent : public PollingComponent {
 public:
  HLW811xComponent() {}

  void setup() override;
  void dump_config() override;
  void update() override;

  void set_clk_pin(InternalGPIOPin *pin) { clk_pin_ = pin; }
  void set_cs_pin(InternalGPIOPin *pin) { cs_pin_ = pin; }
  void set_miso_pin(InternalGPIOPin *pin) { miso_pin_ = pin; }
  void set_mosi_pin(InternalGPIOPin *pin) { mosi_pin_ = pin; }

  void set_res_ratio_ia(float res_ratio_ia) { res_ratio_ia_ = res_ratio_ia; }
  void set_res_ratio_ib(float res_ratio_ib) { res_ratio_ib_ = res_ratio_ib; }
  void set_res_ratio_u(float res_ratio_u) { res_ratio_u_ = res_ratio_u; }
  void set_swap_ab(bool swap_ab) { swap_ab_ = swap_ab; }
  void set_channel_sel(HLW811x_CurrentChannel_t channel_sel) { channel_sel_ = channel_sel; }

#ifdef USE_SENSOR
  SUB_SENSOR(voltage)
  SUB_SENSOR(frequency)
  SUB_SENSOR(power_factor)
  SUB_SENSOR(phase_angle)
  SUB_SENSOR(apparent_power)
  SUB_SENSOR(current_a)
  SUB_SENSOR(current_b)
  SUB_SENSOR(active_power_a)
  SUB_SENSOR(active_power_b)
  SUB_SENSOR(energy_a)
  SUB_SENSOR(energy_b)
#endif
#ifdef USE_TEXT_SENSOR
  SUB_TEXT_SENSOR(log_registers)
  SUB_TEXT_SENSOR(log_settings)
#endif

 protected:
  template<typename... Ts> friend class PublishAction;
  void publish_();
  template<typename... Ts> friend class DisplayStatusAction;
  void display_status_();
  template<typename... Ts> friend class SetSpecialMeasurementChannelAction;
  void set_special_measurement_channel_(HLW811x_CurrentChannel_t channel);
  template<typename... Ts> friend class ResetEnergyAction;
  void reset_energy_(HLW811x_CurrentChannel_t channel);

  HLW811x_Handler_t *handler{nullptr};
  DataPacket *data{nullptr};

  InternalGPIOPin *clk_pin_{nullptr};
  InternalGPIOPin *cs_pin_{nullptr};
  InternalGPIOPin *miso_pin_{nullptr};
  InternalGPIOPin *mosi_pin_{nullptr};

  float res_ratio_ia_{0.2f};
  float res_ratio_ib_{0.2f};
  float res_ratio_u_{1.0f};
  bool swap_ab_{false};
  HLW811x_CurrentChannel_t channel_sel_{HLW811X_CURRENT_CHANNEL_A};

  void init_hlw811x_();
  void poll_hlw811x_();

 private:
};

template<typename... Ts> class PublishAction : public Action<Ts...>, public Parented<HLW811xComponent> {
 public:
  void play(const Ts &...x) override { this->parent_->publish_(); }
};

template<typename... Ts> class DisplayStatusAction : public Action<Ts...>, public Parented<HLW811xComponent> {
 public:
  void play(const Ts &...x) override { this->parent_->display_status_(); }
};

template<typename... Ts> class SetSpecialMeasurementChannelAction : public Action<Ts...>, public Parented<HLW811xComponent> {
 public:
  TEMPLATABLE_VALUE(HLW811x_CurrentChannel_t, channel)
  void play(const Ts &...x) override { this->parent_->set_special_measurement_channel_(this->channel_.value(x...)); }
};

template<typename... Ts> class ResetEnergyAction : public Action<Ts...>, public Parented<HLW811xComponent> {
 public:
  TEMPLATABLE_VALUE(HLW811x_CurrentChannel_t, channel)
  void play(const Ts &...x) override { this->parent_->reset_energy_(this->channel_.value(x...)); }
};

}  // namespace esphome::hlw811x

#endif
