#pragma once

#include "esphome/core/component.h"
#include "BluetoothA2DPSink.h"
#include <atomic>
#include <functional>


namespace esphome
{
    namespace a2dp_sink
    {

        namespace a2dp_sink_priority {
        // AFTER_WIFI so the hub runs after the wifi/ethernet drivers are up and we can read the active
        // interface's MAC for client_id.
        inline constexpr float HUB = esphome::setup_priority::AFTER_WIFI;
        inline constexpr float CHILD = HUB - 1.0f;
        }  // namespace sendspin_priority

#ifdef USE_A2DP_METADATA
        /**
         * @brief A2DP metadata attribute types for AVRC (Advanced Audio/Video Remote Control Protocol).
         *
         * Maps to ESP_AVRC_MD_ATTR_* constants for media player metadata
         * like title, artist, album, genre, track number, playing time, and total tracks.
         */
        enum class A2DPSinkMetadataTypes {
            TITLE = ESP_AVRC_MD_ATTR_TITLE,
            ARTIST = ESP_AVRC_MD_ATTR_ARTIST,
            ALBUM = ESP_AVRC_MD_ATTR_ALBUM,
            GENRE = ESP_AVRC_MD_ATTR_GENRE,
            TRACKNUM = ESP_AVRC_MD_ATTR_TRACK_NUM,
            PLAYINGTIME = ESP_AVRC_MD_ATTR_PLAYING_TIME,
            NUM_TRACKS = ESP_AVRC_MD_ATTR_NUM_TRACKS,
        };
#endif

#ifdef USE_A2DP_PEER_NAME
        /**
         * @brief Request types for peer device information.
         *
         * PEERNAME retrieves the human-readable device name,
         * PEERADDR retrieves the Bluetooth MAC address.
         */
        enum class A2DPSinkPeerRequestTypes {
            PEERNAME,
            PEERADDR,
        };
#endif 

        /**
         * @brief Holds a single A2DP metadata attribute value.
         *
         * Wraps a metadata type (e.g. TITLE, ARTIST) with its string value
         * received from the remote AVRCP peer.
         */
        class A2DPSinkMetadata
        {
        public:
            /**
             * @brief Construct a new A2DPSinkMetadata object.
             * @param type The metadata attribute type.
             * @param text Pointer to the metadata text value.
             */
            A2DPSinkMetadata(uint8_t type, const uint8_t *text) {
                type_ = type;
                text_ = text;
            }

            /**
             * @brief Get the metadata attribute type.
             * @return uint8_t The metadata type.
             */
            uint8_t type() const { return type_; }

            /**
             * @brief Get the metadata text value.
             * @return const uint8_t* Pointer to the metadata text.
             */
            const uint8_t *text() const { return text_; }

        private:
            uint8_t type_;
            const uint8_t *text_;
        };

        /**
         * @brief Central hub for A2DP Bluetooth audio sink functionality.
         *
         * Manages the Bluetooth A2DP sink lifecycle, including connection state,
         * playback control, volume, metadata, RSSI, and various AVRCP callbacks.
         * Child components subscribe to events via CallbackManager instances.
         */
        class A2DPSinkHub : public Component
        {
        public:
            /**
             * @brief Get the ESPHome setup priority for this component.
             * @return float Setup priority (runs after WiFi/ethernet).
             */
            float get_setup_priority() const override { return a2dp_sink_priority::HUB; }

            /**
             * @brief Initialize the A2DP sink hub and register all callbacks.
             */
            void setup() override;

            /**
             * @brief Main loop (currently disabled; hub is event-driven).
             */
            void loop() override;

            /**
             * @brief Log the A2DP Sink configuration for debugging.
             */
            void dump_config() override;

            /**
             * @brief Set the friendly name for the A2DP sink device.
             * @param v The device name string.
             */
            void set_name(std::string &&v) { this->name_ = std::move(v); }

            /**
             * @brief Get the A2DP sink device name.
             * @return std::string& Reference to the device name.
             */
            std::string& name() { return this->name_; };

            /**
             * @brief Enable or disable automatic reconnection to the last paired device.
             * @param v True to enable auto-reconnect, false to disable.
             */
            void set_auto_reconnect(bool v) { this->auto_reconnect_ = v; }

