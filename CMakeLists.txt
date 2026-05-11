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

set(VAD_RESOURCE vad_v7_200K)
if(CONFIG_VAD_V7_200K)
    set(VAD_RESOURCE vad_v7_200K)
elseif(CONFIG_VAD_V8_60K)
    set(VAD_RESOURCE vad_v8_60K)
elseif(CONFIG_VAD_V9_208K)
    set(VAD_RESOURCE vad_v9_208K)
elseif(CONFIG_VAD_V11_208K)
    set(VAD_RESOURCE vad_v11_208K)
endif()

set(KWS_RESOURCE kws_xiaoqiangxiaoqiang_nihaoxiaoqiang_v4_300K)
if(CONFIG_KWS_XIAOQIANGXIAOQIANG_NIHAOXIAOQIANG_V4_300K)
    set(KWS_RESOURCE kws_xiaoqiangxiaoqiang_nihaoxiaoqiang_v4_300K)
elseif(CONFIG_KWS_XIAOQIANGXIAOQIANG_NIHAOXIAOQIANG_V5_125K)
    set(KWS_RESOURCE kws_xiaoqiangxiaoqiang_nihaoxiaoqiang_v5_125K)
elseif(CONFIG_KWS_CN_CUSTOMIZED_V3_750K)
    set(KWS_RESOURCE kws_cn_customized_v3_750K)
elseif(CONFIG_KWS_CN_CUSTOMIZED_V5_886K)
    set(KWS_RESOURCE kws_cn_customized_v5_886K)
endif()

set(ASR_RESOURCE asr_cn_v8_2M)
if(CONFIG_ASR_CN_V10_1M)
    set(ASR_RESOURCE asr_cn_v10_1M)
endif()

if(CONFIG_AMEBAGREEN2)
    set(PLATFORM_DIR rtl8721f_km4)
elseif(CONFIG_AMEBADPLUS)
    set(PLATFORM_DIR rtl8721dx_km4)
elseif(CONFIG_AMEBASMART)
    set(PLATFORM_DIR rtl8730e_ca32_freertos)
endif()

set(AIVOICE_LIB_DIR ${c_CMPT_AIVOICE_DIR}/prebuilts/lib/ameba_rtos/${PLATFORM_DIR})

ameba_list_append(public_libraries
    ${AIVOICE_LIB_DIR}/libaivoice.a
    ${AIVOICE_LIB_DIR}/libafe_kernel.a
    ${AIVOICE_LIB_DIR}/lib${AFE_RESOURCE}.a
    ${AIVOICE_LIB_DIR}/libkernel.a
    ${AIVOICE_LIB_DIR}/lib${KWS_RESOURCE}.a
    ${AIVOICE_LIB_DIR}/lib${VAD_RESOURCE}.a
    ${AIVOICE_LIB_DIR}/libtomlc99.a
    ${AIVOICE_LIB_DIR}/libkissfft.a
    # ${AIVOICE_LIB_DIR}/libcJSON.a
    ${AIVOICE_LIB_DIR}/libaivoice_hal.a
)

if(CONFIG_AMEBASMART)
    ameba_list_append(public_libraries
        ${AIVOICE_LIB_DIR}/libNE10.a
        ${AIVOICE_LIB_DIR}/libfst_cn_cmd_ac40.a
        ${AIVOICE_LIB_DIR}/libnnns_com_v7_35K.a
        ${AIVOICE_LIB_DIR}/lib${ASR_RESOURCE}.a
    )
endif()

ameba_submodule_register(aivoice
    p_EXAMPLE_DIRS
    ${c_CMPT_AIVOICE_DIR}/examples/full_flow_offline
    ${c_CMPT_AIVOICE_DIR}/examples/aivoice_integrated_test
)

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_defines})
ameba_global_library(${public_libraries}) #default: whole-archived
