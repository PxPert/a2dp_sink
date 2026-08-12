#pragma once

#include "../a2dp_sink_hub.h"
#include "esphome/components/number/number.h"

namespace esphome::a2dp_sink {

/**
 * @brief Number control for the A2DP sink volume.
 *
 * Subscribes to volume change callbacks from the hub and forwards user
 * input to the A2DP sink volume control. Accepts values in the range 0-127.
 */
class A2DPSinkVolumeNumber : public A2DPSinkChild, public number::Number {
 public:
  /**
   * @brief Initialize the volume number control and subscribe to hub callbacks.
   */
  void setup() override;

  /**
   * @brief Log the volume number configuration for debugging.
   */
  void dump_config() override;


 protected:
  /**
   * @brief Handle user input to set the volume.
   * @param value The new volume value (0-127).
   */
  void control(float value) override;

};

}  // namespace esphome::a2dp_sink