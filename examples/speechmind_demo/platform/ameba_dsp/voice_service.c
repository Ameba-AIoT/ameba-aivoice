/*
 * Copyright (c) 2022 Realtek, LLC.
 * All rights reserved.
 *
 * Licensed under the Realtek License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License from Realtek
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ameba_soc.h"
#include "ring_buffer.h"

#include "rpc_api.h"
#include "rpc_struct.h"

#include "VoiceRPC_Agent_data.h"
#include "VoiceRPC_System.h"
#include "VoiceRPC_Agent.h"

#include "voice_utils.h"
#include "parcel.h"
#include "aivoice_interface.h"

#define AFE_FRAME_MS 16
#define AFE_IN_CHANNEL 3
#define AFE_SAMPLE_RATE 16000
#define AFE_BITS 16
#define AFE_FRAME_LEN (16*16*2)
#define AFE_FRAME_BYTES (AFE_IN_CHANNEL*AFE_FRAME_MS*AFE_SAMPLE_RATE*(AFE_BITS/8)/1000LL)
#define VAD_FRAME_BYTES (1*AFE_FRAME_MS*AFE_SAMPLE_RATE*(AFE_BITS/8)/1000LL)

#define DATA_CACHE_SIZE 1024
static ring_buffer *g_mic_ring_buffer = NULL;
static ring_buffer *g_afe_ring_buffer = NULL;
static void *g_handle = NULL;
static bool g_voice_running = false;
static bool g_voice_task_exit = false;
static const struct rtk_aivoice_iface *g_aivoice;
static uint8_t g_data[DATA_CACHE_SIZE];

#if defined(USE_DTCM)
#define DRAM0 __attribute__((section(".dram0.data")))
#define DRAM1 __attribute__((section(".dram1.data")))
#else
#define DRAM0
#define DRAM1
#endif

#define DATASIZE_124K (100 * 1024)
char DRAM0 g_dtcm_buffer_124k_0[DATASIZE_124K];

#define portGET_RUN_TIME_COUNTER_VALUE() \
    (*((volatile uint32_t*)(0x4011E000 + 0x004)))

static uint32_t g_usr_data;

/*****************************************************************************/
//            aivoive binary resource configuration
/*****************************************************************************/
#if USE_BINARY_RESOURCE
/* modify these values ​​according to facts. */
// flash start address used to download aivoice_models.bin
#define AIVOICE_BIN_FLASH_ADDRESS_START (0x08A00000)
// bytes of aivoice_models.bin
#define MAX_AIVOICE_BIN_SIZE (4*1024*1024)
#endif

#if USE_BINARY_RESOURCE
__attribute__((weak))
const char *aivoice_load_resource_from_flash(void)
{
    const uint8_t *flash_header = (const uint8_t *)AIVOICE_BIN_FLASH_ADDRESS_START;
    uint32_t bin_size = 0;

    bin_size = ((uint32_t)flash_header[15] << 24) |
               ((uint32_t)flash_header[14] << 16) |
               ((uint32_t)flash_header[13] << 8)  |
               ((uint32_t)flash_header[12]);

    if (bin_size == 0 || bin_size > MAX_AIVOICE_BIN_SIZE) { // 限制最大 4MB
        LOGE("Invalid bin size: %d, max %d\n", bin_size, MAX_AIVOICE_BIN_SIZE);
        return NULL;
    }

    char *aivoice_resources = (char *)malloc(bin_size);
	if (!aivoice_resources) {
		LOGE("malloc failed for aivoice resource buffer\n");
		return NULL;
	}

	LOGI("Load aivoice resource from flash (size=%d bytes)\n", bin_size);
	memcpy(aivoice_resources, (const void *)AIVOICE_BIN_FLASH_ADDRESS_START, bin_size);
	return aivoice_resources;
}
#endif
/*****************************************************************************/

CLNT_STRUCT GetVoiceAgent(void)
{
	return RPC_PrepareCLNT(NONBLOCK_MODE | VOICE_RPC_CHANNEL, VOICE_AGENT, 0);
}


