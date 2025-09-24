#include "ameba_soc.h"

extern void aivoice_algo_offline_example();

static void aivoice_thread(void *param)
{
	(void)param;

	DelayMs(1000);

	aivoice_algo_offline_example();

	vTaskDelete(NULL);
}

static void example_aivoice(void)
{
	if (xTaskCreate(aivoice_thread, ((const char *)"aivoice_thread"), 1024 * 4, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		printf("\n\r%s xTaskCreate failed", __FUNCTION__);
	}
}

void app_example(void)
{
	example_aivoice();
}