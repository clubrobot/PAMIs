idf_component_register(
        SRCS VelocityController.cpp
        INCLUDE_DIRS include
        REQUIRES freertos esp_timer motors pid differential_controller mathutils
)