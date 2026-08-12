#include "esphome/core/log.h"
#include "a2dp_sink_hub.h"
#include "BluetoothA2DPSink.h"

namespace esphome
{
    namespace a2dp_sink
    {
        static const char *TAG = "a2dp_sink_hub";

        /** @brief Global singleton pointer to the active A2DP sink hub instance. */
        static A2DPSinkHub *g_a2dp_hub_instance = nullptr;

        /** @brief Global BluetoothA2DPSink instance managed by the hub. */
        static BluetoothA2DPSink a2dp_sink_;

  #ifdef USE_A2DP_METADATA
        /**
         * @brief Handle AVRC metadata updates from the remote device.
         * @param id The metadata attribute ID.
         * @param text Pointer to the metadata text value.
         */
        void A2DPSinkHub::avrc_metadata_callback(uint8_t id, const uint8_t *text) {
            ESP_LOGD(TAG, "AVRC metadata: attribute 0x%02x = %s", id, text);

            A2DPSinkMetadata m(id, text );
            this->metadata_update_callbacks_.call(m);

        }
#endif

#ifdef USE_A2DP_PLAYBACK_STATUS
        /**
         * @brief Handle AVRC playback status changes.
         * @param playback The new playback status.
         */
        void A2DPSinkHub::avrc_rn_playstatus_callback(esp_avrc_playback_stat_t playback) {
            ESP_LOGD(TAG, "AVRC playback status: %d", (uint8_t)playback);
            this->playback_status_callbacks_.call(playback);
        }
#endif

#ifdef USE_A2DP_POS
        /**
         * @brief Handle playback position updates.
         * @param play_pos Current playback position in milliseconds.
         */
        void A2DPSinkHub::avrc_rn_play_pos_callback(uint32_t play_pos) {
            ESP_LOGD(TAG, "AVRC playback position: %u ms", play_pos);
            this->playback_position_callbacks_.call(play_pos);
        }
#endif

#ifdef USE_A2DP_VOLUME
        /**
         * @brief Handle volume changes from the remote device.
         * @param volume The new volume level.
         */
        void A2DPSinkHub::avrc_rn_volumechange_callback(int volume) {
            ESP_LOGD(TAG, "AVRC volume changed: %d", volume);
            this->volume_change_callbacks_.call(volume);
        }
#endif

#ifdef USE_A2DP_AUDIO_STATE
        /**
         * @brief Handle A2DP audio state changes.
         * @param state The new audio state.
         */
        void A2DPSinkHub::audio_state_callback(esp_a2d_audio_state_t state) {
            ESP_LOGD(TAG, "A2DP audio state: %d", (uint8_t)state);
            this->audio_state_callbacks_.call(state);
        }
#endif

#ifdef USE_A2DP_SAMPLE_RATE
        /**
         * @brief Handle sample rate changes.
         * @param sample_rate The new sample rate in Hz.
         */
        void A2DPSinkHub::sample_rate_callback(uint16_t sample_rate) {
            ESP_LOGD(TAG, "A2DP sample rate: %u Hz", sample_rate);
            this->sample_rate_callbacks_.call(sample_rate);
        }
#endif

#ifdef USE_A2DP_AVRCP_CONNECTION_STATE
        /**
         * @brief Handle AVRCP connection state changes.
         * @param connected True if AVRCP is connected.
         */
        void A2DPSinkHub::avrc_connection_state_callback(bool connected) {
            ESP_LOGD(TAG, "AVRCP connection: %s", connected ? "connected" : "disconnected");
            this->avrc_connection_state_callbacks_.call(connected);
        }
#endif

#ifdef USE_A2DP_CONNECTION_STATE
        /**
         * @brief Handle A2DP connection state changes.
         * @param state The new connection state.
         * @param user_data User-provided context pointer.
         */
        void A2DPSinkHub::on_connection_state_changed(esp_a2d_connection_state_t state, void *user_data) {
            ESP_LOGI(TAG, "A2DP connection state: %d", (uint8_t)state);
            this->connection_state_callbacks_.call(state, user_data);
        }
#endif

#ifdef USE_A2DP_PEER_NAME
        /**
         * @brief Handle peer device name resolution.
         * @param name The resolved peer device name.
         */
        void A2DPSinkHub::peer_name_callback(const char* name) {
            this->peer_name_callbacks_.call(name);
        }
#endif

#ifdef USE_A2DP_RSSI
        /**
         * @brief Handle RSSI signal strength delta updates.
         * @param rssi RSSI delta parameter structure.
         */
        void A2DPSinkHub::rssi_callback(esp_bt_gap_cb_param_t::read_rssi_delta_param& rssi) {
            ESP_LOGD(TAG, "RSSI delta: %d dBm", rssi.rssi_delta);
            this->rssi_callbacks_.call(rssi);
        }
#endif

