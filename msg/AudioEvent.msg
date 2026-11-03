# audio_io 的播放事件。
# 事件用于异步通知开始、结束、失败、抢占、欠载与设备变化。

# 事件类型常量。
# 播放源开始播放。
uint8 EVENT_STARTED=0
# 播放源正常播放完成。
uint8 EVENT_FINISHED=1
# 播放源失败，原因见 detail。
uint8 EVENT_FAILED=2
# 播放源被更高优先级请求抢占。
uint8 EVENT_PREEMPTED=3
# 音频输出欠载，已补静音。
uint8 EVENT_UNDERRUN=4
# 音频文件或流解码失败。
uint8 EVENT_DECODE_FAILED=5
# 音频设备丢失，例如 USB 声卡被拔出。
uint8 EVENT_DEVICE_LOST=6
# 音频设备恢复，重新打开成功。
uint8 EVENT_DEVICE_RECOVERED=7

# 事件时间戳。
std_msgs/Header header

# 关联的请求标识；设备级事件可以为空。
string request_id

# 事件类型，取 EVENT_* 常量。
uint8 event

# 可读的补充说明，例如错误原因或设备名。
string detail