void NotifyMsg(uint32_t usr_data, int type, int32_t len, uint32_t msg_addr)
{
	LOGV("%s Enter %d", __FUNCTION__, __LINE__);
	CLNT_STRUCT clnt = GetVoiceAgent();
	VOICE_RPC_RESULT argp;
	argp.usr_data = usr_data;
	argp.type = type;
	argp.len = len;
	argp.msg_addr = msg_addr;
	HRESULT *res = VOICE_RPC_ToSystem_Callback_0(&argp, &clnt);
	if (res) {
		LOGV("NotifyMsg res=%d", *res);
		free(res);
	}
}

void NotifyState(int type, uint32_t state)
{
	LOGV("%s Enter %d", __FUNCTION__, __LINE__);
	VOICE_RPC_ERROR_STATE argp;
	argp.type = type;
	argp.data = state;
	CLNT_STRUCT clnt = RPC_PrepareCLNT(BLOCK_MODE | VOICE_RPC_CHANNEL, VOICE_AGENT, 0);
	HRESULT *res = VOICE_RPC_ToSystem_State_0(&argp, &clnt);
	if (res) {
		LOGV("NotifyState res=%d", *res);
		free(res);
	}
}

int aivoice_struct_deep_copy(uint8_t* buffer, size_t buffer_size,
                            const struct aivoice_evout_afe* source)
{
    size_t json_length = strlen(source->out_others_json);
    size_t required_size = sizeof(struct aivoice_evout_afe) +
                          AFE_FRAME_LEN +
                          json_length + 1;

    if (buffer_size < required_size) {
        LOGE("Buffer too small for AIVOICE_EVOUT_AFE structure, required: %zu, available: %zu",
             required_size, buffer_size);
        return -1;
    }

    uint8_t* buffer_cursor = buffer;
    struct aivoice_evout_afe* struct_header = (struct aivoice_evout_afe*)buffer_cursor;

    struct_header->ch_num = source->ch_num;
    buffer_cursor += sizeof(struct aivoice_evout_afe);

    struct_header->data = (short*)buffer_cursor;
    memcpy(buffer_cursor, source->data, AFE_FRAME_LEN);
    buffer_cursor += AFE_FRAME_LEN;

    struct_header->out_others_json = (char*)buffer_cursor;
    memcpy(buffer_cursor, source->out_others_json, json_length);
    buffer_cursor[json_length] = '\0';
    buffer_cursor += json_length + 1;

    return 0;
}

static int Aivoice_Callback(void *userdata,
							enum aivoice_out_event_type event_type,
							const void *msg, int len)
{
	LOGV("%s Enter %d", __FUNCTION__, __LINE__);

	memset(g_data, 0, DATA_CACHE_SIZE);
	if(event_type == AIVOICE_EVOUT_AFE) {
		aivoice_struct_deep_copy(g_data, DATA_CACHE_SIZE, msg);
	} else {
		memcpy(g_data, msg, (size_t)len);
	}
	DCache_Clean((void *)g_data, DATA_CACHE_SIZE);

	NotifyMsg((uint32_t)userdata, (int)event_type, len, (uint32_t)g_data);
	return 0;
}

void Voice_ParcelRelease(Parcel *parcel, const uint8_t *data, size_t dataSize)
{
	NotifyState(1, g_usr_data);
}


