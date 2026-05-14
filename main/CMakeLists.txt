idf_component_register(SRCS "main.cpp" "wheeledbase.cpp" "sensors.cpp"
                       INCLUDE_DIRS "include"
        REQUIRES differential_controller odometry wheeledbase position_controller velocity_controller teleplot  mathutils  purepursuit  pid turn_on_the_spot logger motors)
