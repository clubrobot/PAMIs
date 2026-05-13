idf_component_register(SRCS "iot_servo.c"
                        INCLUDE_DIRS include
                        PRIV_REQUIRES esp_driver_ledc
                        REQUIRES driver esp_driver_gpio)
