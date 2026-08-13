#include "esphome/core/log.h"
#include "a2dp_sink_media_source.h"
// #include "AudioTools.h"
// #include "AudioTools/AudioCodecs/CodecSBC.h"
// #include "AudioTools/AudioCodecs/CodecAACHelix.h"
#include "BluetoothA2DPSink.h"
#include <cstdint>
// #include "A2DPDecoderSBC.h"
// #include "A2DPDecoderAAC.h"

namespace esphome::a2dp_sink {

        static constexpr uint32_t PAUSE_POLL_DELAY_MS = 20;
        // static constexpr uint32_t AUDIO_WRITE_TIMEOUT_MS = 50;
        static constexpr uint32_t AUDIO_WRITE_TIMEOUT_MS = 10;
        static constexpr const char *const URI_PREFIX = "a2dp://";
        static const char *TAG = "a2dp_sink_media_source";

        /** @brief Global singleton pointer to the active A2DP media source instance. */
        static A2DPSinkMediaSource *g_a2dp_sink_instance = nullptr;

        /**
         * @brief Main loop for the A2DP media source.
         *
         * Currently a no-op; the component is event-driven via callbacks.
         */
        void A2DPSinkMediaSource::loop() {
            /*
            static unsigned long int last_stub_time_ = 0;

            if (millis() - last_stub_time_ >= 5000) {
                // --- comando stub qui ---
                ESP_LOGI(TAG, "Stub executed %d - audio state: %d", (int)this->get_state(),this->parent_->a2dp_sink()->get_audio_state());
                // -------------------------
                last_stub_time_ = millis();
            }
            */

        }
        /**
         * @brief Initialize the A2DP media source.
         *
         * Sets up the stream reader callback, AVRCP connection state,
         * audio state, playback status, connection state, and sample rate
         * callbacks to synchronize media source state with A2DP events.
         */
        void A2DPSinkMediaSource::setup()
        {
            ESP_LOGI(TAG, "A2DP Sink Media Source initializing");

            this->disable_loop();

            g_a2dp_sink_instance = this;

            this->parent_->set_stream_reader(
                [](const uint8_t *data, uint32_t length) {
                    if (g_a2dp_sink_instance != nullptr) {
                        g_a2dp_sink_instance->a2dp_data_stream(data, length);
                    }
                }
            );

            this->parent_->add_avrc_connection_state_callback([this](bool connected) {
                if (connected) {
                    this->parent_->play_a2dp();
                }
            });

            this->parent_->add_audio_state_callback([this](esp_a2d_audio_state_t state) {
                ESP_LOGD(TAG, "Audio state: %d", state);
                if (state == ESP_A2D_AUDIO_STATE_STARTED) {
                    this->set_state_(media_source::MediaSourceState::PLAYING);
                } else {
                    if (this->get_state() == media_source::MediaSourceState::PLAYING) {
                    this->set_state_(media_source::MediaSourceState::IDLE);
                    }
                }
            });

            this->parent_->add_playback_status_callbacks([this](esp_avrc_playback_stat_t playback) {
                ESP_LOGD(TAG, "Playback status: %d", playback);
                switch (playback) {
                    case ESP_AVRC_PLAYBACK_PLAYING:
                        //this->request_play_uri_(URI_PREFIX);
                        this->set_state_(media_source::MediaSourceState::PLAYING);
                        break;
                    case ESP_AVRC_PLAYBACK_STOPPED:
                        this->set_state_(media_source::MediaSourceState::IDLE);
                        break;
                    case ESP_AVRC_PLAYBACK_PAUSED:
                        this->set_state_(media_source::MediaSourceState::PAUSED);
                        break;
                    default:
                        break;
                }
            });

            this->parent_->add_connection_state_callbacks([this](esp_a2d_connection_state_t state, void *user_data) {
                this->set_state_(media_source::MediaSourceState::IDLE);
                if (state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
                    this->request_play_uri_(URI_PREFIX);
                }
            });

            this->parent_->add_sample_rate_callback([this](uint16_t sample_rate) {
                this->stream_info_ = audio::AudioStreamInfo(
                    16, 
                    this->parent_->a2dp_sink()->channels(), 
                    sample_rate
                );                
            });

            ESP_LOGI(TAG, "A2DP Sink Media Source initialized");
        }

        /**
         * @brief Log the A2DP media source configuration.
         */
        void A2DPSinkMediaSource::dump_config()
        {
            ESP_LOGCONFIG(TAG, "A2DP Sink Media Source");
        }

