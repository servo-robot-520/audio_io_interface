# audio_io 的播放事件。
# 事件用于异步通知开始、结束、失败、抢占、欠载与设备变化。

# 事件类型常量。
uint8 EVENT_STARTED=0
uint8 EVENT_FINISHED=1
uint8 EVENT_FAILED=2
uint8 EVENT_PREEMPTED=3
uint8 EVENT_UNDERRUN=4
uint8 EVENT_DECODE_FAILED=5
uint8 EVENT_DEVICE_LOST=6
uint8 EVENT_DEVICE_RECOVERED=7

# 事件时间戳。
std_msgs/Header header

# 关联的请求标识；设备级事件可以为空。
string request_id

# 事件类型，取 EVENT_* 常量。
uint8 event

# 可读的补充说明，例如错误原因或设备名。
string detail
