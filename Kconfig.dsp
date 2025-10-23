config AIVOICE_EN_MENU
    bool "Enable AIVoice"
    select AIVOICE_EN if WHC_HOST || WHC_NONE

if AIVOICE_EN_MENU
    config SELECT_AFE_RESOURCE
        prompt "Select AFE Resource"
        default y

    choice
        prompt "AFE"
        depends on SELECT_AFE_RESOURCE
        default AFE_RES_2MIC50MM_MENU

        config AFE_RES_1MIC_MENU
            bool "afe_res_1mic"
            select AFE_RES_1MIC if WHC_HOST || WHC_NONE

        config AFE_RES_2MIC30MM_MENU
            bool "afe_res_2mic30mm"
            select AFE_RES_2MIC30MM if WHC_HOST || WHC_NONE

        config AFE_RES_2MIC50MM_MENU
            bool "afe_res_2mic50mm"
            select AFE_RES_2MIC50MM if WHC_HOST || WHC_NONE

        config AFE_RES_2MIC70MM_MENU
            bool "afe_res_2mic70mm"
            select AFE_RES_2MIC70MM if WHC_HOST || WHC_NONE

        config AFE_RES_CIRCLE3MIC50MM_MENU
            bool "afe_res_3mic50mm"
            select AFE_RES_CIRCLE3MIC50MM if WHC_HOST || WHC_NONE

    endchoice

    config SELECT_VAD_RESOURCE
            prompt "Select VAD Resource"
            default y

    choice
        prompt "VAD"
        depends on SELECT_VAD_RESOURCE

        config VAD_V7_200K_MENU
            bool "vad_v7_200K"
            select VAD_V7_200K if WHC_HOST || WHC_NONE

        config VAD_V8_60K_MENU
            bool "vad_v8_60K"
            select VAD_V8_60K if WHC_HOST || WHC_NONE
    endchoice

    config SELECT_KWS_RESOURCE
        prompt "Select KWS Resource"
        default y

    choice
        prompt "KWS"
        depends on SELECT_KWS_RESOURCE
        default KWS_XIAOQIANGXIAOQIANG_NIHAOXIAOQIANG_V4_300K_MENU

        config KWS_XIAOQIANGXIAOQIANG_NIHAOXIAOQIANG_V4_300K_MENU
            bool "kws_xiaoqiangxiaoqiang_nihaoxiaoqiang_v4_300K"
            select KWS_XIAOQIANGXIAOQIANG_NIHAOXIAOQIANG_V4_300K if WHC_HOST || WHC_NONE

        config KWS_XIAOQIANGXIAOQIANG_NIHAOXIAOQIANG_V5_125K_MENU
            bool "kws_xiaoqiangxiaoqiang_nihaoxiaoqiang_v5_125K"
            select KWS_XIAOQIANGXIAOQIANG_NIHAOXIAOQIANG_V5_125K if WHC_HOST || WHC_NONE

        config KWS_CN_CUSTOMIZED_V2_410K_MENU
            bool "kws_cn_customized_v2_410K"
            select KWS_CN_CUSTOMIZED_V2_410K if WHC_HOST || WHC_NONE

        config KWS_CN_CUSTOMIZED_V3_750K_MENU
            bool "kws_cn_customized_v3_750K"
            select KWS_CN_CUSTOMIZED_V3_750K if WHC_HOST || WHC_NONE
    endchoice

    config SELECT_ASR_RESOURCE
        prompt "Select ASR Resource"
        default y

    choice
        prompt "ASR"
        depends on SELECT_ASR_RESOURCE
        config ASR_CN_V8_2M_MENU
            bool "asr_cn_v8_2M"
            select ASR_CN_V8_2M if WHC_HOST || WHC_NONE

        config ASR_CN_V10_1.1M_MENU
            bool "asr_cn_v10_1.1M"
            select ASR_CN_V10_1.1M if WHC_HOST || WHC_NONE
    endchoice

    config SELECT_FST_RESOURCE
        prompt "Select FST Resource"
        default y

    choice
        prompt "FST"
        depends on SELECT_FST_RESOURCE
        default FST_CN_CMD_AC40_MENU

        config FST_CN_CMD_AC40_MENU
            bool "fst_cn_cmd_ac40"
            select FST_CN_CMD_AC40 if WHC_HOST || WHC_NONE
    endchoice

endif

config AIVOICE_EN
    bool

config SELECT_AFE_RESOURCE
    bool

config SELECT_VAD_RESOURCE
    bool

config SELECT_KWS_RESOURCE
    bool

config SELECT_ASR_RESOURCE
    bool

config SELECT_FST_RESOURCE
    bool

config AFE_RES_1MIC
    bool

config AFE_RES_2MIC30MM
    bool

config AFE_RES_2MIC50MM
    bool

config AFE_RES_2MIC70MM
    bool

config AFE_RES_CIRCLE3MIC50MM
    bool

config VAD_V7_200K
    bool

config VAD_V8_60K
    bool

config KWS_XIAOQIANGXIAOQIANG_NIHAOXIAOQIANG_V4_300K
    bool

config KWS_XIAOQIANGXIAOQIANG_NIHAOXIAOQIANG_V5_125K
    bool

config KWS_CN_CUSTOMIZED_V2_410K
    bool

config KWS_CN_CUSTOMIZED_V3_750K
    bool

config ASR_CN_V8_2M
    bool

config ASR_CN_V10_1.1M
    bool

config FST_CN_CMD_AC40
    bool
