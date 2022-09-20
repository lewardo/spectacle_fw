idf_component_register (
    SRCS "src/bluetooth.c" "src/connection.c" "src/controller.c" "src/profile.c"
    INCLUDE_DIRS "include"
    REQUIRES global system
)