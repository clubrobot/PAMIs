idf_component_register(
        SRCS Teleplot.cpp
        INCLUDE_DIRS include
        REQUIRES lwip esp_wifi esp_event nvs_flash esp_timer
)