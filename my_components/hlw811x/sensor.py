from esphome import pins
from esphome import automation
import esphome.codegen as cg
from esphome.components import sensor, text_sensor

import esphome.config_validation as cv
from esphome.const import (
    PLATFORM_ESP32,
    PLATFORM_BK72XX,

    CONF_ID,

    CONF_NUMBER,

    CONF_CLK_PIN,
    CONF_CS_PIN,
    CONF_MISO_PIN,
    CONF_MOSI_PIN,

    CONF_ACTIVE_POWER,
    CONF_APPARENT_POWER,
    CONF_CHANNEL,
    CONF_CURRENT,
    CONF_ENERGY,
    CONF_FREQUENCY,
    CONF_PHASE_ANGLE,
    CONF_POWER_FACTOR,
    CONF_VOLTAGE,

    DEVICE_CLASS_APPARENT_POWER,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_ENERGY,
    DEVICE_CLASS_FREQUENCY,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_POWER_FACTOR,
    DEVICE_CLASS_VOLTAGE,

    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_MEASUREMENT_ANGLE,
    STATE_CLASS_TOTAL_INCREASING,

    ICON_CURRENT_AC,
    ICON_CHIP,

    UNIT_AMPERE,
    UNIT_DEGREES,
    UNIT_HERTZ,
    UNIT_KILOWATT_HOURS,
    UNIT_VOLT,
    UNIT_VOLT_AMPS,
    UNIT_WATT,

    ENTITY_CATEGORY_DIAGNOSTIC,
)

# Import ICONS not included in esphome's const.py, from the local components const.py
from .const import (
    CONF_LOG_REGISTERS,
    CONF_LOG_SETTINGS,
    CONF_CH_A,
    CONF_CH_B,
    CONF_SWAP_AB,
    CONF_CHANNEL_SEL,
    CONF_RES_RATIO_IA,
    CONF_RES_RATIO_IB,
    CONF_RES_RATIO_U,

    ICON_ANGLE,
    ICON_POWER,
    ICON_ENERGY,
    ICON_FREQUENCY,
    ICON_VOLTAGE,
    ICON_SETTINGS,
)

from esphome.core import CORE

if CORE.is_esp32:
  DEPENDENCIES = ["esp32"]

if CORE.is_bk72xx:
  DEPENDENCIES = ["libretiny"]

hlw811x_ns = cg.esphome_ns.namespace("hlw811x")
HLW811xComponent = hlw811x_ns.class_("HLW811xComponent", cg.PollingComponent)
PublishAction = hlw811x_ns.class_("PublishAction", automation.Action)
DisplayStatusAction = hlw811x_ns.class_("DisplayStatusAction", automation.Action)
SetSpecialMeasurementChannelAction = hlw811x_ns.class_("SetSpecialMeasurementChannelAction", automation.Action)
ResetEnergyAction = hlw811x_ns.class_("ResetEnergyAction", automation.Action)

HLW811x_CurrentChannel = cg.global_ns.enum("HLW811x_CurrentChannel_t")
CHANNEL_LIST = {
    "A": HLW811x_CurrentChannel.HLW811X_CURRENT_CHANNEL_A,
    "B": HLW811x_CurrentChannel.HLW811X_CURRENT_CHANNEL_B,
}