static HRESULT *Voice_Create(VOICE_RPC_INIT *pParam, RPC_STRUCT *pRpcStruct, HRESULT *pRes)
{
	LOGV("%s Enter %d", __FUNCTION__, __LINE__);
	(void)pRpcStruct;

	*pRes = 0;
	if (g_handle) {
		*pRes = -1;
		return pRes;
	}

	memset(g_data, 0, DATA_CACHE_SIZE);

	ring_buffer_header *mic_header = (ring_buffer_header *)pParam->mic_header_addr;
	DCache_Invalidate((void *)mic_header, (uint32_t)pParam->mic_header_length);
	g_mic_ring_buffer = ring_buffer_create_by_header(mic_header);
	if (!g_mic_ring_buffer) {
		*pRes = -1;
		return pRes;
	}

	switch (pParam->voice_iface_flag) {
	case 0:
		g_aivoice = &aivoice_iface_full_flow_v1;
		break;

	case 1:
		g_aivoice = &aivoice_iface_afe_kws_v1;
		break;

	case 2:
		g_aivoice = &aivoice_iface_afe_kws_vad_v1;
		break;

	case 3:
		g_aivoice = &aivoice_iface_afe_v1;
		break;

	case 4:
		g_aivoice = &aivoice_iface_vad_v1;
		break;

	case 5:
		g_aivoice = &aivoice_iface_kws_v1;
		break;

	case 6:
		g_aivoice = &aivoice_iface_asr_v1;
		break;

	default:
		g_aivoice = &aivoice_iface_full_flow_v1;
		break;
	}

	struct aivoice_config config;
	memset(&config, 0, sizeof(config));

	uint8_t *data = (uint8_t *)pParam->config_addr;
	size_t length = (size_t)pParam->config_length;
	DCache_Invalidate((void *)data, (uint32_t)pParam->config_length);

	Parcel *parcel = Parcel_Create();
	if (!parcel) {
		*pRes = -1;
		return pRes;
	}

	Parcel_IpcSetData(parcel, data, length, Voice_ParcelRelease);

	struct afe_config afe_param;
	afe_param.mic_array = (afe_mic_geometry_e)Parcel_ReadInt32(parcel);
	afe_param.ref_num = (int)Parcel_ReadInt32(parcel);
	afe_param.sample_rate = (int)Parcel_ReadInt32(parcel);
	afe_param.frame_size = (int)Parcel_ReadInt32(parcel);
	afe_param.afe_mode = (afe_mode_e)Parcel_ReadInt32(parcel);
	afe_param.enable_aec = Parcel_ReadBool(parcel);
	afe_param.enable_ns = Parcel_ReadBool(parcel);
	afe_param.enable_agc = Parcel_ReadBool(parcel);
	afe_param.enable_ssl = Parcel_ReadBool(parcel);
	afe_param.aec_mode = (afe_aec_mode_e)Parcel_ReadInt32(parcel);
	afe_param.aec_enable_threshold = Parcel_ReadInt32(parcel);
	afe_param.enable_res = Parcel_ReadBool(parcel);
	afe_param.aec_cost = (afe_aec_filter_tap_e)Parcel_ReadInt32(parcel);
	afe_param.res_aggressive_mode = (afe_aec_res_aggressive_mode_e)Parcel_ReadInt32(parcel);
	afe_param.ns_mode = (afe_ns_mode_e)Parcel_ReadInt32(parcel);
	afe_param.ns_cost_mode = (afe_ns_cost_mode_e)Parcel_ReadInt32(parcel);
	afe_param.ns_aggressive_mode = (afe_ns_aggressive_mode_e)Parcel_ReadInt32(parcel);
	afe_param.agc_fixed_gain = Parcel_ReadInt32(parcel);
	afe_param.enable_adaptive_agc = Parcel_ReadBool(parcel);
	afe_param.ssl_resolution = Parcel_ReadFloat(parcel);
	afe_param.ssl_min_hz = Parcel_ReadInt32(parcel);
	afe_param.ssl_max_hz = Parcel_ReadInt32(parcel);

	struct vad_config vad_param;
	vad_param.sensitivity = (vad_sensitivity_e)Parcel_ReadInt32(parcel);
	vad_param.left_margin = Parcel_ReadUint32(parcel);
	vad_param.right_margin = Parcel_ReadUint32(parcel);

	struct kws_config kws_param;
	int keyword_nums = Parcel_ReadInt8(parcel);
	for (int i = 0; i < MAX_KWS_KEYWORD_NUMS; i++) {
		if (i < keyword_nums) {
			kws_param.keywords[i] = Parcel_ReadCString(parcel);
			kws_param.thresholds[i] = Parcel_ReadFloat(parcel);
		} else {
			kws_param.keywords[i] = NULL;
			kws_param.thresholds[i] = 0;
		}
	}
	kws_param.sensitivity = (kws_sensitivity_e)Parcel_ReadInt32(parcel);
	kws_param.mode = (kws_mode_e)Parcel_ReadInt32(parcel);
	kws_param.enable_age_gender = Parcel_ReadBool(parcel);

	struct asr_config asr_param;
	asr_param.sensitivity = (asr_sensitivity_e)Parcel_ReadInt32(parcel);

	struct aivoice_sdk_config aivoice_param;
	aivoice_param.timeout = Parcel_ReadInt32(parcel);
	aivoice_param.memory_alloc_mode = (aivoice_memory_alloc_mode_e)Parcel_ReadInt32(parcel);

	config.afe = &afe_param;
	config.vad = &vad_param;
	config.kws = &kws_param;
	config.asr = &asr_param;
	config.common = &aivoice_param;

#if USE_BINARY_RESOURCE
	/* when use a aivoice binary resource instead of c resource libraries,
	   users need to load the resource from flash to memory,
	   then pass the memory address to aivoice */
	config.resource = aivoice_load_resource_from_flash();
	if (!config.resource) {
		LOGE("error: load aivoice resource failed\n");
		*pRes = -1;
		return pRes;
	}
	LOGI("aivoice resource start address %p\n", config.resource);
#endif
	g_handle = g_aivoice->create(&config);
	if (!g_handle) {
		*pRes = -1;
		return pRes;
	}

	rtk_aivoice_register_callback(g_handle, Aivoice_Callback, (void *)pParam->usr_data);

	g_usr_data = pParam->usr_data;

	Parcel_Destroy(parcel);

	return pRes;
}

