
idf_component_register(
    SRCS "main.c"                     # <-- Certifique-se de colocar o seu arquivo fonte aqui
    INCLUDE_DIRS "."
    REQUIRES esp_driver_gpio ds18x20  # <-- Os componentes necessários
)
