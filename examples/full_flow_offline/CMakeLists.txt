ameba_internal_library(example_full_flow_offline)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_full_flow_offline.c
    platform/ameba_rtos/app_example.c
    # platform/ameba_rtos/testwav_1c.c
    testwav_3c.c
)

target_include_directories(
    ${CURRENT_LIB_NAME} PRIVATE

    ${BASEDIR}/component/utils/ringbuffer
)

target_compile_options(
    ${CURRENT_LIB_NAME} PRIVATE
    -Wno-undef
    -Wno-unused-parameter
)

target_compile_definitions(
    ${CURRENT_LIB_NAME} PRIVATE
    TF_LITE_STATIC_MEMORY
)

