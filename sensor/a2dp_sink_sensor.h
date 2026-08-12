#pragma once

#include "esphome/core/defines.h"

#include "../a2dp_sink_hub.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome::a2dp_sink {

/**
 * @brief Base class for all A2DP numeric sensors.
 *
 * Provides deduplication via publish_if_changed_ to avoid unnecessary
 * frontend updates when the value hasn't actually changed.
 */
class A2DPSinkNumericSensor : public A2DPSinkChild, public sensor::Sensor {
 public:
  /**
   * @brief Log the sensor configuration for debugging.
   */
  void dump_config() override;

  /**
   * @brief Initialize the sensor (no-op base implementation).
   */
  void setup() override {};

 protected:
  /**
   * @brief Publish a new sensor value only if it differs from the current state.
   * @param value The new sensor value.
   */
  void publish_if_changed_(float value);

};

#ifdef USE_A2DP_METADATA

/**
 * @brief Sensor for numeric AVRC metadata attributes (track number, playing time, total tracks).
 *
 * Subscribes to metadata updates from the hub and filters by the configured
 * metadata type, parsing the string value to a float.
 */
class A2DPMetadataNumericSensor : public A2DPSinkNumericSensor {
 public:
  /**
   * @brief Initialize the metadata sensor and subscribe to hub callbacks.
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

#ifdef USE_A2DP_POS

/**
 * @brief Sensor for the current A2DP playback position.
 *
 * Subscribes to playback position callbacks from the hub and publishes
 * the position in milliseconds.
 */
class A2DPSinkTrackPositionSensor : public A2DPSinkNumericSensor
{
 public:
  /**
   * @brief Initialize the track position sensor and subscribe to hub callbacks.
   */
  void setup() override;
};

#endif

#ifdef USE_A2DP_RSSI

/**
 * @brief Sensor for the A2DP RSSI signal strength delta.
 *
 * Subscribes to RSSI delta callbacks from the hub and publishes the
 * signal strength value in dBm.
 */
class A2DPSinkRssiSensor : public A2DPSinkNumericSensor
{
 public:
  /**
   * @brief Initialize the RSSI sensor and subscribe to hub callbacks.
   */
  void setup() override;
};

#endif

#ifdef USE_A2DP_SAMPLE_RATE

/**
 * @brief Sensor for the current A2DP audio sample rate.
 *
 * Subscribes to sample rate change callbacks from the hub and publishes
 * the sample rate in Hz.
 */
class A2DPSinkSampleRateSensor : public A2DPSinkNumericSensor
{
 public:
  /**
   * @brief Initialize the sample rate sensor and subscribe to hub callbacks.
   */
  void setup() override;
};

/**
 * @brief Sensor for the number of audio channels.
 *
 * Subscribes to sample rate change callbacks (which also trigger on channel
 * changes) and publishes the current channel count.
 */
class A2DPSinkChannelsSensor : public A2DPSinkNumericSensor
{
 public:
  /**
   * @brief Initialize the channels sensor and subscribe to hub callbacks.
   */
  void setup() override;
};

#endif

}  // namespace esphome::a2dp_sink