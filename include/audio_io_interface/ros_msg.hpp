#pragma once

#include <algorithm>
#include <stdexcept>
#include <string>
#include <string_view>

#include "audio_io_interface/action/play_clip.hpp"
#include "audio_io_interface/msg/audio_clip.hpp"
#include "audio_io_interface/msg/audio_event.hpp"
#include "audio_io_interface/msg/audio_state.hpp"
#include "audio_io_interface/msg/audio_stream.hpp"
#include "audio_io_interface/srv/set_volume.hpp"
#include "audio_io_interface/srv/stop_audio.hpp"

namespace audio_io_interface {

    namespace detail {

        /// 判断字符是否为 ROS 名称中不允许出现的 ASCII 空白。
        [[nodiscard]] constexpr bool is_ascii_whitespace(const char character) noexcept {
            return character == ' ' || (character >= '\t' && character <= '\r');
        }

        /// 判断字符串是否为空或只包含 ASCII 空白。
        [[nodiscard]] inline bool is_empty_or_ascii_whitespace(const std::string_view value) noexcept {
            return value.empty() || std::ranges::all_of(value, is_ascii_whitespace);
        }

        /// 默认 audio_io 节点全名。
        inline constexpr std::string_view kDefaultNodeFqn = "/audio_io";

        /// 去掉尾部多余的 '/'，保留开头的绝对路径。
        [[nodiscard]] inline std::string_view trim_trailing_slashes(std::string_view node_fqn) {
            while (node_fqn.size() > 1 && node_fqn.back() == '/') {
                node_fqn.remove_suffix(1);
            }
            return node_fqn;
        }

        /// 校验节点全名：必须是非根绝对名称，不能包含 ASCII 空白或中间的空路径段。
        inline void require_node_fqn(const std::string_view raw_node_fqn) {
            if (const auto node_fqn = trim_trailing_slashes(raw_node_fqn);
                node_fqn.empty() || node_fqn.front() != '/' || node_fqn == "/" ||
                node_fqn.find("//") != std::string_view::npos ||
                node_fqn.find_first_of(" \t\n\r\v\f") != std::string_view::npos) {
                throw std::invalid_argument(
                    "node_fqn must be an absolute, non-root ROS name without ASCII whitespace");
            }
        }

        /// 校验并返回去掉尾部 '/' 的节点全名。
        [[nodiscard]] inline std::string normalized_node_fqn(const std::string_view node_fqn) {
            require_node_fqn(node_fqn);
            return std::string{trim_trailing_slashes(node_fqn)};
        }

        /// 拼接节点全名与 endpoint 后缀。
        [[nodiscard]] inline std::string endpoint_name(const std::string_view node_fqn,
                                                       const std::string_view endpoint_suffix) {
            return normalized_node_fqn(node_fqn) + "/" + std::string{endpoint_suffix};
        }

    } // namespace detail

    /// 默认 audio_io 节点全名。
    inline constexpr std::string_view kDefaultNodeFqn = detail::kDefaultNodeFqn;

    /// 把默认节点全名限定到当前节点的 namespace。
    ///
    /// - `qualify_node_fqn()` → `/audio_io`
    /// - `qualify_node_fqn(kDefaultNodeFqn, "/robot1")` → `/robot1/audio_io`
    /// - `qualify_node_fqn("/audio/io", "/robot1")` → `/robot1/audio/io`
    ///
    /// 计算出的 FQN 可以直接传给 `topic_capture()`、`topic_event()` 等 helper。
    /// 根 namespace 与多段默认名称都支持；非法 namespace/默认名抛出
    /// std::invalid_argument。
    [[nodiscard]] inline std::string qualify_node_fqn(
            const std::string_view default_node_fqn = detail::kDefaultNodeFqn,
            const std::string_view node_namespace = "/") {
        const auto base = detail::trim_trailing_slashes(default_node_fqn);
        detail::require_node_fqn(base);

        auto ns = detail::trim_trailing_slashes(node_namespace);
        if (ns.empty()) {
            ns = "/";
        }
        if (ns != "/") {
            detail::require_node_fqn(ns);
        }
        if (ns == "/") {
            return std::string{base};
        }
        return detail::normalized_node_fqn(std::string{ns} + std::string{base});
    }

