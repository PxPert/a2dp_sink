from dataclasses import dataclass

import esphome.codegen as cg
from esphome.components import esp32

import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    CONF_NAME,
    CONF_TRIGGER_ID,
)
from esphome.core import CORE

from esphome import automation

CODEOWNERS = ["@PxPert"]
# DEPENDENCIES = ['uart']

DOMAIN = "a2dpsink"

# CONFIG-IDs
CONF_A2DP_SINK_ID = "a2dp_sink_id"  # Used by child components

CONF_A2DP_AUTO_RECONNECT = "auto_reconnect"
CONF_A2DP_MINIMAL_BLUETOOTH = "minimal_bluetooth"

# Automation trigger IDs
CONF_ON_CONNECTION_STATE = "on_connection_state"
CONF_ON_PLAYBACK_STATUS = "on_playback_status"
CONF_ON_PLAYBACK_POSITION = "on_playback_position"
CONF_ON_RSSI = "on_rssi"
CONF_ON_METADATA = "on_metadata"
CONF_ON_PEER_NAME = "on_peer_name"
CONF_ON_VOLUME_CHANGE = "on_volume_change"
CONF_ON_SAMPLE_RATE = "on_sample_rate"
CONF_ON_AVRCP_CONNECTION_STATE = "on_avrcp_connection_state"
CONF_ON_AUDIO_STATE = "on_audio_state"


# ------------------------------
#  Namespace & Hub class
# ------------------------------

a2dp_sink_ns = cg.esphome_ns.namespace('a2dp_sink')


A2DPSinkHub = a2dp_sink_ns.class_(
    'A2DPSinkHub',
    cg.Component,
)

# ------------------------------
#  Automation Triggers
# ------------------------------

ConnectionStateTrigger = a2dp_sink_ns.class_(
    "ConnectionStateTrigger",
    automation.Trigger.template(),
)

PlaybackStatusTrigger = a2dp_sink_ns.class_(
    "PlaybackStatusTrigger",
    automation.Trigger.template(),
)

PlaybackPositionTrigger = a2dp_sink_ns.class_(
    "PlaybackPositionTrigger",
    automation.Trigger.template(),
)

RssiTrigger = a2dp_sink_ns.class_(
    "RssiTrigger",
    automation.Trigger.template(),
)

MetadataUpdateTrigger = a2dp_sink_ns.class_(
    "MetadataUpdateTrigger",
    automation.Trigger.template(),
)

PeerNameTrigger = a2dp_sink_ns.class_(
    "PeerNameTrigger",
    automation.Trigger.template(),
)

VolumeChangeTrigger = a2dp_sink_ns.class_(
    "VolumeChangeTrigger",
    automation.Trigger.template(),
)

SampleRateTrigger = a2dp_sink_ns.class_(
    "SampleRateTrigger",
    automation.Trigger.template(),
)

AVRCPConnectionStateTrigger = a2dp_sink_ns.class_(
    "AVRCPConnectionStateTrigger",
    automation.Trigger.template(),
)

AudioStateTrigger = a2dp_sink_ns.class_(
    "AudioStateTrigger",
    automation.Trigger.template(),
)


# ------------------------------
#  Feature flags — subcomponents call request_*_support() to opt in
# ------------------------------

@dataclass
class A2dpSinkConfiguration:
    position_support: bool = False
    rssi_support: bool = False
    metadata_support: bool = False
    peer_name_support: bool = False
    volume_support: bool = False
    playback_status_support: bool = False
    connection_state_support: bool = False
    sample_rate_support: bool = False
    avrcp_connection_state_support: bool = False
    audio_state_support: bool = False


def _get_data() -> A2dpSinkConfiguration:
    if DOMAIN not in CORE.data:
        CORE.data[DOMAIN] = A2dpSinkConfiguration()
    return CORE.data[DOMAIN]

def request_position_support() -> None:
    """Request track position support for A2DP Sink."""
    _get_data().position_support = True

def request_rssi_support() -> None:
    """Request rssi signal quality support for A2DP Sink."""
    _get_data().rssi_support = True

def request_metadata_support() -> None:
    """Request metadata support for A2DP Sink."""
    _get_data().metadata_support = True

def request_peer_name_support() -> None:
    """Request peer name support for A2DP Sink."""
    _get_data().peer_name_support = True

def request_volume_support() -> None:
    """Request volume support for A2DP Sink."""
    _get_data().volume_support = True

def request_playback_status_support() -> None:
    """Request playback status support for A2DP Sink."""
    _get_data().playback_status_support = True

