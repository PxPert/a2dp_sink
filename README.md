# A2DP Sink Component

ESPHome custom component that turns an ESP32 into a Bluetooth A2DP audio sink. The device receives audio streamed from a paired phone, tablet, or laptop and exposes connection state, playback metadata, volume, RSSI signal strength, and other telemetry as ESPHome entities and automation triggers.

**Platform:** ESP32 only  
**Code owner:** @PxPert

---

## Table of Contents

### Components

- [Hub](#hub) — Central A2DP sink management
  - [Parameters](#hub-parameters)
  - [Bluetooth Stack Options](#hub-bluetooth-stack-options)
  - [Automation Triggers](#hub-automation-triggers)
    - [`on_connection_state`](#on_connection_state)
    - [`on_playback_status`](#on_playback_status)
    - [`on_playback_position`](#on_playback_position)
    - [`on_rssi`](#on_rssi)
    - [`on_metadata`](#on_metadata)
    - [`on_peer_name`](#on_peer_name)
    - [`on_volume_change`](#on_volume_change)
    - [`on_sample_rate`](#on_sample_rate)
    - [`on_avrcp_connection_state`](#on_avrcp_connection_state)
    - [`on_audio_state`](#on_audio_state)

- [Switch](#switch) — Connection and Bluetooth power controls
  - [Connection Switch](#connection-switch)
  - [Bluetooth Switch](#bluetooth-switch)
  - [Parameters](#switch-parameters)

- [Sensor](#sensor) — Numeric A2DP measurements
  - [Available Types](#sensor-available-types)
  - [Parameters](#sensor-parameters)

- [Text Sensor](#text-sensor) — String-based A2DP information
  - [Available Types](#text-sensor-available-types)
  - [Parameters](#text-sensor-parameters)

- [Number](#number) — Volume control
  - [Parameters](#number-parameters)

- [Media Source](#media-source) — ESPHome audio framework integration
  - [Supported Commands](#media-source-commands)
  - [State Management](#media-source-states)

### Reference

- [Complete Example](#complete-example)
- [Use Cases](#use-cases)
- [Architecture](#architecture)

---

## Architecture

The component is organized around a central hub (`A2DPSinkHub`) that manages the Bluetooth A2DP lifecycle. Child components (switches, sensors, text sensors, number controls, media source) subscribe to hub events via a callback system. Feature flags are enabled on demand — only the callbacks and code paths needed by your configuration are compiled in.

```
a2dp_sink:
  ├── switch/          Connection & Bluetooth power switches
  ├── sensor/          Numeric sensors (RSSI, position, sample rate, etc.)
  ├── text_sensor/     Text sensors (metadata, peer name/address)
  ├── number/          Volume control
  └── media_source/    ESPHome media source integration
```

---

## Hub

The `a2dp_sink` block declares the hub and its automation triggers.

```yaml
a2dp_sink:
  name: "My A2DP Sink"          # (Required) Friendly name for the device
  auto_reconnect: true          # (Optional, default: true) Reconnect to last paired device
  minimal_bluetooth: true        # (Optional, default: true) Bluetooth Classic only, no BLE
```

### Hub Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `name` | string | Yes | — | Human-readable name for the A2DP sink device |
| `auto_reconnect` | bool | No | `true` | Automatically reconnect to the last paired Bluetooth device on boot |
| `minimal_bluetooth` | bool | No | `true` | Compile Bluetooth stack with Classic (BR/EDR) only, disabling BLE, NIMBLE, and GATT server. Saves RAM and flash on devices that only need A2DP audio streaming. |
| `id` | ID | No | — | Internal ID for referencing from child components |

### Hub Bluetooth Stack Options

When `minimal_bluetooth: true` (default), the following IDF SDK options are set to strip out unused Bluetooth protocols:

| SDK Config Option | Value | Effect |
|-------------------|-------|--------|
| `CONFIG_BTDM_CTRL_MODE_BR_EDR_ONLY` | `True` | Dual-mode disabled, BR/EDR (Classic) only |
| `CONFIG_BT_BLE_ENABLED` | `False` | BLE (Bluetooth Low Energy) disabled |
| `CONFIG_BT_NIMBLE_ENABLED` | `False` | NimBLE stack disabled |
| `CONFIG_BT_GATTS_ENABLE` | `False` | GATT server disabled |

Set `minimal_bluetooth: false` to enable the full dual-mode Bluetooth stack (BR/EDR + BLE). This is useful if you need BLE for other ESPHome integrations alongside A2DP.

### Hub Automation Triggers

The following triggers fire when the corresponding A2DP/AVRCP event occurs. Each trigger exposes variables available in automation actions.

#### `on_connection_state`

Fires when the A2DP connection state changes (connected, disconnected, connecting, disconnecting).

| Variable | Type | Description |
|----------|------|-------------|
| `state` | uint8 | The A2DP connection state value |

```yaml
a2dp_sink:
  name: "My A2DP Sink"
  on_connection_state:
    then:
      - if:
          condition:
            lambda: 'return x == ESP_A2D_CONNECTION_STATE_CONNECTED;'
          then:
            - logger.log: "A2DP client connected"
          else:
            - logger.log: "A2DP client disconnected"
```

#### `on_playback_status`

Fires when the AVRC playback status changes (playing, paused, stopped).

| Variable | Type | Description |
|----------|------|-------------|
| `playback` | uint8 | The AVRC playback status value |

```yaml
a2dp_sink:
  name: "My A2DP Sink"
  on_playback_status:
    then:
      - logger.log:
          format: "Playback status changed: %d"
          args: [playback]
```

#### `on_playback_position`

Fires when the playback position of the current track changes.

| Variable | Type | Description |
|----------|------|-------------|
| `pos` | uint32 | Current position in the track (milliseconds) |

```yaml
a2dp_sink:
  name: "My A2DP Sink"
  on_playback_position:
    then:
      - logger.log:
          format: "Track position: %d ms"
          args: [pos]
```

#### `on_rssi`

Fires when the RSSI signal strength delta changes.

| Variable | Type | Description |
|----------|------|-------------|
| `rssi_delta` | int8 | RSSI delta value |

```yaml
a2dp_sink:
  name: "My A2DP Sink"
  on_rssi:
    then:
      - logger.log:
          format: "RSSI delta: %d"
          args: [rssi_delta]
```

#### `on_metadata`

Fires when AVRC metadata is updated from the source device.

| Variable | Type | Description |
|----------|------|-------------|
| `type` | uint8 | The metadata attribute type (title, artist, album, genre, etc.) |
| `text` | string | The metadata text value |

```yaml
a2dp_sink:
  name: "My A2DP Sink"
  on_metadata:
    then:
      - logger.log:
          format: "Metadata type %d: %s"
          args: [type, text.c_str()]
```

#### `on_peer_name`

Fires when the peer device name is resolved.

| Variable | Type | Description |
|----------|------|-------------|
| `name` | string | The peer device name |

```yaml
a2dp_sink:
  name: "My A2DP Sink"
  on_peer_name:
    then:
      - logger.log:
          format: "Connected to: %s"
          args: [name.c_str()]
```

#### `on_volume_change`

Fires when the A2DP volume changes.

| Variable | Type | Description |
|----------|------|-------------|
| `volume` | uint8 | The new volume level (0–127) |

```yaml
a2dp_sink:
  name: "My A2DP Sink"
  on_volume_change:
    then:
      - logger.log:
          format: "Volume changed to: %d"
          args: [volume]
```

#### `on_sample_rate`

Fires when the A2DP sample rate changes.

| Variable | Type | Description |
|----------|------|-------------|
| `sample_rate` | uint16 | The new sample rate in Hz |

```yaml
a2dp_sink:
  name: "My A2DP Sink"
  on_sample_rate:
    then:
      - logger.log:
          format: "Sample rate changed to: %d Hz"
          args: [sample_rate]
```

#### `on_avrcp_connection_state`

Fires when the AVRCP connection state changes.

| Variable | Type | Description |
|----------|------|-------------|
| `connected` | bool | `true` if AVRCP is connected, `false` otherwise |

```yaml
a2dp_sink:
  name: "My A2DP Sink"
  on_avrcp_connection_state:
    then:
      - if:
          condition:
            lambda: "return connected;"
          then:
            - logger.log: "AVRCP connected"
          else:
            - logger.log: "AVRCP disconnected"
```

#### `on_audio_state`

Fires when the A2DP audio state changes (suspended, stopped, started, paused).

| Variable | Type | Description |
|----------|------|-------------|
| `state` | uint8 | The A2DP audio state value |

```yaml
a2dp_sink:
  name: "My A2DP Sink"
  on_audio_state:
    then:
      - logger.log:
          format: "Audio state changed: %d"
          args: [state]
```

---

## Switch

Two switch types are available for controlling and monitoring the A2DP sink.

### Connection Switch

Read-only switch that reflects the A2DP client connection state. The switch state updates automatically when a client connects or disconnects.

```yaml
switch:
  - platform: a2dp_sink
    type: connection
    a2dp_sink_id: my_a2dp_hub
    name: "A2DP Connection"
```

### Bluetooth Switch

Controls the Bluetooth A2DP sink power state. Turning it ON starts the A2DP sink (makes it discoverable); turning it OFF stops the sink and disconnects any paired device. Supports restore mode.

```yaml
switch:
  - platform: a2dp_sink
    type: bluetooth
    a2dp_sink_id: my_a2dp_hub
    name: "Bluetooth A2DP"
    restore_mode: RESTORE_DEFAULT_OFF
```

### Switch Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `type` | enum | Yes | — | Switch type: `connection` or `bluetooth` |
| `a2dp_sink_id` | ID | Yes | — | Reference to the parent `a2dp_sink` hub |
| `name` | string | Yes | — | Display name for the switch |

### Sensor Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `type` | enum | Yes | — | Sensor type (see table above) |
| `a2dp_sink_id` | ID | Yes | — | Reference to the parent `a2dp_sink` hub |
| `name` | string | Yes | — | Display name for the sensor |

### Text Sensor Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `type` | enum | Yes | — | Text sensor type (see table above) |
| `a2dp_sink_id` | ID | Yes | — | Reference to the parent `a2dp_sink` hub |
| `name` | string | Yes | — | Display name for the text sensor |

### Number Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `a2dp_sink_id` | ID | Yes | — | Reference to the parent `a2dp_sink` hub |
| `name` | string | Yes | — | Display name for the number control |

### Media Source Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `a2dp_sink_id` | ID | Yes | — | Reference to the parent `a2dp_sink` hub |

---

## Sensor

Numeric sensors expose A2DP-related measurements.

### Sensor Available Types

| Type | Description | Unit |
|------|-------------|------|
| `tracknum` | Current track number from AVRC metadata | — |
| `playingtime` | Total playing time of the current track | — |
| `num_tracks` | Total number of tracks in the current playlist | — |
| `trackposition` | Current playback position in the track | ms |
| `rssi` | RSSI signal strength delta | dBm |
| `samplereate` | Current audio sample rate | Hz |
| `channels` | Number of audio channels | — |

### Example

```yaml
sensor:
  - platform: a2dp_sink
    type: rssi
    a2dp_sink_id: my_a2dp_hub
    name: "A2DP RSSI"

  - platform: a2dp_sink
    type: trackposition
    a2dp_sink_id: my_a2dp_hub
    name: "Track Position"

  - platform: a2dp_sink
    type: samplereate
    a2dp_sink_id: my_a2dp_hub
    name: "Sample Rate"

  - platform: a2dp_sink
    type: channels
    a2dp_sink_id: my_a2dp_hub
    name: "Audio Channels"

  - platform: a2dp_sink
    type: tracknum
    a2dp_sink_id: my_a2dp_hub
    name: "Track Number"

  - platform: a2dp_sink
    type: playingtime
    a2dp_sink_id: my_a2dp_hub
    name: "Playing Time"

  - platform: a2dp_sink
    type: num_tracks
    a2dp_sink_id: my_a2dp_hub
    name: "Total Tracks"
```

### Sensor Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `type` | enum | Yes | — | Sensor type (see table above) |
| `a2dp_sink_id` | ID | Yes | — | Reference to the parent `a2dp_sink` hub |
| `name` | string | Yes | — | Display name for the sensor |

---

## Text Sensor

Text sensors expose string-based A2DP information.

### Text Sensor Available Types

| Type | Description |
|------|-------------|
| `title` | Current track title from AVRC metadata |
| `artist` | Current artist from AVRC metadata |
| `album` | Current album from AVRC metadata |
| `genre` | Current genre from AVRC metadata |
| `peername` | Human-readable name of the connected Bluetooth device |
| `peeraddr` | Bluetooth MAC address of the connected device (e.g. `AA:BB:CC:DD:EE:FF`) |

### Example

```yaml
text_sensor:
  - platform: a2dp_sink
    type: title
    a2dp_sink_id: my_a2dp_hub
    name: "Track Title"

  - platform: a2dp_sink
    type: artist
    a2dp_sink_id: my_a2dp_hub
    name: "Artist"

  - platform: a2dp_sink
    type: album
    a2dp_sink_id: my_a2dp_hub
    name: "Album"

  - platform: a2dp_sink
    type: genre
    a2dp_sink_id: my_a2dp_hub
    name: "Genre"

  - platform: a2dp_sink
    type: peername
    a2dp_sink_id: my_a2dp_hub
    name: "Connected Device"

  - platform: a2dp_sink
    type: peeraddr
    a2dp_sink_id: my_a2dp_hub
    name: "Device Address"
```

### Text Sensor Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `type` | enum | Yes | — | Text sensor type (see table above) |
| `a2dp_sink_id` | ID | Yes | — | Reference to the parent `a2dp_sink` hub |
| `name` | string | Yes | — | Display name for the text sensor |

---

## Number

The volume number control lets you adjust the A2DP sink volume from the ESPHome dashboard.

### Example

```yaml
number:
  - platform: a2dp_sink
    a2dp_sink_id: my_a2dp_hub
    name: "A2DP Volume"
```

### Number Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `a2dp_sink_id` | ID | Yes | — | Reference to the parent `a2dp_sink` hub |
| `name` | string | Yes | — | Display name for the number control |

**Volume range:** 0 (mute) to 127 (maximum), step 5

---

## Media Source

The A2DP sink can be used as an ESPHome media source, integrating with the audio framework for play/pause/stop/next/previous controls.

### Example

```yaml
media_source:
  - platform: a2dp_sink
    a2dp_sink_id: my_a2dp_hub
```

### Media Source Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `a2dp_sink_id` | ID | Yes | — | Reference to the parent `a2dp_sink` hub |

### Media Source Commands

- **Play:** Starts the A2DP stream (URI scheme: `a2dp://`)
- **Pause:** Pauses the A2DP stream
- **Stop:** Stops the A2DP stream
- **Next:** Skips to the next track via AVRCP
- **Previous:** Skips to the previous track via AVRCP

### Media Source States

The media source tracks the following states:

| State | Description |
|-------|-------------|
| `IDLE` | No audio playing, waiting for connection |
| `PLAYING` | Audio is actively streaming |
| `PAUSED` | Audio stream is paused |

State transitions are automatically synchronized with A2DP audio state and AVRC playback status callbacks.

---

## Complete Example

```yaml
substitutions:
  friendly_name: Sample device
  device_name: sample-device

esphome:
  name: ${device_name}
  friendly_name: ${friendly_name}

psram:
  mode: quad
  speed: 80MHZ
  ignore_not_found: true
  
mdns:
  disabled: true
  
esp32:
  board: esp-wrover-kit
  cpu_frequency: 240MHZ
  flash_frequency: 80MHz
  
  framework:
    type: esp-idf
    advanced:
      enable_ota_rollback: false
      sram1_as_iram: true
  

wifi:
  power_save_mode: none
  ssid: !secret wifi_dmz_ssid
  password: !secret wifi_dmz_password
  post_connect_roaming: false
  
logger:
  hardware_uart: UART0
  level: WARN
  baud_rate: 0

api:
  encryption:
    key: !secret api_key

ota:
  - platform: esphome
    password: !secret ota_pwd_sec

external_components:
  - source: github://PxPert/a2dp_sink@master
    components: [ a2dp_sink ]
    
a2dp_sink:
  name: SampleBTPlayer
  minimal_bluetooth: true
          
i2s_audio:
  - id: i2s_sample
    
speaker:
  - platform: i2s_audio
    id: speaker_sample
    dac_type: external
    channel: stereo
    use_apll: true
    sample_rate: 48000
    i2s_dout_pin: GPIO32
    timeout: 1s
    buffer_duration: 50ms
    

media_source:
  - platform: a2dp_sink
    id: media_source_bluetooth
    
media_player:    
  - platform: speaker_source
    id: external_media_player
    name: Media Player
    media_pipeline:
      format: WAV
      num_channels: 2
      sample_rate: 48000
      speaker: speaker_sample
      sources:
        - media_source_bluetooth

        
number:
  - platform: a2dp_sink
    name: "Bluetooth Volume"

    
switch:
  - platform: a2dp_sink
    type: connection
    name: "Bluetooth connection"
    id: "btConnection"
    restore_mode: ALWAYS_OFF

  - platform: a2dp_sink
    type: bluetooth
    name: "Bluetooth state"
    id: "btState"
    restore_mode: RESTORE_DEFAULT_OFF

button:
  - platform: template
    name: next
    on_press:
      - then:
          - media_player.next: external_media_player
  - platform: template
    name: previous
    on_press:
      - then:
          - media_player.previous: external_media_player
            
  - platform: restart
    name: "restart"
    id: button_restart_1
```
---

## Use Cases

### Bluetooth Speaker

Set up an ESP32 with a DAC/I2S amplifier as a Bluetooth speaker. Use the media source integration for playback controls and the volume number for remote volume adjustment.

### Track Information Display

Subscribe to metadata text sensors (title, artist, album) and display the now-playing information on an OLED or e-ink screen.

### Signal Quality Monitoring

Use the RSSI sensor and `on_rssi` trigger to monitor Bluetooth connection quality and alert when the signal drops below a threshold.

### Multi-room Audio Routing

Combine the `on_connection_state` trigger with Home Assistant automations to route audio to different speakers based on which device is connected.
