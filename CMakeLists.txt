cmake_minimum_required(VERSION 3.22)
set(PROJECT_VERSION 0.1.0)
project(audio_io_interface VERSION ${PROJECT_VERSION})

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif ()

find_package(ament_cmake REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(std_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
        "msg/AudioClip.msg"
        "msg/AudioStream.msg"
        "msg/AudioState.msg"
        "msg/AudioEvent.msg"
        "srv/StopAudio.srv"
        "srv/SetVolume.srv"
        "action/PlayClip.action"
        DEPENDENCIES
        builtin_interfaces
        std_msgs
)

install(DIRECTORY include/${PROJECT_NAME}/
        DESTINATION include/${PROJECT_NAME}/${PROJECT_NAME}
)
ament_export_include_directories(include/${PROJECT_NAME})

install(DIRECTORY python/${PROJECT_NAME}_msg/
        DESTINATION ${PYTHON_INSTALL_DIR}/${PROJECT_NAME}_msg
        PATTERN "__pycache__" EXCLUDE
        PATTERN "*.pyc" EXCLUDE
)

if (BUILD_TESTING)
    find_package(ament_cmake_gtest REQUIRED)
    find_package(ament_cmake_pytest REQUIRED)
    ament_add_gtest(test_audio_io_ros_msg_helpers test/test_ros_msg_helpers.cpp)
    # ros_msg.hpp 使用 std::ranges（C++20），而本包没有全局设置语言标准。
    target_compile_features(test_audio_io_ros_msg_helpers PRIVATE cxx_std_20)
    target_include_directories(test_audio_io_ros_msg_helpers PRIVATE include)
    target_link_libraries(test_audio_io_ros_msg_helpers
            ${PROJECT_NAME}__rosidl_typesupport_cpp
    )
    ament_add_pytest_test(test_audio_io_interface_contract test/test_interface_contract.py)
endif ()

ament_package()
