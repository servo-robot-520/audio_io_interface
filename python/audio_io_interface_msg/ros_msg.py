"""audio_io_interface 的 ROS endpoint 名称辅助函数。"""

from __future__ import annotations

DEFAULT_NODE_FQN = "/audio_io"

# 下列私有名称仅供 endpoint owner（audio_io 节点自身）创建 endpoint 时使用。
PRIVATE_TOPIC_PLAY = "~/play"
PRIVATE_TOPIC_STREAM = "~/stream"
PRIVATE_TOPIC_CAPTURE = "~/capture"
PRIVATE_TOPIC_STATE = "~/state"
PRIVATE_TOPIC_EVENT = "~/event"
PRIVATE_ACTION_PLAY_CLIP = "~/play_clip"
PRIVATE_SERVICE_STOP = "~/stop"
PRIVATE_SERVICE_SET_VOLUME = "~/set_volume"

_ASCII_WHITESPACE = " \t\n\r\v\f"


def _normalized_node_fqn(node_fqn: str) -> str:
    """校验并返回去掉尾部 '/' 的 audio_io 节点全名。"""
    trimmed = node_fqn.rstrip("/")
    if (
            not trimmed
            or not trimmed.startswith("/")
            or trimmed == "/"
            or "//" in trimmed
            or any(character in _ASCII_WHITESPACE for character in trimmed)
    ):
        raise ValueError(
            "node_fqn must be an absolute, non-root ROS name without ASCII whitespace"
        )
    return trimmed


def _endpoint_name(node_fqn: str, endpoint_suffix: str) -> str:
    """拼接节点全名与 endpoint 后缀。"""
    return f"{_normalized_node_fqn(node_fqn)}/{endpoint_suffix}"


def qualify_node_fqn(node_namespace: str, default_node_fqn: str = DEFAULT_NODE_FQN) -> str:
    """把默认 audio_io 节点全名限定到当前节点的 namespace。"""
    base = _normalized_node_fqn(default_node_fqn)
    namespace = node_namespace.rstrip("/") or "/"
    if namespace != "/":
        namespace = _normalized_node_fqn(namespace)
    if namespace == "/":
        return base
    return _normalized_node_fqn(f"{namespace}{base}")


def topic_play(node_fqn: str = DEFAULT_NODE_FQN) -> str:
    """返回播放话题的绝对名称。"""
    return _endpoint_name(node_fqn, "play")


def topic_stream(node_fqn: str = DEFAULT_NODE_FQN) -> str:
    """返回 PCM 流话题的绝对名称。"""
    return _endpoint_name(node_fqn, "stream")


def topic_capture(node_fqn: str = DEFAULT_NODE_FQN) -> str:
    """返回麦克风采集话题的绝对名称。"""
    return _endpoint_name(node_fqn, "capture")


def topic_state(node_fqn: str = DEFAULT_NODE_FQN) -> str:
    """返回状态话题的绝对名称。"""
    return _endpoint_name(node_fqn, "state")


def topic_event(node_fqn: str = DEFAULT_NODE_FQN) -> str:
    """返回事件话题的绝对名称。"""
    return _endpoint_name(node_fqn, "event")


def action_play_clip(node_fqn: str = DEFAULT_NODE_FQN) -> str:
    """返回播放动作的绝对名称。"""
    return _endpoint_name(node_fqn, "play_clip")


def service_stop(node_fqn: str = DEFAULT_NODE_FQN) -> str:
    """返回停止服务的绝对名称。"""
    return _endpoint_name(node_fqn, "stop")


def service_set_volume(node_fqn: str = DEFAULT_NODE_FQN) -> str:
    """返回音量服务的绝对名称。"""
    return _endpoint_name(node_fqn, "set_volume")


__all__ = [
    "DEFAULT_NODE_FQN",
    "PRIVATE_TOPIC_PLAY",
    "PRIVATE_TOPIC_STREAM",
    "PRIVATE_TOPIC_CAPTURE",
    "PRIVATE_TOPIC_STATE",
    "PRIVATE_TOPIC_EVENT",
    "PRIVATE_ACTION_PLAY_CLIP",
    "PRIVATE_SERVICE_STOP",
    "PRIVATE_SERVICE_SET_VOLUME",
    "qualify_node_fqn",
    "topic_play",
    "topic_stream",
    "topic_capture",
    "topic_state",
    "topic_event",
    "action_play_clip",
    "service_stop",
    "service_set_volume",
]