def request_connection_state_support() -> None:
    """Request connection state notifications support for A2DP Sink."""
    _get_data().connection_state_support = True

def request_sample_rate_support() -> None:
    """Request sample rate change notifications support for A2DP Sink."""
    _get_data().sample_rate_support = True

def request_avrcp_connection_state_support() -> None:
    """Request AVRCP connection state notifications support for A2DP Sink."""
    _get_data().avrcp_connection_state_support = True

def request_audio_state_support() -> None:
    """Request audio state notifications support for A2DP Sink."""
    _get_data().audio_state_support = True



# ------------------------------
#  Automation trigger schemas
# ------------------------------

ON_CONNECTION_STATE_SCHEMA = automation.validate_automation(
    {
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(ConnectionStateTrigger),
    }
)

ON_PLAYBACK_STATUS_SCHEMA = automation.validate_automation(
    {
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(PlaybackStatusTrigger),
    }
)

ON_PLAYBACK_POSITION_SCHEMA = automation.validate_automation(
    {
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(PlaybackPositionTrigger),
    }
)

ON_RSSI_SCHEMA = automation.validate_automation(
    {
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(RssiTrigger),
    }
)

ON_METADATA_SCHEMA = automation.validate_automation(
    {
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(MetadataUpdateTrigger),
    }
)

ON_PEER_NAME_SCHEMA = automation.validate_automation(
    {
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(PeerNameTrigger),
    }
)

ON_VOLUME_CHANGE_SCHEMA = automation.validate_automation(
    {
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(VolumeChangeTrigger),
    }
)

ON_SAMPLE_RATE_SCHEMA = automation.validate_automation(
    {
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(SampleRateTrigger),
    }
)

ON_AVRCP_CONNECTION_STATE_SCHEMA = automation.validate_automation(
    {
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(AVRCPConnectionStateTrigger),
    }
)

ON_AUDIO_STATE_SCHEMA = automation.validate_automation(
    {
        cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(AudioStateTrigger),
    }
)


# ------------------------------
#  Parameter Config
# ------------------------------
CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(A2DPSinkHub),
            cv.Required(CONF_NAME): cv.string,
            cv.Optional(CONF_A2DP_AUTO_RECONNECT, default=True): cv.boolean,
            cv.Optional(CONF_A2DP_MINIMAL_BLUETOOTH, default=True): cv.boolean,
            cv.Optional(CONF_ON_CONNECTION_STATE): ON_CONNECTION_STATE_SCHEMA,
            cv.Optional(CONF_ON_PLAYBACK_STATUS): ON_PLAYBACK_STATUS_SCHEMA,
            cv.Optional(CONF_ON_PLAYBACK_POSITION): ON_PLAYBACK_POSITION_SCHEMA,
            cv.Optional(CONF_ON_RSSI): ON_RSSI_SCHEMA,
            cv.Optional(CONF_ON_METADATA): ON_METADATA_SCHEMA,
            cv.Optional(CONF_ON_PEER_NAME): ON_PEER_NAME_SCHEMA,
            cv.Optional(CONF_ON_VOLUME_CHANGE): ON_VOLUME_CHANGE_SCHEMA,
            cv.Optional(CONF_ON_SAMPLE_RATE): ON_SAMPLE_RATE_SCHEMA,
            cv.Optional(CONF_ON_AVRCP_CONNECTION_STATE): ON_AVRCP_CONNECTION_STATE_SCHEMA,
            cv.Optional(CONF_ON_AUDIO_STATE): ON_AUDIO_STATE_SCHEMA,
        }
    ),
    cv.only_on_esp32,
)


# ------------------------------
#  Actions
# ------------------------------

