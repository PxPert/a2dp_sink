#pragma once

#include "esphome/components/audio/audio.h"
#include "esphome/components/media_source/media_source.h"
#include "esphome/core/component.h"
#include "../a2dp_sink_hub.h"


namespace esphome::a2dp_sink {


        /**
         * @brief Media source integration for A2DP Bluetooth audio sink.
         *
         * Bridges the ESPHome media source framework with the A2DP sink,
         * handling playback commands (play, pause, stop, next, previous),
         * audio data streaming, and state management.
         */
        class A2DPSinkMediaSource : public A2DPSinkChild, public media_source::MediaSource
        {
        public:
            /**
             * @brief Initialize the A2DP media source.
             *
             * Sets up the stream reader callback, AVRCP connection state,
             * audio state, playback status, connection state, and sample rate
             * callbacks to synchronize media source state with A2DP events.
             */
            void setup() override;

            /**
             * @brief Main loop for the A2DP media source.
             *
             * Currently a no-op; the component is event-driven.
             */
            void loop() override;

            /**
             * @brief Log the A2DP media source configuration for debugging.
             */
            void dump_config() override;

            // MediaSource interface implementation
            /**
             * @brief Start playing the given URI.
             * @param uri The media URI to play (must start with "a2dp://").
             * @return bool True if the URI was accepted, false otherwise.
             */
            virtual bool play_uri(const std::string &uri) override;

            /**
             * @brief Handle a media source command (play, pause, stop, next, previous).
             * @param command The media source command to execute.
             */
            virtual void handle_command(media_source::MediaSourceCommand command) override;

            /**
             * @brief Check if this media source can handle the given URI.
             * @param uri The URI to check.
             * @return bool True if the URI starts with "a2dp://".
             */
            virtual bool can_handle(const std::string &uri) const override;

            /**
             * @brief Check if this media source has an internal playlist.
             * @return bool Always true for A2DP sink.
             */
            virtual bool has_internal_playlist() const { return true; }


        protected:
            /**
             * @brief Process incoming A2DP audio data.
             * @param data Pointer to the audio data buffer.
             * @param length Size of the audio data in bytes.
             */
            void a2dp_data_stream(const uint8_t *data, uint32_t length);

            /** @brief Audio stream information (bit depth, channels, sample rate). */
            audio::AudioStreamInfo stream_info_;

        }; // class A2DPSink

} // namespace esphome::a2dp_sink
