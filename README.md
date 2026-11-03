# audio_io_interface

## 简介

`audio_io_interface` 定义 ROS 2 音频 I/O 节点（`/audio/io`）使用的通用消息、服务和动作：

- 播放任意音频片段（`AudioClip`）；
- 播放实时 PCM 流（`AudioStream`）；
- 发布播放/采集状态（`AudioState`）与事件（`AudioEvent`）；
- 停止播放（`StopAudio`）、设置音量（`SetVolume`）；
- 通过 action 播放并获取进度（`PlayClip`）。

本包只包含 **audio_io 的通用设备/播放接口**，不包含 TTS、鸭子语义叫声或摸头检测消息；这些属于后续的 TTS / duck_audio 接口包。

生成命名空间为 `audio_io_interface`，Python 模块为 `audio_io_interface_msg`。

## 端点约定

与 `remote_io_interface` 一致，本包把 endpoint 创建者和外部消费者的名称 API 分开：

- `~/...` 是 ROS 2 **endpoint owner 私有名称**，只能由创建该 publisher/subscriber/service/action 的 `audio_io` 节点使用；它会相对创建者自身的节点名展开。
- GUI、App、其他业务节点必须把目标 `audio_io` 节点的完整节点名（FQN）传给 helper，由 helper 生成绝对 endpoint 名称。外部节点若自行使用 `~/...`，它会指向外部节点自身。

默认节点 FQN 为 `/audio/io`。

| 类型 | owner 私有名称 | 外部消费者 helper |
| --- | --- | --- |
| 播放话题 | `~/play` | `topic_play(node_fqn)` |
| PCM 流话题 | `~/stream` | `topic_stream(node_fqn)` |
| 采集话题 | `~/capture` | `topic_capture(node_fqn)` |
| 状态话题 | `~/state` | `topic_state(node_fqn)` |
| 事件话题 | `~/event` | `topic_event(node_fqn)` |
| 播放动作 | `~/play_clip` | `action_play_clip(node_fqn)` |
| 停止服务 | `~/stop` | `service_stop(node_fqn)` |
| 音量服务 | `~/set_volume` | `service_set_volume(node_fqn)` |

`node_fqn` 必须是非根绝对 ROS 名称，不能包含 ASCII 空白或中间的空路径段；尾部多余 `/` 会自动移除。

例如目标节点是 `/robot_a/audio/io` 时：

```text
/robot_a/audio/io/play
/robot_a/audio/io/stream
/robot_a/audio/io/capture
/robot_a/audio/io/state
/robot_a/audio/io/event
/robot_a/audio/io/play_clip
/robot_a/audio/io/stop
/robot_a/audio/io/set_volume
```

## 消息

### `AudioClip`

一段可播放的音频片段。`uri` 支持 `file://`、`package://`；`http(s)://` 是否可用由 `audio_io` 配置决定。

```msg
std_msgs/Header header
string request_id
string uri
string format
float32 gain
bool loop
uint8 priority
uint8 policy
string group
float64 start_offset
float64 duration
```

优先级与策略常量的取值和含义以 `msg/AudioClip.msg` 内的注释为准，README 不再重复维护。

### `AudioStream`

一段 PCM 音频流。首包开始播放；`eos=true` 表示流结束；超时没有新包时 `audio_io` 会淡出并释放。

```msg
std_msgs/Header header
string stream_id
string encoding
uint32 sample_rate
uint8 channels
uint64 sequence
bool eos
uint8[] data
```

`encoding` 当前支持 `s16le` 与 `f32le`，数据为小端、交错排列。

### 麦克风采集

麦克风数据不单独定义 `AudioCapture` 消息，而是复用 `AudioStream`：

- `audio_io` 在 `~/capture` 发布采集到的 PCM；
- 设备原生采样率通常为 48 kHz，单声道/立体声由设备决定；
- `sequence` 用于检测丢包与乱序；
- 停止采集时发送 `eos=true`；异常断开由 `AudioEvent` 的 `DEVICE_LOST` / `DEVICE_RECOVERED` 表示；
- 采集健康看 `AudioState.capture_ok` 与 `AudioState.capture_error`；
- `duck_ear` 订阅该话题后重采样到 16 kHz mono，再跑 log-mel + ONNX。

外部节点通过 helper 获取绝对名称：

```cpp
const auto capture_topic = audio_io_interface::topic_capture("/robot_a/audio/io");
```