        /**
         * @brief Start playing the given URI.
         * @param uri The media URI to play (must start with "a2dp://").
         * @return bool True if the URI was accepted, false otherwise.
         */
        bool A2DPSinkMediaSource::play_uri(const std::string &uri) {
            ESP_LOGD(TAG, "Play URI: '%s'", uri.c_str());
            
            if (!this->is_ready() || this->is_failed() || this->status_has_error()) {
                return false;
            }

            // Check if source is already playing
            if (this->get_state() != media_source::MediaSourceState::IDLE) {
                ESP_LOGW(TAG, "Cannot play URI '%s': source not idle (state=%d)", uri.c_str(), (int)this->get_state());
                return false;
            }

            // Validate URI starts with "a2dp://"
            if (!this->can_handle(uri)) {
                ESP_LOGW(TAG, "URI '%s' not handled: does not start with '%s'", uri.c_str(), URI_PREFIX);
                return false;
            }

            ESP_LOGI(TAG, "URI '%s' accepted for playback", uri.c_str());
            return true;
        }

        /**
         * @brief Handle a media source command.
         *
         * Routes the command to the appropriate A2DP action:
         * - STOP: disconnects from the A2DP source
         * - PAUSE: pauses the A2DP stream
         * - PLAY: resumes the A2DP stream
         * - NEXT: skips to the next track
         * - PREVIOUS: skips to the previous track
         * @param command The media source command to execute.
         */
        void A2DPSinkMediaSource::handle_command(media_source::MediaSourceCommand command) {
            ESP_LOGD(TAG, "Handling media command: %d", command);
            switch (command) {
                case media_source::MediaSourceCommand::STOP:
                    ESP_LOGD(TAG, "Executing STOP command");
                    if (
                        (this->get_state() != media_source::MediaSourceState::PLAYING) 
                        &&
                        (this->get_state() != media_source::MediaSourceState::PAUSED)
                    )
                    {
                        ESP_LOGW(TAG, "Cannot stop: source not in PLAYING or PAUSED state");
                        return;
                    }
                    this->parent_->set_connected(false);
                    this->set_state_(media_source::MediaSourceState::IDLE);
                break;
                case media_source::MediaSourceCommand::PAUSE:
                    ESP_LOGD(TAG, "Executing PAUSE command");
                    if (this->get_state() != media_source::MediaSourceState::PLAYING) {
                        ESP_LOGW(TAG, "Cannot pause: source not in PLAYING state");
                        return;
                    }
                    this->parent_->pause_a2dp();
                    // this->set_state_(media_source::MediaSourceState::PAUSED);
                break;
                case media_source::MediaSourceCommand::PLAY:
                    ESP_LOGD(TAG, "Executing PLAY command");
                    if (this->parent_->get_connection_state() != ESP_A2D_CONNECTION_STATE_CONNECTED) {
                        ESP_LOGW(TAG, "Cannot play: A2DP not connected (state=%d)", this->parent_->get_connection_state());
                    } else {
                        this->parent_->play_a2dp();
                    }
                    // this->set_state_(media_source::MediaSourceState::PLAYING);
                break;
                case media_source::MediaSourceCommand::NEXT:
                    ESP_LOGD(TAG, "Executing NEXT command");
                    if (this->parent_->get_connection_state() != ESP_A2D_CONNECTION_STATE_CONNECTED) {
                        ESP_LOGW(TAG, "Cannot skip next: A2DP not connected");
                        return;
                    }
                    this->parent_->next_track();
                break;
                case media_source::MediaSourceCommand::PREVIOUS:
                    ESP_LOGD(TAG, "Executing PREVIOUS command");
                    if (this->parent_->get_connection_state() != ESP_A2D_CONNECTION_STATE_CONNECTED) {
                        ESP_LOGW(TAG, "Cannot skip previous: A2DP not connected");
                        return;
                    }
                    this->parent_->prev_track();
                break;
                default:
                    ESP_LOGW(TAG, "Unhandled media command: %d", (uint8_t)command);
                break;
            }
        }

        /**
         * @brief Check if this media source can handle the given URI.
         * @param uri The URI to check.
         * @return bool True if the URI starts with "a2dp://".
         */
        bool A2DPSinkMediaSource::can_handle(const std::string &uri) const {
            // ESP_LOGE(TAG, "Check URI: '%s'", uri.c_str());
            return uri.starts_with(URI_PREFIX);
        }

        /**
         * @brief Process incoming A2DP audio data.
         *
         * If the media source is in the PLAYING state, writes the audio
         * data to the output stream.
         * @param data Pointer to the audio data buffer.
         * @param length Size of the audio data in bytes.
         */
        void A2DPSinkMediaSource::a2dp_data_stream(const uint8_t *data, uint32_t length) {

            if (this->get_state() == media_source::MediaSourceState::PLAYING) {
                this->write_output(data, length, AUDIO_WRITE_TIMEOUT_MS, this->stream_info_);
            }

        }

} // namespace esphome::a2dp_sink