def _validate(config):
    # Ensure all required SPI GPIOs are defined
    spi_pins = (CONF_CLK_PIN, CONF_CS_PIN, CONF_MISO_PIN, CONF_MOSI_PIN)
    present = [p for p in spi_pins if p in config]
    missing = [p for p in spi_pins if p not in config]

    if present and missing:
        raise cv.Invalid(
            "All four SPI pins must be defined together: missing "
            + ", ".join(missing)
        )

    # Detect duplicate GPIOs among SPI pins
    seen = {}
    for pin_key in present:
        pin_number = config[pin_key][CONF_NUMBER]
        if pin_number in seen:
            raise cv.Invalid(
                f"{pin_key} (GPIO{pin_number}) is already used by "
                f"{seen[pin_number]} - each SPI pin must use a distinct GPIO"
            )
        seen[pin_number] = pin_key

    # Check Channel A/B
    if config[CONF_CHANNEL_SEL] not in CHANNEL_LIST:
        raise cv.Invalid(
            f"{config[CONF_CHANNEL_SEL]} can only be {' or '.join(CHANNEL_LIST)}"
        )
    return config

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(HLW811xComponent),

            cv.Optional(CONF_CLK_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_CS_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_MISO_PIN): pins.gpio_input_pin_schema,
            cv.Optional(CONF_MOSI_PIN): pins.gpio_output_pin_schema,

            cv.Optional(CONF_SWAP_AB, default=False): cv.boolean,
            cv.Optional(CONF_CHANNEL_SEL, default="A"): cv.enum(
                CHANNEL_LIST, upper=True
            ),
            cv.Optional(CONF_RES_RATIO_IA, default=0.2): cv.float_,
            cv.Optional(CONF_RES_RATIO_IB, default=0.2): cv.float_,
            cv.Optional(CONF_RES_RATIO_U, default=1.0): cv.float_,

            cv.Optional(CONF_VOLTAGE): sensor.sensor_schema(
                unit_of_measurement=UNIT_VOLT,
                icon=ICON_VOLTAGE,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_VOLTAGE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_FREQUENCY): sensor.sensor_schema(
                unit_of_measurement=UNIT_HERTZ,
                icon=ICON_FREQUENCY,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_FREQUENCY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_POWER_FACTOR): sensor.sensor_schema(
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_POWER_FACTOR,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_PHASE_ANGLE): sensor.sensor_schema(
                unit_of_measurement=UNIT_DEGREES,
                icon=ICON_ANGLE,
                accuracy_decimals=2,
                #state_class=STATE_CLASS_MEASUREMENT,
                state_class=STATE_CLASS_MEASUREMENT_ANGLE,
            ),
            cv.Optional(CONF_APPARENT_POWER): sensor.sensor_schema(
                unit_of_measurement=UNIT_VOLT_AMPS,
                icon=ICON_POWER,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_APPARENT_POWER,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_CURRENT + CONF_CH_A): sensor.sensor_schema(
                unit_of_measurement=UNIT_AMPERE,
                icon=ICON_CURRENT_AC,
                accuracy_decimals=3,
                device_class=DEVICE_CLASS_CURRENT,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_CURRENT + CONF_CH_B): sensor.sensor_schema(
                unit_of_measurement=UNIT_AMPERE,
                icon=ICON_CURRENT_AC,
                accuracy_decimals=3,
                device_class=DEVICE_CLASS_CURRENT,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_ACTIVE_POWER + CONF_CH_A): sensor.sensor_schema(
                unit_of_measurement=UNIT_WATT,
                icon=ICON_POWER,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_POWER,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_ACTIVE_POWER + CONF_CH_B): sensor.sensor_schema(
                unit_of_measurement=UNIT_WATT,
                icon=ICON_POWER,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_POWER,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_ENERGY + CONF_CH_A): sensor.sensor_schema(
                unit_of_measurement=UNIT_KILOWATT_HOURS,
                icon=ICON_ENERGY,
                accuracy_decimals=3,
                device_class=DEVICE_CLASS_ENERGY,
                state_class=STATE_CLASS_TOTAL_INCREASING,
            ),
            cv.Optional(CONF_ENERGY + CONF_CH_B): sensor.sensor_schema(
                unit_of_measurement=UNIT_KILOWATT_HOURS,
                icon=ICON_ENERGY,
                accuracy_decimals=3,
                device_class=DEVICE_CLASS_ENERGY,
                state_class=STATE_CLASS_TOTAL_INCREASING,
            ),

            cv.Optional(CONF_LOG_REGISTERS): text_sensor.text_sensor_schema(
                icon=ICON_CHIP,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            ),
            cv.Optional(CONF_LOG_SETTINGS): text_sensor.text_sensor_schema(
                icon=ICON_SETTINGS,
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            ),
        }
    )
    .extend(cv.polling_component_schema("60s")),
    cv.only_on([PLATFORM_ESP32, PLATFORM_BK72XX]),
    _validate,
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    if clk_pin_config := config.get(CONF_CLK_PIN):
        clk_pin = await cg.gpio_pin_expression(clk_pin_config)
        cg.add(var.set_clk_pin(clk_pin))
    if cs_pin_config := config.get(CONF_CS_PIN):
        cs_pin = await cg.gpio_pin_expression(cs_pin_config)
        cg.add(var.set_cs_pin(cs_pin))
    if miso_pin_config := config.get(CONF_MISO_PIN):
        miso_pin = await cg.gpio_pin_expression(miso_pin_config)
        cg.add(var.set_miso_pin(miso_pin))
    if mosi_pin_config := config.get(CONF_MOSI_PIN):
        mosi_pin = await cg.gpio_pin_expression(mosi_pin_config)
        cg.add(var.set_mosi_pin(mosi_pin))

    if swap_config := config.get(CONF_SWAP_AB):
        cg.add(var.set_swap_ab(swap_config))
    if channel_sel_config := config.get(CONF_CHANNEL_SEL):
        cg.add(var.set_channel_sel(channel_sel_config))
    if res_ratio_ia_config := config.get(CONF_RES_RATIO_IA):
        cg.add(var.set_res_ratio_ia(res_ratio_ia_config))
    if res_ratio_ib_config := config.get(CONF_RES_RATIO_IB):
        cg.add(var.set_res_ratio_ib(res_ratio_ib_config))
    if res_ratio_u_config := config.get(CONF_RES_RATIO_U):
        cg.add(var.set_res_ratio_u(res_ratio_u_config))

    if voltage_config := config.get(CONF_VOLTAGE):
        sens = await sensor.new_sensor(voltage_config)
        cg.add(var.set_voltage_sensor(sens))
    if frequency_config := config.get(CONF_FREQUENCY):
        sens = await sensor.new_sensor(frequency_config)
        cg.add(var.set_frequency_sensor(sens))
    if power_factor_config := config.get(CONF_POWER_FACTOR):
        sens = await sensor.new_sensor(power_factor_config)
        cg.add(var.set_power_factor_sensor(sens))
    if phase_angle_config := config.get(CONF_PHASE_ANGLE):
        sens = await sensor.new_sensor(phase_angle_config)
        cg.add(var.set_phase_angle_sensor(sens))
    if apparent_power_config := config.get(CONF_APPARENT_POWER):
        sens = await sensor.new_sensor(apparent_power_config)
        cg.add(var.set_apparent_power_sensor(sens))
    if current_a_config := config.get(CONF_CURRENT + CONF_CH_A):
        sens = await sensor.new_sensor(current_a_config)
        cg.add(var.set_current_a_sensor(sens))
    if current_b_config := config.get(CONF_CURRENT + CONF_CH_B):
        sens = await sensor.new_sensor(current_b_config)
        cg.add(var.set_current_b_sensor(sens))
    if active_power_a_config := config.get(CONF_ACTIVE_POWER + CONF_CH_A):
        sens = await sensor.new_sensor(active_power_a_config)
        cg.add(var.set_active_power_a_sensor(sens))
    if active_power_b_config := config.get(CONF_ACTIVE_POWER + CONF_CH_B):
        sens = await sensor.new_sensor(active_power_b_config)
        cg.add(var.set_active_power_b_sensor(sens))
    if energy_a_config := config.get(CONF_ENERGY + CONF_CH_A):
        sens = await sensor.new_sensor(energy_a_config)
        cg.add(var.set_energy_a_sensor(sens))
    if energy_b_config := config.get(CONF_ENERGY + CONF_CH_B):
        sens = await sensor.new_sensor(energy_b_config)
        cg.add(var.set_energy_b_sensor(sens))
    if log_registers_config := config.get(CONF_LOG_REGISTERS):
        sens = await text_sensor.new_text_sensor(log_registers_config)
        cg.add(var.set_log_registers_text_sensor(sens))
    if log_settings_config := config.get(CONF_LOG_SETTINGS):
        sens = await text_sensor.new_text_sensor(log_settings_config)
        cg.add(var.set_log_settings_text_sensor(sens))