async def to_code(config):
    # A2DP library
    esp32.add_idf_component(
        name="ESP32-A2DP",
        repo="https://github.com/PxPert/ESP32-A2DP",
        ref="esphome"
    )

    # Bluetooth settings
    esp32.add_idf_sdkconfig_option("CONFIG_BT_ENABLED", True)
    esp32.add_idf_sdkconfig_option("CONFIG_BT_CLASSIC_ENABLED", True)
    esp32.add_idf_sdkconfig_option("CONFIG_BT_A2DP_ENABLE", True)
    esp32.add_idf_sdkconfig_option("CONFIG_BT_SPP_ENABLED", True)
    esp32.add_idf_sdkconfig_option("CONFIG_BT_BLUEDROID_ENABLED", True)
    esp32.add_idf_sdkconfig_option("CONFIG_BT_AVRCP_CT_COVER_ART_ENABLED", False)
    esp32.add_idf_sdkconfig_option("CONFIG_BTDM_CTRL_MODEM_SLEEP", False)
    

    # Stripe unnecessary Bluetooth protocols to save memory
    if (config[CONF_A2DP_MINIMAL_BLUETOOTH]):
        esp32.add_idf_sdkconfig_option("CONFIG_BTDM_CTRL_MODE_BR_EDR_ONLY", True)
        esp32.add_idf_sdkconfig_option("CONFIG_BT_BLE_ENABLED", False)
        esp32.add_idf_sdkconfig_option("CONFIG_BT_NIMBLE_ENABLED", False)
        esp32.add_idf_sdkconfig_option("CONFIG_BT_GATTS_ENABLE", False)
    else:
        esp32.add_idf_sdkconfig_option("CONFIG_BTDM_CTRL_MODE_BTDM", True)


    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_name(config[CONF_NAME]))
    cg.add(var.set_auto_reconnect(config[CONF_A2DP_AUTO_RECONNECT]))

    # Build automation triggers
    # Using a trigger automatically requests the corresponding feature support,
    # so the USE_A2DP_* define is emitted and the C++ callback infrastructure
    # is compiled in.

    if CONF_ON_CONNECTION_STATE in config:
        request_connection_state_support()
    if CONF_ON_PLAYBACK_STATUS in config:
        request_playback_status_support()
    if CONF_ON_PLAYBACK_POSITION in config:
        request_position_support()
    if CONF_ON_RSSI in config:
        request_rssi_support()
    if CONF_ON_METADATA in config:
        request_metadata_support()
    if CONF_ON_PEER_NAME in config:
        request_peer_name_support()
    if CONF_ON_VOLUME_CHANGE in config:
        request_volume_support()
    if CONF_ON_SAMPLE_RATE in config:
        request_sample_rate_support()
    if CONF_ON_AVRCP_CONNECTION_STATE in config:
        request_avrcp_connection_state_support()
    if CONF_ON_AUDIO_STATE in config:
        request_audio_state_support()

    for conf in config.get(CONF_ON_CONNECTION_STATE, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(
            trigger,
            [(cg.uint8, "state")],
            conf
        )

    for conf in config.get(CONF_ON_PLAYBACK_STATUS, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(
            trigger,
            [(cg.uint8, "playback")],
            conf
        )

    for conf in config.get(CONF_ON_PLAYBACK_POSITION, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(
            trigger,
            [(cg.uint32, "pos")],
            conf
        )

    for conf in config.get(CONF_ON_RSSI, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(
            trigger,
            [(cg.int8, "rssi_delta")],
            conf
        )

    for conf in config.get(CONF_ON_METADATA, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(
            trigger,
            [(cg.uint8, "type"), (cg.std_string, "text")],
            conf
        )

    for conf in config.get(CONF_ON_PEER_NAME, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(
            trigger,
            [(cg.std_string, "name")],
            conf
        )

    for conf in config.get(CONF_ON_VOLUME_CHANGE, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(
            trigger,
            [(cg.uint8, "volume")],
            conf
        )

    for conf in config.get(CONF_ON_SAMPLE_RATE, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(
            trigger,
            [(cg.uint16, "sample_rate")],
            conf
        )

    for conf in config.get(CONF_ON_AVRCP_CONNECTION_STATE, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(
            trigger,
            [(cg.bool_, "connected")],
            conf
        )

    for conf in config.get(CONF_ON_AUDIO_STATE, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(
            trigger,
            [(cg.uint8, "state")],
            conf
        )

    # Emit USE_A2DP_* defines so the hub C++ can conditionally compile
    # callback infrastructure only for features the user actually needs.
    data = _get_data()
    if data.position_support:
        cg.add_define("USE_A2DP_POS", True)
    if data.rssi_support:
        cg.add_define("USE_A2DP_RSSI", True)
    if data.metadata_support:
        cg.add_define("USE_A2DP_METADATA", True)
    if data.peer_name_support:
        cg.add_define("USE_A2DP_PEER_NAME", True)
    if data.volume_support:
        cg.add_define("USE_A2DP_VOLUME", True)
    if data.playback_status_support:
        cg.add_define("USE_A2DP_PLAYBACK_STATUS", True)
    if data.connection_state_support:
        cg.add_define("USE_A2DP_CONNECTION_STATE", True)
    if data.sample_rate_support:
        cg.add_define("USE_A2DP_SAMPLE_RATE", True)
    if data.avrcp_connection_state_support:
        cg.add_define("USE_A2DP_AVRCP_CONNECTION_STATE", True)
    if data.audio_state_support:
        cg.add_define("USE_A2DP_AUDIO_STATE", True)