            /**
             * @brief Check if auto-reconnect is enabled.
             * @return bool True if auto-reconnect is enabled.
             */
            bool auto_reconnect() { return this->auto_reconnect_; };

#ifdef USE_A2DP_CONNECTION_STATE
            /**
             * @brief Register a callback for A2DP connection state changes.
             * @param callback Function taking (esp_a2d_connection_state_t state, void* user_data).
             */
            template<typename F> void add_connection_state_callbacks(F &&callback) {
                this->connection_state_callbacks_.add(std::forward<F>(callback));
            }
#endif

#ifdef USE_A2DP_PLAYBACK_STATUS
            /**
             * @brief Register a callback for AVRC playback status changes.
             * @param callback Function taking (esp_avrc_playback_stat_t playback).
             */
            template<typename F> void add_playback_status_callbacks(F &&callback) {
                this->playback_status_callbacks_.add(std::forward<F>(callback));
            }
#endif

#ifdef USE_A2DP_POS
            /**
             * @brief Register a callback for playback position updates.
             * @param callback Function taking (uint32_t pos) in milliseconds.
             */
            template<typename F> void add_playback_position_callbacks(F &&callback) {
                this->playback_position_callbacks_.add(std::forward<F>(callback));
            }
#endif

#ifdef USE_A2DP_RSSI
            /**
             * @brief Register a callback for RSSI signal strength delta updates.
             * @param callback Function taking (esp_bt_gap_cb_param_t::read_rssi_delta_param& rssi).
             */
            template<typename F> void add_rssi_callback(F &&callback) {
                this->rssi_callbacks_.add(std::forward<F>(callback));
            }
#endif

#ifdef USE_A2DP_METADATA
            /**
             * @brief Register a callback for AVRC metadata updates.
             * @param callback Function taking (const A2DPSinkMetadata& metadata).
             */
            template<typename F> void add_metadata_update_callback(F &&callback) {
                this->metadata_update_callbacks_.add(std::forward<F>(callback));
            }

            /**
             * @brief Subscribe to a specific metadata attribute from the remote device.
             * @param attr The AVRC metadata attribute bitmask to request.
             */
            void add_metadata_attribute(uint8_t attr) { this->metadata_mask_ |= attr; }

#endif

#ifdef USE_A2DP_PEER_NAME
            /**
             * @brief Register a callback for peer device name resolution.
             * @param callback Function taking (const char* name).
             */
            template<typename F> void add_peer_name_callback(F &&callback) {
                this->peer_name_callbacks_.add(std::forward<F>(callback));
            }
#endif

#ifdef USE_A2DP_VOLUME
            /**
             * @brief Register a callback for volume changes.
             * @param callback Function taking (int volume) in the range 0-127.
             */
            template<typename F> void add_volume_change_callback(F &&callback) {
                this->volume_change_callbacks_.add(std::forward<F>(callback));
            }
#endif

#ifdef USE_A2DP_AUDIO_STATE
            /**
             * @brief Register a callback for A2DP audio state changes.
             * @param callback Function taking (esp_a2d_audio_state_t state).
             */
            template<typename F> void add_audio_state_callback(F &&callback) {
                this->audio_state_callbacks_.add(std::forward<F>(callback));
            }
#endif

#ifdef USE_A2DP_AVRCP_CONNECTION_STATE
            /**
             * @brief Register a callback for AVRCP connection state changes.
             * @param callback Function taking (bool connected).
             */
            template<typename F> void add_avrc_connection_state_callback(F &&callback) {
                this->avrc_connection_state_callbacks_.add(std::forward<F>(callback));
            }
#endif

#ifdef USE_A2DP_SAMPLE_RATE
            /**
             * @brief Register a callback for sample rate changes.
             * @param callback Function taking (uint16_t sample_rate) in Hz.
             */
            template<typename F> void add_sample_rate_callback(F &&callback) {
                this->sample_rate_callbacks_.add(std::forward<F>(callback));
            }
#endif

            /**
             * @brief Get a pointer to the underlying BluetoothA2DPSink instance.
             * @return BluetoothA2DPSink* Pointer to the A2DP sink object.
             */
            BluetoothA2DPSink* a2dp_sink() const;

            /**
             * @brief Start the A2DP sink and make it discoverable.
             */
            void start();

            /**
             * @brief Stop the A2DP sink and disconnect from any paired device.
             */
            void stop();

            /**
             * @brief Set the volume of the A2DP sink.
             * @param volume Volume level in the range 0-127.
             */
            void set_volume(uint8_t volume);

            /**
             * @brief Set the stream reader callback for A2DP audio data.
             * @param callBack Function pointer taking (const uint8_t* data, uint32_t size).
             */
            void set_stream_reader(void (*callBack)(const uint8_t*, uint32_t));

            /**
             * @brief Stop the A2DP stream without disconnecting the Bluetooth link.
             */
            void stop_a2dp();

            /**
             * @brief Pause the currently playing A2DP stream.
             */
            void pause_a2dp();

            /**
             * @brief Resume playing the A2DP stream after a pause.
             */
            void play_a2dp();

            /**
             * @brief Skip to the next track via AVRCP.
             */
            void next_track();

            /**
             * @brief Skip to the previous track via AVRCP.
             */
            void prev_track();

            /**
             * @brief Get the current A2DP connection state.
             * @return esp_a2d_connection_state_t The connection state enum value.
             */
            esp_a2d_connection_state_t get_connection_state();

            /**
             * @brief Set the connection state (connect/disconnect).
             * @param connected True to connect, false to disconnect.
             */
            void set_connected(bool connected);

            /**
             * @brief Get the peer device name.
             * @return const char* The peer device name string.
             */
            const char* get_peer_name();

