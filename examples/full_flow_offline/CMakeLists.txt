ameba_internal_library(example_full_flow_offline)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_full_flow_offline.c
    platform/ameba_rtos/app_example.c
)

if(CONFIG_AMEBASMART)
    target_sources(
        ${CURRENT_LIB_NAME} PRIVATE
        testwav_3c.c
    )
elseif(CONFIG_AMEBADPLUS OR CONFIG_AMEBAGREEN2)
    target_sources(
        ${CURRENT_LIB_NAME} PRIVATE
        platform/ameba_rtos/testwav_2c.c
    )
endif()

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
    USE_BINARY_RESOURCE=0
)

if(CONFIG_AMEBASMART)
    target_compile_definitions(
        ${CURRENT_LIB_NAME} PRIVATE
        AIVOICE_TARGET_LOW_RESOURCE=0
    )
elseif(CONFIG_AMEBADPLUS OR CONFIG_AMEBAGREEN2)
    target_compile_definitions(
        ${CURRENT_LIB_NAME} PRIVATE
        AIVOICE_TARGET_LOW_RESOURCE=1
    )
endif()
