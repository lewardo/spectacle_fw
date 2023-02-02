# **spectacle** firmware for esp32

firmware for the spectacle smart glasses version 1  
component framework for module and app-like development on the esp32

requirements:
* esp idf v4.4+
* cmake
* ninja

targets:
* esp32

## DONE:

* system component mechanism to register system functions, registering and initialisation, event subscription and broadcast service
* touch component for capacitive touch input handling
* system sleep api to sleep and shut down


## TODO:

* bluetooth component for wireless
* network component for wifi and internet applications
* framebuffer component to use oled displays with different drivers
* system driver components to provide universal api to communicate with devices
* app system to build on components and provide OS like U\[IX\]