        /**
         * @brief Initialize the A2DP Sink hub.
         *
         * Registers all conditional callbacks (metadata, playback status, position,
         * connection state, peer name, RSSI, volume, audio state, AVRCP connection,
         * sample rate) with the BluetoothA2DPSink instance and disables the main loop.
         */
        void A2DPSinkHub::setup()
        {
            ESP_LOGI(TAG, "A2DP Sink initializing");

            g_a2dp_hub_instance = this;

            this->disable_loop();

#ifdef USE_A2DP_METADATA
            /*
             * Metadata subscription and callbacks
             */
            if (this->metadata_mask_ > 0) {
                a2dp_sink_.set_avrc_metadata_attribute_mask(this->metadata_mask_);
            }
            a2dp_sink_.set_avrc_metadata_callback(
                [](uint8_t id, const uint8_t *text) {
                    if (g_a2dp_hub_instance != nullptr) {
                        g_a2dp_hub_instance->avrc_metadata_callback(id, text);
                    }
                }
            );
#endif

#ifdef USE_A2DP_PLAYBACK_STATUS
            /*
             * Playback status callback
             */
            a2dp_sink_.set_avrc_rn_playstatus_callback(
                [](esp_avrc_playback_stat_t playback) {
                    if (g_a2dp_hub_instance != nullptr) {
                        g_a2dp_hub_instance->avrc_rn_playstatus_callback(playback);
                    }
                }
            );
#endif

#ifdef USE_A2DP_POS
            /*
             * Playback position callback
             */
            a2dp_sink_.set_avrc_rn_play_pos_callback(
                [](uint32_t play_pos) {
                    if (g_a2dp_hub_instance != nullptr) {
                        g_a2dp_hub_instance->avrc_rn_play_pos_callback(play_pos);
                    }
                }
            );
#endif

#ifdef USE_A2DP_CONNECTION_STATE
            /* 
             * Connection state callback
             */
            a2dp_sink_.set_on_connection_state_changed(
                [](esp_a2d_connection_state_t state, void *user_data) {
                    if (g_a2dp_hub_instance != nullptr) {
                        g_a2dp_hub_instance->on_connection_state_changed(state, user_data);
                    }
                }
            );
#endif

#ifdef USE_A2DP_PEER_NAME
            a2dp_sink_.set_peer_name_callback(
                [](char* name) {
                    if (g_a2dp_hub_instance != nullptr) {
                        g_a2dp_hub_instance->peer_name_callback(name);
                    }
                }
            );
#endif

#ifdef USE_A2DP_RSSI
            /*
             * RSSI callback
             */
            a2dp_sink_.set_rssi_active(true);
            a2dp_sink_.set_rssi_callback(
                [](esp_bt_gap_cb_param_t::read_rssi_delta_param& rssi) {
                    if (g_a2dp_hub_instance != nullptr) {
                        g_a2dp_hub_instance->rssi_callback(rssi);
                    }
                }
            );
#endif

#ifdef USE_A2DP_VOLUME
            /*
             * Volume change callback
             */
            a2dp_sink_.set_avrc_rn_volumechange(
                [](int volume) {
                    if (g_a2dp_hub_instance != nullptr) {
                        g_a2dp_hub_instance->avrc_rn_volumechange_callback(volume);
                    }
                }
            );

            // Same callback to nofity when the local volume change has been completed (after the remote device has acknowledged it)
            a2dp_sink_.set_avrc_rn_volumechange_completed(
                [](int volume) {
                    if (g_a2dp_hub_instance != nullptr) {
                        g_a2dp_hub_instance->avrc_rn_volumechange_callback(volume);
                    }
                }
            );
#endif

#ifdef USE_A2DP_AUDIO_STATE
            /*
             * Audio state callback
             */
            a2dp_sink_.set_on_audio_state_changed(
                [](esp_a2d_audio_state_t state, void *) {
                    if (g_a2dp_hub_instance != nullptr) {
                        g_a2dp_hub_instance->audio_state_callback(state);
                    }
                }
            );
#endif

#ifdef USE_A2DP_AVRCP_CONNECTION_STATE
            /*
             * AVRCP connection state callback
             */
            a2dp_sink_.set_avrc_connection_state_callback(
                [](bool connected) {
                    if (g_a2dp_hub_instance != nullptr) {
                        g_a2dp_hub_instance->avrc_connection_state_callback(connected);
                    }
                }
            );
#endif

#ifdef USE_A2DP_SAMPLE_RATE
            /*
             * Sample rate change callback
             */
            a2dp_sink_.set_sample_rate_callback(
                [](uint16_t sample_rate) {
                    if (g_a2dp_hub_instance != nullptr) {
                        g_a2dp_hub_instance->sample_rate_callback(sample_rate);
                    }
                }
            );
#endif

            ESP_LOGI(TAG, "A2DP Sink initialized");
        }

