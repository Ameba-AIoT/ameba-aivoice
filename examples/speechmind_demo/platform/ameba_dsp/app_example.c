#include "ameba_soc.h"
#include <xtensa/idma.h>
#define NUM_MAX_DESC 2 // speedup weight loading

IDMA_BUFFER_DEFINE(buffer_idma, NUM_MAX_DESC, IDMA_1D_DESC);

extern void example_voice(void);

void app_example(void)
{
	//IDMA INIT
	idma_init(0, MAX_BLOCK_16, 16, TICK_CYCLES_1, 0, NULL);
	idma_init_loop(buffer_idma, IDMA_1D_DESC, NUM_MAX_DESC, NULL, NULL);
	example_voice();
}
