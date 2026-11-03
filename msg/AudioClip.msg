# 一段可播放的音频片段。
# uri 支持 file://、package://；http(s):// 是否可用由 audio_io 配置决定。

# 音频焦点优先级，数值越大越优先。
# 空闲：最低优先级，通常只用于测试或占位。
uint8 PRIORITY_IDLE=0
# 环境音：背景氛围、低优先级提示。
uint8 PRIORITY_AMBIENT=1
# 语音：TTS 与语音播报。
uint8 PRIORITY_SPEECH=2
# 交互音效：按键、动作反馈等。
uint8 PRIORITY_INTERACTION=3
# 警报：最高优先级，可抢占其他播放源。
uint8 PRIORITY_ALERT=4

# 与当前播放源冲突时的处理策略。
# 排队：等待当前播放源结束后再播放。
uint8 POLICY_QUEUE=0
# 抢占：停止低优先级或同优先级的播放源，立即播放。
uint8 POLICY_PREEMPT=1
# 混音：与当前播放源同时播放，并按优先级 ducking。
uint8 POLICY_MIX=2
# 替换同组：只停止同一 group 的播放源，再播放。
uint8 POLICY_REPLACE_SAME_GROUP=3

# 时间戳与 frame_id；frame_id 可留空。
std_msgs/Header header

# 调用方生成的请求标识，用于状态与事件关联；为空时由 audio_io 生成。
string request_id

# 音频资源地址，例如 file:///var/lib/robot/sounds/chirp/chirp_a.wav。
string uri

# 音频格式提示；为空时按扩展名/内容自动识别。
string format

# 线性增益，1.0 为原始音量。
float32 gain

# 是否循环播放。
bool loop

# 音频焦点优先级，取 PRIORITY_* 常量。
uint8 priority

# 冲突处理策略，取 POLICY_* 常量。
uint8 policy

# 音频焦点组，例如 voice、tts、music。
string group

# 从资源起始位置偏移多少秒开始播放。
float64 start_offset

# 播放时长，单位秒；0 表示播放到结尾。
float64 duration
