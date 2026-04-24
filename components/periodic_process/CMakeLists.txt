idf_component_register(
        SRCS PeriodicProcess.cpp
        INCLUDE_DIRS include
        REQUIRES freertos esp_timer clock
)