# 一段 PCM 音频流。
# 首包开始播放；eos 为 true 时表示流结束。若超过配置的超时时间没有新包，
# audio_io 会淡出并释放该流。

# 时间戳与 frame_id。
std_msgs/Header header

# 流标识；同一路流的所有包必须保持一致。
string stream_id

# PCM 编码，当前支持 s16le 与 f32le。
string encoding

# 采样率，单位 Hz。
uint32 sample_rate

# 声道数；1 为单声道，2 为立体声。
uint8 channels

# 包序号，从 0 开始递增，用于检测丢包与乱序。
uint64 sequence

# 是否为最后一包。
bool eos

# 交错排列的 PCM 数据，字节序为小端。
uint8[] data
