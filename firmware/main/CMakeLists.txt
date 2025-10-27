idf_component_register(SRCS "main.S"
                      INCLUDE_DIRS ".")
set(SOURCES main.c firmware.S)
target_compile_options(${COMPONENT_LIB} PRIVATE -mlongcalls -mabi=call0)
