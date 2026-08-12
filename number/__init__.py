import esphome.codegen as cg
from esphome.components import number
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID
)
from esphome.types import ConfigType

from .. import (
    CONF_A2DP_SINK_ID,
    A2DPSinkHub,
    a2dp_sink_ns,
    request_volume_support,
)

CODEOWNERS = ["@PxPert"]
DEPENDENCIES = ["a2dp_sink"]

A2DPSinkVolumeNumber = a2dp_sink_ns.class_(
    "A2DPSinkVolumeNumber",
    number.Number,
    cg.Component,
)


def _request_roles(config: ConfigType) -> ConfigType:
    """Request the number role for the A2DP Sink."""
    request_volume_support()
    return config


CONFIG_SCHEMA = cv.All(
    number.number_schema(A2DPSinkVolumeNumber)
    .extend(
        {
            cv.GenerateID(CONF_A2DP_SINK_ID): cv.use_id(A2DPSinkHub),
        }
    ),
    _request_roles,
    cv.only_on_esp32,
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await cg.register_parented(var, config[CONF_A2DP_SINK_ID])
    await number.register_number(var, config, min_value=0, max_value=127, step=5)