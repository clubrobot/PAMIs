idf_component_register(
        SRCS Odometry.cpp Codewheel.cpp
        INCLUDE_DIRS include
        REQUIRES freertos esp_timer esp_driver_pcnt
)