static HRESULT *Voice_destroy(long *pParam, RPC_STRUCT *pRpcStruct, HRESULT *pRes)
{
	LOGV("%s Enter.", __FUNCTION__);
	(void)pParam;
	(void)pRpcStruct;

	*pRes = 0;
	g_voice_running = false;
	while (!g_voice_task_exit) {
		vTaskDelay(10);
	}
	if (g_handle) {
		g_aivoice->destroy(g_handle);
	}
	g_handle = NULL;
	return pRes;
}

void VoiceLoop(void *param)
{
	LOGV("%s Enter.", __FUNCTION__);
	uint8_t tmp_data[AFE_FRAME_BYTES] = {0};
	while (g_voice_running) {
		if (g_mic_ring_buffer->available(g_mic_ring_buffer) < AFE_FRAME_BYTES) {
			vTaskDelay(1);
			continue;
		}
		uint32_t res = g_mic_ring_buffer->read(g_mic_ring_buffer, (uint8_t *)tmp_data, AFE_FRAME_BYTES);
		//int32_t t0 = portGET_RUN_TIME_COUNTER_VALUE();
		g_aivoice->feed(g_handle,
						(char *)tmp_data,
						AFE_FRAME_BYTES);
		//int32_t t1 = portGET_RUN_TIME_COUNTER_VALUE();
		//int32_t total_cycles = t1 -t0;
		//LOGD("total cycles %d us\n", total_cycles);
	}
	g_voice_task_exit = true;
	vTaskDelete(NULL);
}

static HRESULT *Voice_Start(long *pParam, RPC_STRUCT *pRpcStruct, HRESULT *pRes)
{
	LOGV("%s Enter", __FUNCTION__);
	(void)pParam;
	(void)pRpcStruct;

	*pRes = 0;
	g_voice_running = true;
	g_voice_task_exit = false;
	xTaskCreate(VoiceLoop, "VoiceLoop", DEFAULT_STACK_SIZE, NULL, tskIDLE_PRIORITY + 5, NULL);
	return pRes;
}


void DspVoiceTask(void *params)
{
	(void)params;
	LOGV("%s Enter", __FUNCTION__);

	p_VOICE_RPC_ToAgent_Create_0_svc = Voice_Create;
	p_VOICE_RPC_ToAgent_destroy_0_svc = Voice_destroy;
	p_VOICE_RPC_ToAgent_Start_0_svc = Voice_Start;
	NotifyState(0, 1);
	vTaskDelete(NULL);
}
