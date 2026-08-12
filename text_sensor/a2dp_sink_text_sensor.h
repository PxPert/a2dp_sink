#pragma once

#include "esphome/core/defines.h"


#include "../a2dp_sink_hub.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome::a2dp_sink {

/**
 * @brief Base class for all A2DP text sensors.
 *
 * Provides deduplication via publish_if_changed_ to avoid unnecessary
 * frontend updates when the text value hasn't changed.
 */
class A2DPSinkTextSensor : public A2DPSinkChild, public text_sensor::TextSensor {
 public:
  /**
   * @brief Log the text sensor configuration for debugging.
   */
  void dump_config() override;

  /**
   * @brief Initialize the text sensor (no-op base implementation).
   */
  void setup() override {};

 protected:
  /**
   * @brief Publish a new text value only if it differs from the current state.
   * @param value The new text value.
   */
  void publish_if_changed_(const char *value);

};

#ifdef USE_A2DP_METADATA

/**
 * @brief Text sensor for AVRC metadata attributes (title, artist, album, genre).
 *
 * Subscribes to metadata updates from the hub and filters by the configured
 * metadata type, publishing the matching text value.
 */
class A2DPMetadataTextSensor : public A2DPSinkTextSensor {
 public:
  /**
   * @brief Initialize the metadata text sensor and subscribe to hub callbacks.
   */
  void setup() override;

  /**
   * @brief Set the metadata attribute type to filter on.
   * @param metadata_type The A2DPSinkMetadataTypes enum value.
   */
  void set_metadata_type(A2DPSinkMetadataTypes metadata_type) { this->metadata_type_ = metadata_type; }
 protected:
   A2DPSinkMetadataTypes metadata_type_;

};
#endif

#ifdef USE_A2DP_PEER_NAME
/**
 * @brief Text sensor for peer device information.
 *
 * Subscribes to peer name callbacks from the hub and publishes either
 * the peer device name or the Bluetooth MAC address, depending on the
 * configured peer type.
 */
class A2DPSinkPeerTextSensor : public A2DPSinkTextSensor
{
 public:
  /**
   * @brief Initialize the peer text sensor and subscribe to hub callbacks.
   */
  void setup() override;

  /**
   * @brief Set the peer request type (name or address).
   * @param peer_type The A2DPSinkPeerRequestTypes enum value.
   */
  void set_peer_type(A2DPSinkPeerRequestTypes peer_type) { this->peer_type_ = peer_type; }
 protected:
  A2DPSinkPeerRequestTypes peer_type_;
};
#endif


}  // namespace esphome::a2dp_sink

