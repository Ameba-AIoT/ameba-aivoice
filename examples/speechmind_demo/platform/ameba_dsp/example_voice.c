#define LOG_TAG "DSPVoiceExample"
//#include "log/log.h"

#include "ameba_soc.h"

#include "VoiceRPC_Agent.h"
#include "VoiceRPC_System.h"

#include "rpc_api.h"
#include "rpc_struct.h"

#include "voice_utils.h"


extern void DspVoiceTask(void *param);

void AppInitRPC(void *param)
{
	(void)param;
	LOGD("%s Enter.", __FUNCTION__);

	struct REG_STRUCT *pReg = NULL;

	// Add for Speech
	pReg = VOICE_SYSTEM_0_register(pReg);
	pReg = ReplyHandler_register(pReg);

	RPC_INIT_STRUCT init_param;
	init_param.task_size = 1024 * 3;
	init_param.priority = 3;
	RPC_InitProxy(pReg, VOICE_RPC_CHANNEL, &init_param);
	vTaskDelete(NULL);
}

void app_ipc_proxy_init(void)
{
	LOGD("%s Enter.", __FUNCTION__);
	xTaskCreate(AppInitRPC, "AppInitRPC", DEFAULT_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
}

void example_dsp_voice(void)
{
	LOGD("%s Enter.", __FUNCTION__);
	xTaskCreate(DspVoiceTask, "DspVoiceTask", 2048, NULL, tskIDLE_PRIORITY + 1, NULL);
}

void voice_task(void *param)
{
	LOGD("%s Enter.", __FUNCTION__);
	RRAM_TypeDef *rram = RRAM_DEV;
	while (rram->IMQ2_INIT_DONE == 0) {
		vTaskDelay(200);
	}

	app_ipc_proxy_init();
	RPC_SetLog(0);
	example_dsp_voice();
	vTaskDelete(NULL);
}

void example_voice(void)
{
	printf("[DSP]example_voice\n\n");
#if defined(CONFIG_RPC_EN) && CONFIG_RPC_EN
	RPC_Init();
	if (xTaskCreate(voice_task, ((const char *)"voice_task"), 1024, NULL, tskIDLE_PRIORITY + 2, NULL) != pdPASS) {
		printf("\n\r%s xTaskCreate(voice_task) failed", __FUNCTION__);
	}
#endif
}
