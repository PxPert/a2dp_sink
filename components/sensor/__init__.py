import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_TYPE, UNIT_EMPTY, ICON_EMPTY
from esphome.types import ConfigType

from .. import (
    CONF_A2DP_SINK_ID,
    A2DPSinkHub,
    a2dp_sink_ns,
    request_position_support,
    request_rssi_support,
    request_metadata_support,
    request_sample_rate_support,
)

CODEOWNERS = ["@PxPert"]
DEPENDENCIES = ["a2dp_sink"]

A2DPMetadataNumericSensor = a2dp_sink_ns.class_(
    "A2DPMetadataNumericSensor",
    sensor.Sensor,
    cg.Component,
)

A2DPSinkTrackPositionSensor = a2dp_sink_ns.class_(
    "A2DPSinkTrackPositionSensor",
    sensor.Sensor,
    cg.Component,
)

A2DPSinkRssiSensor = a2dp_sink_ns.class_(
    "A2DPSinkRssiSensor",
    sensor.Sensor,
    cg.Component,
)

A2DPSinkSampleRateSensor = a2dp_sink_ns.class_(
    "A2DPSinkSampleRateSensor",
    sensor.Sensor,
    cg.Component,
)

A2DPSinkChannelsSensor = a2dp_sink_ns.class_(
    "A2DPSinkChannelsSensor",
    sensor.Sensor,
    cg.Component,
)

A2DPSinkNumericMetadataTypes = a2dp_sink_ns.enum("A2DPSinkMetadataTypes", is_class=True)
A2DPSINK_NUMERIC_METADATA_TYPES = {
    "tracknum": A2DPSinkNumericMetadataTypes.TRACKNUM,
    "playingtime": A2DPSinkNumericMetadataTypes.PLAYINGTIME,
    "num_tracks": A2DPSinkNumericMetadataTypes.NUM_TRACKS,
    "trackposition": "",
    "rssi": "",
    "samplereate": "",
    "channels": "",
}

A2DPSINK_NUMERIC_TYPES = {
    "tracknum": A2DPMetadataNumericSensor,
    "playingtime": A2DPMetadataNumericSensor,
    "num_tracks": A2DPMetadataNumericSensor,
    "trackposition": A2DPSinkTrackPositionSensor,
    "rssi": A2DPSinkRssiSensor,
    "samplereate": A2DPSinkSampleRateSensor,
    "channels": A2DPSinkChannelsSensor,
}

A2DPSINK_NUMERIC_REQUEST_MAP = {
    "tracknum":      request_metadata_support,
    "playingtime":   request_metadata_support,
    "num_tracks":    request_metadata_support,
    "trackposition": request_position_support,
    "rssi":          request_rssi_support,
    "samplereate":   request_sample_rate_support,
    "channels":      request_sample_rate_support,
}

def _validate_type(config):
    """Select the sensor class based on CONF_TYPE and bake it into CONF_ID."""
    sensor_class = A2DPSINK_NUMERIC_TYPES[config[CONF_TYPE]]
    config[CONF_ID] = cv.declare_id(sensor_class)(config[CONF_ID])
    return config


def _request_roles(config: ConfigType) -> ConfigType:
    """Request only the sensor role needed for this A2DP Sink type."""
    A2DPSINK_NUMERIC_REQUEST_MAP[config[CONF_TYPE]]()
    return config


CONFIG_SCHEMA = cv.All(
    sensor.sensor_schema().extend(
        {
            cv.GenerateID(CONF_A2DP_SINK_ID): cv.use_id(A2DPSinkHub),
            cv.Required(CONF_TYPE): cv.enum(A2DPSINK_NUMERIC_METADATA_TYPES, lower=True),
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
    await sensor.register_sensor(var, config)

    # Only set metadata_type for A2DPMetadataNumericSensor types
    if config[CONF_TYPE] in ("tracknum", "playingtime", "num_tracks"):
        cg.add(var.set_metadata_type(config[CONF_TYPE]))