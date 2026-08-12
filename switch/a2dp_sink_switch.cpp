#include "a2dp_sink_switch.h"

namespace esphome::a2dp_sink {

static const char *const TAG = "a2dp_sink.switch";

#ifdef USE_A2DP_CONNECTION_STATE
// ---------------------------------------------------------------------------
// A2DPSwitchConnection
// ---------------------------------------------------------------------------

/**
 * @brief Log the connection switch configuration.
 */
void A2DPSwitchConnection::dump_config() {
  LOG_SWITCH("", "A2DPSink Connection Switch", this);
}

/**
 * @brief Initialize the connection switch.
 *
 * Disables the loop and subscribes to connection state callbacks from the hub.
 * When the connection state changes, the switch publishes the new state.
 */
void A2DPSwitchConnection::setup() {
  this->disable_loop();
  this->parent_->add_connection_state_callbacks([this](esp_a2d_connection_state_t state, void *user_data) {
    bool is_connected = (state == ESP_A2D_CONNECTION_STATE_CONNECTED);
    this->publish_state_if_changed_(is_connected);
  });
}

/**
 * @brief Write a new state to the connection switch.
 * @param state True to connect, false to disconnect.
 */
void A2DPSwitchConnection::write_state(bool state) {
  if (state != this->state) {
    ESP_LOGI(TAG, "A2DP connection switch state changed: %s", state ? "ON" : "OFF");
    this->parent_->set_connected(state);
  }    
}

/**
 * @brief Publish the connection state only if it has changed.
 * @param state The new connection state.
 */
void A2DPSwitchConnection::publish_state_if_changed_(bool state) {
  if (this->state != state) {
    this->publish_state(state);
  }
}
#endif
// ---------------------------------------------------------------------------
// A2DPSwitchBluetooth
// ---------------------------------------------------------------------------

/**
 * @brief Log the Bluetooth switch configuration.
 */
void A2DPSwitchBluetooth::dump_config() {
  LOG_SWITCH("", "A2DPSink Bluetooth Switch", this);
}

/**
 * @brief Initialize the Bluetooth switch.
 *
 * Disables the loop and checks the restore mode to determine the initial state.
 * If the initial state is ON, starts the A2DP sink; otherwise, does nothing.
 */
void A2DPSwitchBluetooth::setup() {
  this->disable_loop();
  bool initial_state = this->get_initial_state_with_restore_mode().value_or(false);

  // write state before setup
  if (initial_state) {
    this->turn_on();
  } else {
    this->turn_off();
  }
}

/**
 * @brief Write a new state to the Bluetooth switch.
 * @param state True to start the A2DP sink, false to stop it.
 */
void A2DPSwitchBluetooth::write_state(bool state) {
  if (state) {
    ESP_LOGI(TAG, "Bluetooth ON: starting A2DP Sink");
    this->parent_->start();
  } else {
    ESP_LOGI(TAG, "Bluetooth OFF: stopping A2DP Sink");
    this->parent_->stop();
  }
  this->publish_state_if_changed_(state);
}

/**
 * @brief Publish the Bluetooth state only if it has changed.
 * @param state The new Bluetooth state.
 */
void A2DPSwitchBluetooth::publish_state_if_changed_(bool state) {
  if (this->state != state) {
    this->publish_state(state);
  }
}

}  // namespace esphome::a2dp_sink