#
# Actions without any parameter
#
HLW811X_ACTION_SCHEMA = automation.maybe_simple_id(
    {
        cv.GenerateID(): cv.use_id(HLW811xComponent),
    }
)

@automation.register_action("hlw811x.publish", PublishAction, HLW811X_ACTION_SCHEMA, synchronous=True)
@automation.register_action("hlw811x.display_status", DisplayStatusAction, HLW811X_ACTION_SCHEMA, synchronous=True)

async def hlw811x_action_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    return var


#
# Actions with Channel parameter
#
HLW811X_ACTION_CHANNEL_LIST_SCHEMA = automation.maybe_simple_id(
    {
        cv.GenerateID(): cv.use_id(HLW811xComponent),
        cv.Required(CONF_CHANNEL): cv.enum(
            CHANNEL_LIST, upper=True
        ),
    },
)

@automation.register_action("hlw811x.set_channel_sel", SetSpecialMeasurementChannelAction, HLW811X_ACTION_CHANNEL_LIST_SCHEMA, synchronous=True)
@automation.register_action("hlw811x.reset_energy", ResetEnergyAction, HLW811X_ACTION_CHANNEL_LIST_SCHEMA, synchronous=True)

async def hlw811x_channel_action_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    template_ = await cg.templatable(config[CONF_CHANNEL], args, HLW811x_CurrentChannel)
    cg.add(var.set_channel(template_))
    return var
