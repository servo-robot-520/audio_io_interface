# 一段可播放的音频片段。
# uri 支持 file://、package://；http(s):// 是否可用由 audio_io 配置决定。

# 音频焦点优先级，数值越大越优先。
uint8 PRIORITY_IDLE=0
uint8 PRIORITY_AMBIENT=1
uint8 PRIORITY_SPEECH=2
uint8 PRIORITY_INTERACTION=3
uint8 PRIORITY_ALERT=4

# 与当前播放源冲突时的处理策略。
uint8 POLICY_QUEUE=0
uint8 POLICY_PREEMPT=1
uint8 POLICY_MIX=2
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