            /**
             * @brief Get the current peer Bluetooth address.
             * @return esp_bd_addr_t* Pointer to the 6-byte Bluetooth address.
             */
            esp_bd_addr_t* get_current_peer_address();

            /**
             * @brief Convert an esp_bd_addr_t (6-byte Bluetooth address) to a human-readable
             *        string like "AA:BB:CC:DD:EE:FF".
             * @param addr The Bluetooth address to convert.
             * @return std::string The formatted address string.
             */
            static std::string bd_addr_to_string(const esp_bd_addr_t& addr);

        protected:
            std::string name_;
            bool auto_reconnect_;

#ifdef USE_A2DP_CONNECTION_STATE
            /**
             * @brief Handle A2DP connection state changes and forward to subscribers.
             * @param state The new connection state.
             * @param user_data User-provided context pointer.
             */
            void on_connection_state_changed(esp_a2d_connection_state_t state, void *user_data);

            CallbackManager<void(esp_a2d_connection_state_t, void*)> connection_state_callbacks_{};
#endif

#ifdef USE_A2DP_PLAYBACK_STATUS
            /**
             * @brief Handle AVRC playback status changes and forward to subscribers.
             * @param playback The new playback status.
             */
            void avrc_rn_playstatus_callback(esp_avrc_playback_stat_t playback);

            CallbackManager<void(esp_avrc_playback_stat_t playback)> playback_status_callbacks_{};
#endif

#ifdef USE_A2DP_POS
            /**
             * @brief Handle playback position updates and forward to subscribers.
             * @param play_pos Current playback position in milliseconds.
             */
            void avrc_rn_play_pos_callback(uint32_t play_pos);

            CallbackManager<void(uint32_t pos)> playback_position_callbacks_{};

#endif

#ifdef USE_A2DP_RSSI
            /**
             * @brief Handle RSSI delta updates and forward to subscribers.
             * @param rssi RSSI delta parameter structure.
             */
            void rssi_callback(esp_bt_gap_cb_param_t::read_rssi_delta_param& rssi);

            CallbackManager<void(esp_bt_gap_cb_param_t::read_rssi_delta_param&)> rssi_callbacks_{};
#endif

#ifdef USE_A2DP_METADATA
            /**
             * @brief Handle AVRC metadata updates and forward to subscribers.
             * @param id The metadata attribute ID.
             * @param text Pointer to the metadata text value.
             */
            void avrc_metadata_callback(uint8_t id, const uint8_t *text);

            CallbackManager<void(const A2DPSinkMetadata &)> metadata_update_callbacks_{};
            uint8_t metadata_mask_ = 0;

#endif

#ifdef USE_A2DP_PEER_NAME
            /**
             * @brief Handle peer device name resolution and forward to subscribers.
             * @param name The resolved peer device name.
             */
            void peer_name_callback(const char* name);

            CallbackManager<void(const char*)> peer_name_callbacks_{};
#endif

#ifdef USE_A2DP_VOLUME
            /**
             * @brief Handle volume changes and forward to subscribers.
             * @param volume The new volume level.
             */
            void avrc_rn_volumechange_callback(int volume);

            CallbackManager<void(int)> volume_change_callbacks_{};
#endif

#ifdef USE_A2DP_AUDIO_STATE
            /**
             * @brief Handle A2DP audio state changes and forward to subscribers.
             * @param state The new audio state.
             */
            void audio_state_callback(esp_a2d_audio_state_t state);

            CallbackManager<void(esp_a2d_audio_state_t)> audio_state_callbacks_{};
#endif

#ifdef USE_A2DP_AVRCP_CONNECTION_STATE
            /**
             * @brief Handle AVRCP connection state changes and forward to subscribers.
             * @param connected True if AVRCP is connected.
             */
            void avrc_connection_state_callback(bool connected);

            CallbackManager<void(bool)> avrc_connection_state_callbacks_{};
#endif

#ifdef USE_A2DP_SAMPLE_RATE
            /**
             * @brief Handle sample rate changes and forward to subscribers.
             * @param sample_rate The new sample rate in Hz.
             */
            void sample_rate_callback(uint16_t sample_rate);

            CallbackManager<void(uint16_t)> sample_rate_callbacks_{};
#endif


        }; // class A2DPSinkHub

        /**
         * @brief Base class for all a2dp_sink subcomponents.
         *
         * Consolidates the Component + Parented<A2DPSinkHub> inheritance and pins the setup
         * priority so the hub's setup() always runs before any child. Subcomponents should
         * inherit from this instead of listing Component/Parented individually and must not
         * override get_setup_priority().
         */
        class A2DPSinkChild : public Component, public Parented<A2DPSinkHub> {
        public:
            /**
             * @brief Get the ESPHome setup priority for child components.
             * @return float Setup priority (runs after hub).
             */
            float get_setup_priority() const override { return a2dp_sink_priority::CHILD; }

        };


    } // namespace a2dp_sink
} // namespace esphome
