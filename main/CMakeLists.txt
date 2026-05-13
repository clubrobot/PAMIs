idf_component_register(SRCS "main.cpp" "Wheeledbase.cpp" "wheeledbase_.cpp" "sensors.cpp"
                       INCLUDE_DIRS "include"
        REQUIRES differential_controller odometry position_controller velocity_controller teleplot  mathutils  purepursuit  pid turn_on_the_spot logger motors)
