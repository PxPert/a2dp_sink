#pragma once

#include "../a2dp_sink_hub.h"
#include "esphome/components/switch/switch.h"

namespace esphome::a2dp_sink {

#ifdef USE_A2DP_CONNECTION_STATE
/**
 * @brief Switch that reflects the A2DP client connection state.
 *
 * The state is read-only: when a client connects or disconnects,
 * the hub invokes the callback and the switch updates its state.
 */
class A2DPSwitchConnection : public A2DPSinkChild, public switch_::Switch {
 public:
  /**
   * @brief Log the connection switch configuration for debugging.
   */
  void dump_config() override;

  /**
   * @brief Initialize the connection switch and subscribe to hub callbacks.
   */
  void setup() override;

  /**
   * @brief Write a new state to the switch.
   * @param state True to connect, false to disconnect.
   */
  void write_state(bool state) override;

 protected:
  /**
   * @brief Publish the connection state only if it has changed.
   * @param state The new connection state.
   */
  void publish_state_if_changed_(bool state);
};
#endif

/**
 * @brief Switch that turns the Bluetooth interface on/off.
 *
 * The state is fully user-controlled: ON calls a2dp_sink_->start(),
 * OFF calls a2dp_sink_->stop().
 */
class A2DPSwitchBluetooth : public A2DPSinkChild, public switch_::Switch {
 public:
  /**
   * @brief Log the Bluetooth switch configuration for debugging.
   */
  void dump_config() override;

  /**
   * @brief Initialize the Bluetooth switch with restore mode support.
   */
  void setup() override;

  /**
   * @brief Write a new state to the Bluetooth switch.
   * @param state True to start the A2DP sink, false to stop it.
   */
  void write_state(bool state) override;

  /**
   * @brief Get the ESPHome setup priority for this component.
   * @return float Setup priority (runs after hub and child setup).
   */
  float get_setup_priority() const override { return a2dp_sink_priority::CHILD - 1.0f; }

 protected:
  /**
   * @brief Publish the Bluetooth state only if it has changed.
   * @param state The new Bluetooth state.
   */
  void publish_state_if_changed_(bool state);
};

}  // namespace esphome::a2dp_sink