#include "a2dp_sink_text_sensor.h"

#include <string>

namespace esphome::a2dp_sink {

static const char *const TAG = "a2dp_sink.text_sensor";

/**
 * @brief Log the text sensor configuration.
 */
void A2DPSinkTextSensor::dump_config() { LOG_TEXT_SENSOR("", "A2DPSink", this); }


// Dedup to avoid frontend churn; TextSensor::publish_state already dedups the string assign but still notifies.
/**
 * @brief Publish a new text value only if it differs from the current state.
 * @param value The new text value.
 */
void A2DPSinkTextSensor::publish_if_changed_(const char *value) {
  if (this->get_raw_state() != value) {
    this->publish_state(value);
  }
}

#ifdef USE_A2DP_METADATA

// THREAD CONTEXT: Main loop. The registered metadata callback also fires on the main loop
// (SendspinHub dispatches metadata from client_->loop()).
/**
 * @brief Initialize the metadata text sensor.
 *
 * Disables the loop, requests the specific metadata attribute from the parent hub,
 * and subscribes to metadata update callbacks. Filters by metadata type and publishes
 * the matching text value.
 */
void A2DPMetadataTextSensor::setup() {
  this->disable_loop();

  // Request this specific metadata attribute from the parent hub
  this->parent_->add_metadata_attribute((uint8_t)this->metadata_type_);

  this->parent_->add_metadata_update_callback([this](const A2DPSinkMetadata &metadata) {
    ESP_LOGD(TAG, "Metadata callback: type=%u", metadata.type());
    if (metadata.type() == (uint8_t)this->metadata_type_ ) {
      this->publish_if_changed_((const char*)metadata.text());
    }
  });
}
#endif

#ifdef USE_A2DP_PEER_NAME

/**
 * @brief Initialize the peer text sensor.
 *
 * Disables the loop and subscribes to peer name callbacks. When the peer
 * name is resolved, publishes either the device name or the Bluetooth
 * MAC address (formatted as a string) depending on the configured peer type.
 */
void A2DPSinkPeerTextSensor::setup() {
  this->disable_loop();

  this->parent_->add_peer_name_callback([this](const char *name) {
    if (this->parent_->get_connection_state() == ESP_A2D_CONNECTION_STATE_CONNECTED) {
      if (this->peer_type_ == A2DPSinkPeerRequestTypes::PEERNAME) {
        ESP_LOGD(TAG, "Peer name resolved: %s", name);
        this->publish_if_changed_(name);
      } else if (this->peer_type_ == A2DPSinkPeerRequestTypes::PEERADDR) {
        auto* peer_addr = this->parent_->get_current_peer_address();
        this->publish_if_changed_(A2DPSinkHub::bd_addr_to_string(*peer_addr).c_str());
      } else {
        ESP_LOGW(TAG, "Unknown peer request type, ignoring");
        return;
      }
    } else {
      this->publish_if_changed_("");
    }

  });
}
#endif
}  // namespace esphome::a2dp_sink