    /// endpoint owner（audio_io 节点自身）创建 publisher/subscriber/service/action 时使用的私有名称。
    inline constexpr std::string_view kPrivateTopicPlay = "~/play";
    inline constexpr std::string_view kPrivateTopicStream = "~/stream";
    inline constexpr std::string_view kPrivateTopicCapture = "~/capture";
    inline constexpr std::string_view kPrivateTopicState = "~/state";
    inline constexpr std::string_view kPrivateTopicEvent = "~/event";
    inline constexpr std::string_view kPrivateActionPlayClip = "~/play_clip";
    inline constexpr std::string_view kPrivateServiceStop = "~/stop";
    inline constexpr std::string_view kPrivateServiceSetVolume = "~/set_volume";

    /// 外部消费者使用的绝对话题名称。
    ///
    /// @param node_fqn audio_io 在根 namespace 下的节点全名，默认 /audio_io
    /// @param node_namespace 调用方节点所在 namespace；默认 "/"。
    ///        同一 namespace 下可直接传 node->get_namespace()。
    ///        如果 node_fqn 已经是完整 FQN，保持 node_namespace="/" 即可。
    [[nodiscard]] inline std::string topic_play(const std::string_view node_fqn = detail::kDefaultNodeFqn,
                                                const std::string_view node_namespace = "/") {
        return detail::endpoint_name(qualify_node_fqn(node_fqn, node_namespace), "play");
    }

    [[nodiscard]] inline std::string topic_stream(const std::string_view node_fqn = detail::kDefaultNodeFqn,
                                                  const std::string_view node_namespace = "/") {
        return detail::endpoint_name(qualify_node_fqn(node_fqn, node_namespace), "stream");
    }

    [[nodiscard]] inline std::string topic_capture(const std::string_view node_fqn = detail::kDefaultNodeFqn,
                                                   const std::string_view node_namespace = "/") {
        return detail::endpoint_name(qualify_node_fqn(node_fqn, node_namespace), "capture");
    }

    [[nodiscard]] inline std::string topic_state(const std::string_view node_fqn = detail::kDefaultNodeFqn,
                                                 const std::string_view node_namespace = "/") {
        return detail::endpoint_name(qualify_node_fqn(node_fqn, node_namespace), "state");
    }

    [[nodiscard]] inline std::string topic_event(const std::string_view node_fqn = detail::kDefaultNodeFqn,
                                                 const std::string_view node_namespace = "/") {
        return detail::endpoint_name(qualify_node_fqn(node_fqn, node_namespace), "event");
    }

    /// 外部消费者使用的绝对动作名称。
    [[nodiscard]] inline std::string action_play_clip(const std::string_view node_fqn = detail::kDefaultNodeFqn,
                                                      const std::string_view node_namespace = "/") {
        return detail::endpoint_name(qualify_node_fqn(node_fqn, node_namespace), "play_clip");
    }

    /// 外部消费者使用的绝对服务名称。
    [[nodiscard]] inline std::string service_stop(const std::string_view node_fqn = detail::kDefaultNodeFqn,
                                                  const std::string_view node_namespace = "/") {
        return detail::endpoint_name(qualify_node_fqn(node_fqn, node_namespace), "stop");
    }

    [[nodiscard]] inline std::string service_set_volume(const std::string_view node_fqn = detail::kDefaultNodeFqn,
                                                        const std::string_view node_namespace = "/") {
        return detail::endpoint_name(qualify_node_fqn(node_fqn, node_namespace), "set_volume");
    }

    // 与生成消息/服务/动作名称一致的简洁类型别名。
    using MsgAudioClip = audio_io_interface::msg::AudioClip;
    using MsgAudioStream = audio_io_interface::msg::AudioStream;
    using MsgAudioState = audio_io_interface::msg::AudioState;
    using MsgAudioEvent = audio_io_interface::msg::AudioEvent;
    using SrvStopAudio = audio_io_interface::srv::StopAudio;
    using SrvSetVolume = audio_io_interface::srv::SetVolume;
    using ActionPlayClip = audio_io_interface::action::PlayClip;
    using ActionPlayClipGoal = ActionPlayClip::Goal;
    using ActionPlayClipResult = ActionPlayClip::Result;
    using ActionPlayClipFeedback = ActionPlayClip::Feedback;

} // namespace audio_io_interface
