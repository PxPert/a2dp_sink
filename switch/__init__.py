import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_TYPE
from esphome.types import ConfigType

from .. import (
    CONF_A2DP_SINK_ID,
    A2DPSinkHub,
    a2dp_sink_ns,
    request_connection_state_support,
)

CODEOWNERS = ["@PxPert"]
DEPENDENCIES = ["a2dp_sink"]

A2DPSwitchConnection = a2dp_sink_ns.class_(
    "A2DPSwitchConnection",
    switch.Switch,
    cg.Component,
)

A2DPSwitchBluetooth = a2dp_sink_ns.class_(
    "A2DPSwitchBluetooth",
    switch.Switch,
    cg.Component,
)

A2DP_SWITCH_TYPES = {
    "connection": A2DPSwitchConnection,
    "bluetooth": A2DPSwitchBluetooth,
}

def _validate_type(config):
    """Select the switch class based on CONF_TYPE and bake it into CONF_ID."""
    switch_class = A2DP_SWITCH_TYPES[config[CONF_TYPE]]
    config[CONF_ID] = cv.declare_id(switch_class)(config[CONF_ID])
    return config


def _request_roles(config: ConfigType) -> ConfigType:
    """Request the switch role for the A2DP Sink."""
    if config[CONF_TYPE] in ("connection"):
        request_connection_state_support()
        
    return config


CONFIG_SCHEMA = cv.All(
    switch.switch_schema(switch.Switch).extend(
        {
            cv.GenerateID(CONF_A2DP_SINK_ID): cv.use_id(A2DPSinkHub),
            cv.Required(CONF_TYPE): cv.enum(A2DP_SWITCH_TYPES, lower=True),
        }
    ),
    _validate_type,
    _request_roles,
    cv.only_on_esp32,
)


async def to_code(config: ConfigType) -> None:
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await cg.register_parented(var, config[CONF_A2DP_SINK_ID])
    await switch.register_switch(var, config)