#pragma once

#include <utility>
#include <vector>

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/core/hal.h"
#include "a2dp_sink_hub.h"

namespace esphome {
namespace a2dp_sink {

#ifdef USE_A2DP_CONNECTION_STATE
/**
 * @brief Trigger fired when the A2DP connection state changes.
 *
 * Emits the new connection state as a uint8_t value.
 */
class ConnectionStateTrigger : public Trigger<uint8_t> {
 public:
  explicit ConnectionStateTrigger(A2DPSinkHub *parent) {
    parent->add_connection_state_callbacks([this](esp_a2d_connection_state_t state, void *user_data) {
      this->trigger((uint8_t)state);
    });
  }
};
#endif

#ifdef USE_A2DP_PLAYBACK_STATUS
/**
 * @brief Trigger fired when the AVRC playback status changes.
 *
 * Emits the new playback status as a uint8_t value.
 */
class PlaybackStatusTrigger : public Trigger<uint8_t> {
 public:
  explicit PlaybackStatusTrigger(A2DPSinkHub *parent) {
    parent->add_playback_status_callbacks([this](esp_avrc_playback_stat_t playback) {
      this->trigger((uint8_t)playback);
    });
  }
};
#endif

#ifdef USE_A2DP_POS
/**
 * @brief Trigger fired when the playback position changes.
 *
 * Emits the current position in milliseconds as a uint32_t.
 */
class PlaybackPositionTrigger : public Trigger<uint32_t> {
 public:
  explicit PlaybackPositionTrigger(A2DPSinkHub *parent) {
    parent->add_playback_position_callbacks([this](uint32_t pos) {
      this->trigger(pos);
    });
  }
};
#endif

#ifdef USE_A2DP_RSSI
/**
 * @brief Trigger fired when the RSSI signal strength delta changes.
 *
 * Emits the RSSI delta value as an int8_t.
 */
class RssiTrigger : public Trigger<int8_t> {
 public:
  explicit RssiTrigger(A2DPSinkHub *parent) {
    parent->add_rssi_callback([this](esp_bt_gap_cb_param_t::read_rssi_delta_param& rssi) {
      this->trigger(rssi.rssi_delta);
    });
  }
};
#endif

#ifdef USE_A2DP_METADATA
/**
 * @brief Trigger fired when AVRC metadata is updated.
 *
 * Emits the metadata attribute type as uint8_t and the text value as std::string.
 */
class MetadataUpdateTrigger : public Trigger<uint8_t, std::string> {
 public:
  explicit MetadataUpdateTrigger(A2DPSinkHub *parent) {
    parent->add_metadata_update_callback([this](const A2DPSinkMetadata &meta) {
      this->trigger(meta.type(), std::string((const char*)meta.text()));
    });
  }
};
#endif

#ifdef USE_A2DP_PEER_NAME
/**
 * @brief Trigger fired when the peer device name is resolved.
 *
 * Emits the peer device name as a std::string.
 */
class PeerNameTrigger : public Trigger<std::string> {
 public:
  explicit PeerNameTrigger(A2DPSinkHub *parent) {
    parent->add_peer_name_callback([this](const char* name) {
      this->trigger(std::string(name));
    });
  }
};
#endif

#ifdef USE_A2DP_VOLUME
/**
 * @brief Trigger fired when the A2DP volume changes.
 *
 * Emits the new volume level as a uint8_t.
 */
class VolumeChangeTrigger : public Trigger<uint8_t> {
 public:
  explicit VolumeChangeTrigger(A2DPSinkHub *parent) {
    parent->add_volume_change_callback([this](int volume) {
      this->trigger((uint8_t)volume);
    });
  }
};
#endif

#ifdef USE_A2DP_AUDIO_STATE
/**
 * @brief Trigger fired when the A2DP audio state changes.
 *
 * Emits the new audio state as a uint8_t.
 */
class AudioStateTrigger : public Trigger<uint8_t> {
 public:
  explicit AudioStateTrigger(A2DPSinkHub *parent) {
    parent->add_audio_state_callback([this](esp_a2d_audio_state_t state) {
      this->trigger((uint8_t)state);
    });
  }
};
#endif

#ifdef USE_A2DP_SAMPLE_RATE
/**
 * @brief Trigger fired when the A2DP sample rate changes.
 *
 * Emits the new sample rate in Hz as a uint16_t.
 */
class SampleRateTrigger : public Trigger<uint16_t> {
 public:
  explicit SampleRateTrigger(A2DPSinkHub *parent) {
    parent->add_sample_rate_callback([this](uint16_t sample_rate) {
      this->trigger(sample_rate);
    });
  }
};
#endif

#ifdef USE_A2DP_AVRCP_CONNECTION_STATE
/**
 * @brief Trigger fired when the AVRCP connection state changes.
 *
 * Emits a bool indicating whether AVRCP is connected.
 */
class AVRCPConnectionStateTrigger : public Trigger<bool> {
 public:
  explicit AVRCPConnectionStateTrigger(A2DPSinkHub *parent) {
    parent->add_avrc_connection_state_callback([this](bool connected) {
      this->trigger(connected);
    });
  }
};
#endif

}  // namespace a2dp_sink
}  // namespace esphome