```python
from audio_io_interface_msg.ros_msg import topic_capture

print(topic_capture("/robot_a/audio/io"))  # /robot_a/audio/io/capture
```

### `AudioState`

播放与采集状态快照。通过 `transient_local` QoS 发布，新订阅者可以立即拿到最新状态。

```msg
string state
string playback_device
string capture_device
string request_id
string uri
float32 position
float32 duration
uint32 queue_depth
uint32 underruns
bool capture_ok
string capture_error
```

`state` 的取值与含义以 `msg/AudioState.msg` 内的注释为准。

### `AudioEvent`

异步播放事件。

```msg
std_msgs/Header header
string request_id
uint8 event
string detail
```

事件类型常量的取值和含义以 `msg/AudioEvent.msg` 内的注释为准，README 不再重复维护。

## 服务

### `StopAudio`

```srv
string group
bool all
---
bool success
uint32 stopped
string message
```

### `SetVolume`

```srv
float32 volume
bool mute
---
bool success
float32 volume
bool mute
string message
```

## 动作

### `PlayClip`

```action
audio_io_interface/AudioClip clip
---
string request_id
bool success
string message
---
float32 position
float32 progress
```

## C++ 使用

```cpp
#include "audio_io_interface/ros_msg.hpp"

// audio_io 节点自己创建 endpoint 时使用私有名称。
const auto private_play = audio_io_interface::kPrivateTopicPlay;

// 外部节点必须给出目标 audio_io 的节点全名。
constexpr std::string_view audio_io_fqn = "/robot_a/audio/io";
const auto play_topic = audio_io_interface::topic_play(audio_io_fqn);
const auto stop_service = audio_io_interface::service_stop(audio_io_fqn);
const auto play_action = audio_io_interface::action_play_clip(audio_io_fqn);
```

`ros_msg.hpp` 同时导出以下类型别名：

```cpp
audio_io_interface::MsgAudioClip;
audio_io_interface::MsgAudioStream;
audio_io_interface::MsgAudioState;
audio_io_interface::MsgAudioEvent;
audio_io_interface::SrvStopAudio;
audio_io_interface::SrvSetVolume;
audio_io_interface::ActionPlayClip;
audio_io_interface::ActionPlayClipGoal;
audio_io_interface::ActionPlayClipResult;
audio_io_interface::ActionPlayClipFeedback;
```

## Python 使用

```python
from audio_io_interface_msg.ros_msg import (
    PRIVATE_TOPIC_PLAY,
    action_play_clip,
    service_stop,
    topic_play,
)

audio_io_fqn = "/robot_a/audio/io"
print(PRIVATE_TOPIC_PLAY)          # ~/play
print(topic_play(audio_io_fqn))    # /robot_a/audio/io/play
print(service_stop(audio_io_fqn))  # /robot_a/audio/io/stop
print(action_play_clip(audio_io_fqn))
```

外部 helper 会拒绝空、相对、根路径或包含 ASCII 空白的节点名。`PRIVATE_*` 常量仅供 `audio_io` endpoint owner 使用。

## 构建与测试

```bash
source /opt/ros/humble/setup.bash

colcon build --base-paths src --packages-select audio_io_interface
colcon test --base-paths src --packages-select audio_io_interface
colcon test-result --test-result-base build/audio_io_interface/test_results --verbose
```

接口依赖：

- `builtin_interfaces`
- `std_msgs`
- `rosidl_default_generators` / `rosidl_default_runtime`

## 与设计文档的对应

本包对应 [`docs/microduck-audio-architecture.zh-CN.md`](../../docs/microduck-audio-architecture.zh-CN.md) 中 `/audio/io` 的通用接口：

| 文档中的接口 | 本包中的类型 |
| --- | --- |
| `AudioClip` | `msg/AudioClip.msg` |
| `AudioStream` | `msg/AudioStream.msg` |
| `AudioState` | `msg/AudioState.msg` |
| `AudioEvent` | `msg/AudioEvent.msg` |
| `StopAudio` | `srv/StopAudio.srv` |
| `SetVolume` | `srv/SetVolume.srv` |
| `PlayClip` | `action/PlayClip.action` |

TTS 的 `TtsText` / `Synthesize` / `Say` 与 duck_audio 的 `PlayVoice` / `PettingEvent` 不在本包中，避免把语音语义耦合进通用音频 I/O。
