#include "a2dp_sink_sensor.h"

#include <string>

namespace esphome::a2dp_sink {

static const char *const TAG = "a2dp_sink.sensor";

/**
 * @brief Log the numeric sensor configuration.
 */
void A2DPSinkNumericSensor::dump_config() { LOG_SENSOR("", "A2DPSink Numeric", this); }


// Dedup to avoid frontend churn; Sensor::publish_state already dedups the float assign but still notifies.
/**
 * @brief Publish a new sensor value only if it differs from the current state.
 * @param value The new sensor value.
 */
void A2DPSinkNumericSensor::publish_if_changed_(float value) {
  if (this->get_raw_state() != value) {
    this->publish_state(value);
  }
}

#ifdef USE_A2DP_METADATA

/**
 * @brief Initialize the metadata numeric sensor.
 *
 * Disables the loop, requests the specific metadata attribute from the parent hub,
 * and subscribes to metadata update callbacks. Filters by metadata type and parses
 * the string value to a float before publishing.
 */
void A2DPMetadataNumericSensor::setup() {
  this->disable_loop();

  // Request this specific metadata attribute from the parent hub
  this->parent_->add_metadata_attribute((uint8_t)this->metadata_type_);
  
  this->parent_->add_metadata_update_callback([this](const A2DPSinkMetadata &metadata) {
    if (metadata.type() == (uint8_t)this->metadata_type_) {
      // Metadata values come as strings, parse them to float
      float value = 0.0f;
      if (sscanf((const char*)metadata.text(), "%f", &value) == 1) {
        this->publish_if_changed_(value);
      }
    }
  });
}
#endif

#ifdef USE_A2DP_POS

/**
 * @brief Initialize the track position sensor.
 *
 * Disables the loop and subscribes to playback position callbacks,
 * publishing the position value as a float.
 */
void A2DPSinkTrackPositionSensor::setup() {
  this->disable_loop();

  this->parent_->add_playback_position_callbacks([this](uint32_t pos) {
    this->publish_if_changed_((float)pos);
  });
}

#endif

#ifdef USE_A2DP_RSSI

/**
 * @brief Initialize the RSSI sensor.
 *
 * Disables the loop and subscribes to RSSI delta callbacks,
 * publishing the RSSI delta value as a float.
 */
void A2DPSinkRssiSensor::setup() {
  this->disable_loop();

  this->parent_->add_rssi_callback([this](esp_bt_gap_cb_param_t::read_rssi_delta_param& rssi) {
    this->publish_if_changed_((float)rssi.rssi_delta);
  });
}

#endif

#ifdef USE_A2DP_SAMPLE_RATE

/**
 * @brief Initialize the sample rate sensor.
 *
 * Disables the loop and subscribes to sample rate callbacks,
 * publishing the sample rate value as a float.
 */
void A2DPSinkSampleRateSensor::setup() {
  this->disable_loop();

  this->parent_->add_sample_rate_callback([this](uint16_t sample_rate) {
    this->publish_if_changed_((float)sample_rate);
  });
}

    /**
     * @brief Initialize the channels sensor.
     *
     * Disables the loop and subscribes to sample rate callbacks (which also
     * trigger on channel changes), publishing the channel count as a float.
     */
    void A2DPSinkChannelsSensor::setup() {
  this->disable_loop();

  this->parent_->add_sample_rate_callback([this](uint16_t /*sample_rate*/) {
    this->publish_if_changed_((float)this->parent_->a2dp_sink()->channels());
  });
}

#endif

}  // namespace esphome::a2dp_sink