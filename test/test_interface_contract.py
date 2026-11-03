"""验证 audio_io_interface 的接口字段与端点命名契约。"""

import importlib.util
from pathlib import Path

import pytest

PACKAGE_ROOT = Path(__file__).resolve().parents[1]


def lines(relative: str) -> list[str]:
    """返回接口文件中去掉空行、注释与分隔符后的内容行。"""
    return [
        line.strip()
        for line in (PACKAGE_ROOT / relative).read_text(encoding="utf-8").splitlines()
        if line.strip() and not line.lstrip().startswith("#") and line.strip() != "---"
    ]


def fields(relative: str) -> list[str]:
    """返回接口文件中的字段行，排除常量定义。"""
    return [line for line in lines(relative) if "=" not in line]


def action_sections(relative: str) -> list[list[str]]:
    """返回 action 文件的 goal/result/feedback 三段字段。"""
    sections: list[list[str]] = [[]]
    for line in (PACKAGE_ROOT / relative).read_text(encoding="utf-8").splitlines():
        stripped = line.strip()
        if not stripped or stripped.startswith("#"):
            continue
        if stripped == "---":
            sections.append([])
            continue
        if "=" not in stripped:
            sections[-1].append(stripped)
    return sections


def load_ros_msg():
    """直接加载 Python 辅助模块，避免依赖安装后的生成包。"""
    module_path = PACKAGE_ROOT / "python/audio_io_interface_msg/ros_msg.py"
    spec = importlib.util.spec_from_file_location("audio_io_interface_ros_msg", module_path)
    assert spec is not None and spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def test_audio_clip_contract() -> None:
    """AudioClip 必须保留播放片段所需的全部字段与优先级常量。"""
    assert fields("msg/AudioClip.msg") == [
        "std_msgs/Header header",
        "string request_id",
        "string uri",
        "string format",
        "float32 gain",
        "bool loop",
        "uint8 priority",
        "uint8 policy",
        "string group",
        "float64 start_offset",
        "float64 duration",
    ]
    content = "\n".join(lines("msg/AudioClip.msg"))
    assert "uint8 PRIORITY_ALERT=4" in content
    assert "uint8 POLICY_PREEMPT=1" in content
    assert "uint8 POLICY_REPLACE_SAME_GROUP=3" in content


def test_audio_stream_contract() -> None:
    """AudioStream 必须能表达 PCM 格式、包序与流结束。"""
    assert fields("msg/AudioStream.msg") == [
        "std_msgs/Header header",
        "string stream_id",
        "string encoding",
        "uint32 sample_rate",
        "uint8 channels",
        "uint64 sequence",
        "bool eos",
        "uint8[] data",
    ]


def test_audio_state_and_event_contract() -> None:
    """AudioState 与 AudioEvent 必须覆盖播放、采集、欠载和设备事件。"""
    assert fields("msg/AudioState.msg") == [
        "string state",
        "string playback_device",
        "string capture_device",
        "string request_id",
        "string uri",
        "float32 position",
        "float32 duration",
        "uint32 queue_depth",
        "uint32 underruns",
        "bool capture_ok",
        "string capture_error",
    ]
    assert fields("msg/AudioEvent.msg") == [
        "std_msgs/Header header",
        "string request_id",
        "uint8 event",
        "string detail",
    ]
    content = "\n".join(lines("msg/AudioEvent.msg"))
    assert "uint8 EVENT_PREEMPTED=3" in content
    assert "uint8 EVENT_DEVICE_RECOVERED=7" in content


def test_service_and_action_contract() -> None:
    """服务与动作的请求/响应字段必须与文档一致。"""
    assert fields("srv/StopAudio.srv") == [
        "string group",
        "bool all",
        "bool success",
        "uint32 stopped",
        "string message",
    ]
    assert fields("srv/SetVolume.srv") == [
        "float32 volume",
        "bool mute",
        "bool success",
        "float32 volume",
        "bool mute",
        "string message",
    ]
    goal, result, feedback = action_sections("action/PlayClip.action")
    assert goal == ["audio_io_interface/AudioClip clip"]
    assert result == ["string request_id", "bool success", "string message"]
    assert feedback == ["float32 position", "float32 progress"]


def test_python_endpoint_helpers() -> None:
    """Python helper 必须区分私有名称与目标节点 FQN。"""
    ros_msg = load_ros_msg()

    assert ros_msg.DEFAULT_NODE_FQN == "/audio_io"
    assert ros_msg.PRIVATE_TOPIC_PLAY == "~/play"
    assert ros_msg.PRIVATE_TOPIC_STREAM == "~/stream"
    assert ros_msg.PRIVATE_TOPIC_CAPTURE == "~/capture"
    assert ros_msg.PRIVATE_TOPIC_STATE == "~/state"
    assert ros_msg.PRIVATE_TOPIC_EVENT == "~/event"
    assert ros_msg.PRIVATE_ACTION_PLAY_CLIP == "~/play_clip"
    assert ros_msg.PRIVATE_SERVICE_STOP == "~/stop"
    assert ros_msg.PRIVATE_SERVICE_SET_VOLUME == "~/set_volume"

    assert ros_msg.topic_play() == "/audio_io/play"
    assert ros_msg.topic_stream() == "/audio_io/stream"
    assert ros_msg.topic_capture() == "/audio_io/capture"
    assert ros_msg.topic_state() == "/audio_io/state"
    assert ros_msg.topic_event() == "/audio_io/event"
    assert ros_msg.action_play_clip() == "/audio_io/play_clip"
    assert ros_msg.service_stop() == "/audio_io/stop"
    assert ros_msg.service_set_volume() == "/audio_io/set_volume"

    assert ros_msg.topic_play("/robot_a/audio_io") == "/robot_a/audio_io/play"
    assert ros_msg.topic_play("/robot_a/audio_io///") == "/robot_a/audio_io/play"


def test_python_endpoint_helpers_reject_invalid_node_fqn() -> None:
    """Python helper 必须拒绝空、相对、根路径与含空白的节点名。"""
    ros_msg = load_ros_msg()
    for invalid in ("", "audio/io", "/", "/audio//io", "/audio_io\t"):
        with pytest.raises(ValueError, match="node_fqn"):
            ros_msg.topic_play(invalid)


def test_helper_files_expose_consistent_private_names() -> None:
    """C++ 与 Python helper 必须使用同一套私有 endpoint 名称。"""
    helper_files = (
        "include/audio_io_interface/ros_msg.hpp",
        "python/audio_io_interface_msg/ros_msg.py",
    )
    required = (
        "~/play",
        "~/stream",
        "~/capture",
        "~/state",
        "~/event",
        "~/play_clip",
        "~/stop",
        "~/set_volume",
        "topic_play",
        "action_play_clip",
        "service_set_volume",
    )
    for relative in helper_files:
        content = (PACKAGE_ROOT / relative).read_text(encoding="utf-8")
        assert all(item in content for item in required), relative
