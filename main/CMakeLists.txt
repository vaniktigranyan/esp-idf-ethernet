set(COMPONENT_SRCS "main.c" "ethernet.c" "http_server.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")


register_component()

spiffs_create_partition_image(storage ${CMAKE_CURRENT_SOURCE_DIR}/spiffs FLASH_IN_PROJECT)

