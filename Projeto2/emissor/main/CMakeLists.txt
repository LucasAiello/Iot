idf_component_register(SRCS "wifi.c" "mqtt.c" "projeto_emissor.c"
                    INCLUDE_DIRS "."
                    REQUIRES espressif__mqtt nvs_flash esp_wifi driver esp_driver_gpio
                    )
