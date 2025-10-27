if(NOT CONFIG_AIVOICE_EN)
    return() # DO nothing if CONFIG_AIVOICE_EN is not set
endif()

##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above
ameba_list_append(public_includes
    ${c_CMPT_AIVOICE_DIR}/include
)

# Component public part, user config end
#----------------------------------------#

set(AFE_RESOURCE afe_res_2mic50mm)
if(CONFIG_AFE_RES_1MIC)
    set(AFE_RESOURCE afe_res_1mic)
elseif(CONFIG_AFE_RES_2MIC30MM)
    set(AFE_RESOURCE afe_res_2mic30mm)
elseif(CONFIG_AFE_RES_2MIC50MM)
    set(AFE_RESOURCE afe_res_2mic50mm)
elseif(CONFIG_AFE_RES_2MIC70MM)
    set(AFE_RESOURCE afe_res_2mic70mm)
elseif(CONFIG_AFE_RES_CIRCLE3MIC50MM)
    set(AFE_RESOURCE afe_res_circle3mic50mm)
endif()

set(KWS_RESOURCE kws_xiaoqiangxiaoqiang_nihaoxiaoqiang_v4_300K)
if(CONFIG_KWS_RES_CUSTOM)
    set(KWS_RESOURCE kws_cn_customized_v3_750K)
endif()

if(CONFIG_AMEBADPLUS)
    ameba_list_append(public_libraries
        ${c_CMPT_AIVOICE_DIR}/prebuilts/ameba_rtos/rtl8721dx_km4/libaivoice.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/ameba_rtos/rtl8721dx_km4/libafe_kernel.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/ameba_rtos/rtl8721dx_km4/libafe_res_1mic.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/ameba_rtos/rtl8721dx_km4/libkernel.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/ameba_rtos/rtl8721dx_km4/libkws_xiaoqiangxiaoqiang_nihaoxiaoqiang_v4_300K.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/ameba_rtos/rtl8721dx_km4/libvad_v7_200K.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/ameba_rtos/rtl8721dx_km4/libtomlc99.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/ameba_rtos/rtl8721dx_km4/libkissfft.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/ameba_rtos/rtl8721dx_km4/libaivoice_hal.a
    )
elseif(CONFIG_AMEBASMART)
    ameba_list_append(public_libraries
        ${c_CMPT_AIVOICE_DIR}/prebuilts/ameba_rtos/rtl8730e_ca32_freertos/libaivoice.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/ameba_rtos/rtl8730e_ca32_freertos/libafe_kernel.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/ameba_rtos/rtl8730e_ca32_freertos/lib${AFE_RESOURCE}.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/ameba_rtos/rtl8730e_ca32_freertos/libkernel.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/ameba_rtos/rtl8730e_ca32_freertos/libfst_cn_cmd_ac40.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/ameba_rtos/rtl8730e_ca32_freertos/lib${KWS_RESOURCE}.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/ameba_rtos/rtl8730e_ca32_freertos/libasr_cn_v8_2M.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/ameba_rtos/rtl8730e_ca32_freertos/libvad_v7_200K.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/ameba_rtos/rtl8730e_ca32_freertos/libnnns_com_v7_35K.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/ameba_rtos/rtl8730e_ca32_freertos/libtomlc99.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/ameba_rtos/rtl8730e_ca32_freertos/libNE10.a
        ${c_CMPT_AIVOICE_DIR}/prebuilts/ameba_rtos/rtl8730e_ca32_freertos/libaivoice_hal.a
    )
endif()

ameba_submodule_register(aivoice
    p_EXAMPLE_DIRS
    ${c_CMPT_AIVOICE_DIR}/examples/full_flow_offline
)

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_defines})
ameba_global_library(${public_libraries}) #default: whole-archived
