# audio_io 的播放与采集状态快照。
# 该消息通过 transient_local QoS 发布，新订阅者可以立即拿到最新状态。

# 播放状态：idle、playing、degraded 或 error。
string state

# 播放设备名称，例如 plughw:aic3104。
string playback_device

# 采集设备名称；未启用采集时为空。
string capture_device

# 当前正在播放的请求标识。
string request_id

# 当前正在播放的资源地址。
string uri

# 当前播放位置，单位秒。
float32 position

# 当前资源总时长，单位秒；未知时为 0。
float32 duration

# 等待播放的请求数量。
uint32 queue_depth

# 累计欠载次数。
uint32 underruns

# 采集侧是否正常。
bool capture_ok

# 采集侧最近一次错误；正常时为空。
string capture_error
