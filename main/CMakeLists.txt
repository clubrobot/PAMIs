idf_component_register(SRCS "main.cpp" "wheeledbase.cpp" "sensors.cpp"
                       INCLUDE_DIRS "include"
        REQUIRES differential_controller odometry position_controller velocity_controller teleplot)