        /**
         * @brief Start the A2DP Sink and make it discoverable.
         * @param name The friendly name for the sink device.
         * @param auto_reconnect Whether to auto-reconnect to the last paired device.
         */
        void A2DPSinkHub::start() {
            ESP_LOGI(TAG, "A2DP Sink starting (name=%s, auto_reconnect=%d)", this->name().c_str(), this->auto_reconnect());
            a2dp_sink_.start(this->name().c_str(), this->auto_reconnect());
        }

        /**
         * @brief Stop the A2DP Sink and clean up Bluetooth resources.
         */
        void A2DPSinkHub::stop() {
            ESP_LOGI(TAG, "A2DP Sink stopping");
            a2dp_sink_.end();
        }

        /**
         * @brief Main loop for the A2DP Sink hub.
         *
         * Currently a no-op since the hub is event-driven via callbacks.
         */
        void A2DPSinkHub::loop()
        {
        }

        /**
         * @brief Get a pointer to the underlying BluetoothA2DPSink instance.
         * @return BluetoothA2DPSink* Pointer to the A2DP sink object.
         */
        BluetoothA2DPSink* A2DPSinkHub::a2dp_sink() const {
            return &a2dp_sink_;
        }

        /**
         * @brief Set the volume of the A2DP sink.
         * @param volume Volume level in the range 0-127.
         */
        void A2DPSinkHub::set_volume(uint8_t volume) {
            ESP_LOGD(TAG, "Setting A2DP volume to %u", volume);
            a2dp_sink_.set_volume(volume);
        }

        /**
         * @brief Set the stream reader callback for A2DP audio data.
         * @param callBack Function pointer taking (const uint8_t* data, uint32_t size).
         */
        void A2DPSinkHub::set_stream_reader(void (*callBack)(const uint8_t*, uint32_t)) {
            a2dp_sink_.set_stream_reader(callBack, false);
        }

        /**
         * @brief Stop the A2DP stream without disconnecting the Bluetooth link.
         */
        void A2DPSinkHub::stop_a2dp() {
            a2dp_sink_.stop();
        }

        /**
         * @brief Pause the currently playing A2DP stream.
         */
        void A2DPSinkHub::pause_a2dp() {
            a2dp_sink_.pause();
        }

        /**
         * @brief Resume playing the A2DP stream after a pause.
         */
        void A2DPSinkHub::play_a2dp() {
            a2dp_sink_.play();
        }

        /**
         * @brief Skip to the next track via AVRCP.
         */
        void A2DPSinkHub::next_track() {
            a2dp_sink_.next();
        }

        /**
         * @brief Skip to the previous track via AVRCP.
         */
        void A2DPSinkHub::prev_track() {
            a2dp_sink_.previous();
        }

        /**
         * @brief Get the current A2DP connection state.
         * @return esp_a2d_connection_state_t The connection state enum value.
         */
        esp_a2d_connection_state_t A2DPSinkHub::get_connection_state() {
            return a2dp_sink_.get_connection_state();
        }

        /**
         * @brief Set the connection state (connect/disconnect).
         * @param connected True to connect, false to disconnect.
         */
        void A2DPSinkHub::set_connected(bool connected) {
            a2dp_sink_.set_connected(connected);
        }

        /**
         * @brief Get the peer device name.
         * @return const char* The peer device name string.
         */
        const char* A2DPSinkHub::get_peer_name() {
            return a2dp_sink_.get_peer_name();
        }

        /**
         * @brief Get the current peer Bluetooth address.
         * @return esp_bd_addr_t* Pointer to the 6-byte Bluetooth address.
         */
        esp_bd_addr_t* A2DPSinkHub::get_current_peer_address() {
            return a2dp_sink_.get_current_peer_address();
        }

        /**
         * @brief Log the A2DP Sink configuration for debugging.
         */
        void A2DPSinkHub::dump_config()
        {
            ESP_LOGCONFIG(TAG, "A2DP Sink");
        }

        /**
         * @brief Convert an esp_bd_addr_t (6-byte Bluetooth address) to a human-readable
         *        string like "AA:BB:CC:DD:EE:FF".
         * @param addr The Bluetooth address to convert.
         * @return std::string The formatted address string.
         */
        std::string A2DPSinkHub::bd_addr_to_string(const esp_bd_addr_t& addr) {
            char buf[18]; // "XX:XX:XX:XX:XX:XX\0" = 17 chars max
            snprintf(buf, sizeof(buf), ESP_BD_ADDR_STR, ESP_BD_ADDR_HEX(addr));
            return std::string(buf);
        }



    } // namespace a2dp_sink
} // namespace esphome
