import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import audio, media_source
from esphome.const import (
    CONF_ID,
)
from esphome.types import ConfigType

from esphome import automation

from .. import (
    CONF_A2DP_SINK_ID,
    A2DPSinkHub,
    a2dp_sink_ns,
    request_playback_status_support,
    request_connection_state_support,
    request_sample_rate_support,
    request_avrcp_connection_state_support,
    request_audio_state_support
)

AUTO_LOAD = ["audio"]
CODEOWNERS = ["@PxPert"]
# DEPENDENCIES = ['uart']
DOMAIN = "a2dpsink"
DEPENDENCIES = ["a2dp_sink"]

# CONFIG-IDs
CONF_A2DPSINK_ID = "a2dp_sink_id"


# ------------------------------
# ------------------------------


A2DPSinkMediaSource = a2dp_sink_ns.class_(
    'A2DPSinkMediaSource',
    cg.Component,
    media_source.MediaSource,
)


def _request_roles(config: ConfigType) -> ConfigType:
    """Request the media_source role for the A2DP Sink."""
    request_playback_status_support()
    request_connection_state_support()
    request_sample_rate_support()
    request_avrcp_connection_state_support()
    request_audio_state_support()
    return config


# ------------------------------
#  Parameter Config
# ------------------------------
CONFIG_SCHEMA = cv.All(
    media_source.media_source_schema(
        A2DPSinkMediaSource,
    )
    .extend(
        {
            cv.GenerateID(CONF_A2DP_SINK_ID): cv.use_id(A2DPSinkHub),
        }
    ),
    _request_roles,
    cv.only_on_esp32,
)


# ------------------------------
#  Actions
# ------------------------------

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await media_source.register_media_source(var, config)

    a2dpsink_hub = await cg.get_variable(config[CONF_A2DP_SINK_ID])
    await cg.register_parented(var, a2dpsink_hub)

