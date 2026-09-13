idf_component_register(SRCS "main.c"
                    REQUIRES ds18x20 onewire_bus sensor_hub
                    INCLUDE_DIRS ".")
