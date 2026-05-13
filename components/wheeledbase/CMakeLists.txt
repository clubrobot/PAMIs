idf_component_register(
        SRCS Wheeledbase.cpp
        INCLUDE_DIRS include
        REQUIRES mathutils position_controller purepursuit velocity_controller pid turn_on_the_spot logger
)