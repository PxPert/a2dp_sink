#include "a2dp_sink_volume.h"

namespace esphome::a2dp_sink {

static const char *const TAG = "a2dp_sink.number";

/**
 * @brief Initialize the volume number control.
 *
 * Disables the loop and subscribes to volume change callbacks from the hub,
 * publishing the current volume as a float state.
 */
void A2DPSinkVolumeNumber::setup() {
  // Subscribe to volume change callbacks from the hub
  this->disable_loop();

  this->parent_->add_volume_change_callback([this](int volume) {
    this->publish_state(static_cast<float>(volume));
  });
}

/**
 * @brief Log the volume number configuration.
 */
void A2DPSinkVolumeNumber::dump_config() { 
  LOG_NUMBER("", "A2DPSink Volume", this); 
}

/**
 * @brief Handle user input to set the volume.
 * @param value The new volume value (0-127, already validated by ESPHome).
 */
void A2DPSinkVolumeNumber::control(float value) {
  // The value is already validated by ESPHome (0-127 range)
  this->parent_->set_volume(static_cast<uint8_t>(value));
}


}  // namespace esphome::a2dp_sink