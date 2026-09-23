idf_component_register(
    SRCS "st7789.c"
    INCLUDE_DIRS "include"
    REQUIRES lvgl esp_lvgl_port  # <-- Isso diz ao ESP-IDF que seu componente usa o LVGL
)
