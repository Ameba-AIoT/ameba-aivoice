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

#ifndef EXAMPLE_VOICE_UTILS_H
#define EXAMPLE_VOICE_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ameba_soc.h"

#ifdef RPC_KR4_DSP
#define VOICE_RPC_CHANNEL		((RPC_DSP_AP << 1) | (RPC_AP_DSP << 4))
#endif

#ifdef RPC_KM4_DSP
#define VOICE_RPC_CHANNEL		((RPC_DSP_NP << 1) | (RPC_NP_DSP << 4))
#endif

#define DEFAULT_STACK_SIZE		1024*16

#if DSP_VOICE_DEBUG
#define LOGV(fmt, ...)  DiagPrintf("[DSP_VOICE-V] : " fmt "\n", ##__VA_ARGS__)
#else
#define LOGV(fmt, ...)  do { } while(0)
#endif
#define LOGD(fmt, ...)  DiagPrintf("[DSP_VOICE-D] : " fmt "\n", ##__VA_ARGS__)
#define LOGI(fmt, ...)  DiagPrintf("[DSP_VOICE-I] : " fmt "\n", ##__VA_ARGS__)
#define LOGE(fmt, ...)  DiagPrintf("[DSP_VOICE-E] : " fmt "\n", ##__VA_ARGS__)


#ifdef __cplusplus
}
#endif

#endif // EXAMPLE_VOICE_UTILS_H
