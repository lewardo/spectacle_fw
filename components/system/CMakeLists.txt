idf_component_register (
    SRCS "src/component.c" "src/sleep.c" "src/system.c"
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "local"
    REQUIRES global
)