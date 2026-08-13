file(GLOB_RECURSE SRC_FILES "*.c")
idf_component_register(SRCS ${SRC_FILES}
                    INCLUDE_DIRS "." "screens"
                    PRIV_INCLUDE_DIRS "ui"
                    REQUIRES "lvgl__lvgl" "freertos" "esp_bsp" "espressif__esp_lvgl_port")