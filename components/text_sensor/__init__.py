import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_TYPE
from esphome.types import ConfigType


from .. import (
    CONF_A2DP_SINK_ID,
    A2DPSinkHub,
    a2dp_sink_ns,
    request_metadata_support,
    request_peer_name_support
)


CODEOWNERS = ["@PxPert"]
DEPENDENCIES = ["a2dp_sink"]

A2DPMetadataTextSensor = a2dp_sink_ns.class_(
    "A2DPMetadataTextSensor",
    text_sensor.TextSensor,
    cg.Component,
)

A2DPSinkPeerTextSensor = a2dp_sink_ns.class_(
    "A2DPSinkPeerTextSensor",
    text_sensor.TextSensor,
    cg.Component,
)

A2DPSinkMetadataTypes = a2dp_sink_ns.enum("A2DPSinkMetadataTypes", is_class=True)
A2DPSinkPeerRequestTypes = a2dp_sink_ns.enum("A2DPSinkPeerRequestTypes", is_class=True)
A2DPSINK_TEXT_METADATA_TYPES = {
    "title": A2DPSinkMetadataTypes.TITLE,
    "artist": A2DPSinkMetadataTypes.ARTIST,
    "album": A2DPSinkMetadataTypes.ALBUM,
    "genre": A2DPSinkMetadataTypes.GENRE,
    "peername": A2DPSinkPeerRequestTypes.PEERNAME,
    "peeraddr": A2DPSinkPeerRequestTypes.PEERADDR,
}

A2DPSINK_TEXT_TYPES = {
    "title": A2DPMetadataTextSensor,
    "artist": A2DPMetadataTextSensor,
    "album": A2DPMetadataTextSensor,
    "genre": A2DPMetadataTextSensor,
    "peername": A2DPSinkPeerTextSensor,
    "peeraddr": A2DPSinkPeerTextSensor,
}

A2DPSINK_TEXT_REQUEST_MAP = {
    "title": request_metadata_support,
    "artist": request_metadata_support,
    "album": request_metadata_support,
    "genre": request_metadata_support,
    "peername": request_peer_name_support,
    "peeraddr": request_peer_name_support,
}

def _validate_type(config):
    """Select the text sensor class based on CONF_TYPE and bake it into CONF_ID."""
    sensor_class = A2DPSINK_TEXT_TYPES[config[CONF_TYPE]]
    config[CONF_ID] = cv.declare_id(sensor_class)(config[CONF_ID])
    return config


def _request_roles(config: ConfigType) -> ConfigType:
    """Request the text_sensor role for the A2DP Sink."""
    A2DPSINK_TEXT_REQUEST_MAP[config[CONF_TYPE]]()
    return config


CONFIG_SCHEMA = cv.All(
    text_sensor.text_sensor_schema().extend(
        {
            cv.GenerateID(CONF_A2DP_SINK_ID): cv.use_id(A2DPSinkHub),
            cv.Required(CONF_TYPE): cv.enum(A2DPSINK_TEXT_METADATA_TYPES, lower=True),
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
    await text_sensor.register_text_sensor(var, config)

    # Only set metadata_type for A2DPMetadataTextSensor types
    if config[CONF_TYPE] in ("title", "artist", "album","genre"):
        cg.add(var.set_metadata_type(config[CONF_TYPE]))
    else:
        cg.add(var.set_peer_type(config[CONF_TYPE]))