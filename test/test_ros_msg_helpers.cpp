#include <stdexcept>
#include <type_traits>

#include <gtest/gtest.h>

#include "audio_io_interface/ros_msg.hpp"

namespace {

constexpr std::string_view kRobotNodeFqn = "/robot_a/audio/io";

TEST(RosMsgHelpers, ProvidesPrivateEndpointOwnerNames) {
    EXPECT_EQ(audio_io_interface::kPrivateTopicPlay, "~/play");
    EXPECT_EQ(audio_io_interface::kPrivateTopicStream, "~/stream");
    EXPECT_EQ(audio_io_interface::kPrivateTopicCapture, "~/capture");
    EXPECT_EQ(audio_io_interface::kPrivateTopicState, "~/state");
    EXPECT_EQ(audio_io_interface::kPrivateTopicEvent, "~/event");
    EXPECT_EQ(audio_io_interface::kPrivateActionPlayClip, "~/play_clip");
    EXPECT_EQ(audio_io_interface::kPrivateServiceStop, "~/stop");
    EXPECT_EQ(audio_io_interface::kPrivateServiceSetVolume, "~/set_volume");
}

TEST(RosMsgHelpers, BuildsDefaultAbsoluteEndpointNames) {
    EXPECT_EQ(audio_io_interface::topic_play(), "/audio/io/play");
    EXPECT_EQ(audio_io_interface::topic_stream(), "/audio/io/stream");
    EXPECT_EQ(audio_io_interface::topic_capture(), "/audio/io/capture");
    EXPECT_EQ(audio_io_interface::topic_state(), "/audio/io/state");
    EXPECT_EQ(audio_io_interface::topic_event(), "/audio/io/event");
    EXPECT_EQ(audio_io_interface::action_play_clip(), "/audio/io/play_clip");
    EXPECT_EQ(audio_io_interface::service_stop(), "/audio/io/stop");
    EXPECT_EQ(audio_io_interface::service_set_volume(), "/audio/io/set_volume");
}

TEST(RosMsgHelpers, BuildsNamesForCustomNodeFqn) {
    EXPECT_EQ(audio_io_interface::topic_play(kRobotNodeFqn),
              "/robot_a/audio/io/play");
    EXPECT_EQ(audio_io_interface::topic_stream(kRobotNodeFqn),
              "/robot_a/audio/io/stream");
    EXPECT_EQ(audio_io_interface::action_play_clip(kRobotNodeFqn),
              "/robot_a/audio/io/play_clip");
    EXPECT_EQ(audio_io_interface::service_set_volume(kRobotNodeFqn),
              "/robot_a/audio/io/set_volume");
    EXPECT_EQ(audio_io_interface::topic_play(std::string{kRobotNodeFqn} + "///"),
              "/robot_a/audio/io/play");
}

TEST(RosMsgHelpers, RejectsInvalidNodeFqn) {
    EXPECT_THROW(static_cast<void>(audio_io_interface::topic_play("")),
                 std::invalid_argument);
    EXPECT_THROW(static_cast<void>(audio_io_interface::topic_play("audio/io")),
                 std::invalid_argument);
    EXPECT_THROW(static_cast<void>(audio_io_interface::topic_play("/")),
                 std::invalid_argument);
    EXPECT_THROW(static_cast<void>(audio_io_interface::topic_play("/audio//io")),
                 std::invalid_argument);
    EXPECT_THROW(static_cast<void>(audio_io_interface::topic_play("/audio/io\t")),
                 std::invalid_argument);
}

TEST(RosMsgHelpers, ExposesGeneratedTypeAliases) {
    static_assert(std::is_same_v<audio_io_interface::MsgAudioClip,
                                 audio_io_interface::msg::AudioClip>);
    static_assert(std::is_same_v<audio_io_interface::MsgAudioStream,
                                 audio_io_interface::msg::AudioStream>);
    static_assert(std::is_same_v<audio_io_interface::MsgAudioState,
                                 audio_io_interface::msg::AudioState>);
    static_assert(std::is_same_v<audio_io_interface::MsgAudioEvent,
                                 audio_io_interface::msg::AudioEvent>);
    static_assert(std::is_same_v<audio_io_interface::SrvStopAudio,
                                 audio_io_interface::srv::StopAudio>);
    static_assert(std::is_same_v<audio_io_interface::SrvSetVolume,
                                 audio_io_interface::srv::SetVolume>);
    static_assert(std::is_same_v<audio_io_interface::ActionPlayClip,
                                 audio_io_interface::action::PlayClip>);
    static_assert(std::is_same_v<audio_io_interface::ActionPlayClipGoal,
                                 audio_io_interface::ActionPlayClip::Goal>);
    static_assert(std::is_same_v<audio_io_interface::ActionPlayClipResult,
                                 audio_io_interface::ActionPlayClip::Result>);
    static_assert(std::is_same_v<audio_io_interface::ActionPlayClipFeedback,
                                 audio_io_interface::ActionPlayClip::Feedback>);
}

} // namespace
