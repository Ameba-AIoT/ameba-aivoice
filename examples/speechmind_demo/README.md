# AIVoice Demo Flow Example

## Description

This folder contains AIVoice demo flow example.

It shows how to use AIVoice flow with realtime audio stream and will be always-on.

**Audio functions such as recording and playback are integrated in the MCU. The example needs to be used with SpeechMind in MCU.**

The audio recorder will in send the 3 channel audio, channel 1 and channel 2 are microphone signals with 2mic50mm array and channel 3 is AEC reference signal. You can try saying “你好小强” “打开空调” “你好小强” “播放音乐” etc.

**Note: AFE res, kws lib, fst lib should match the content of audio, otherwise AIVoice can not detect.**

## Supported IC
1. AmebaLite DSP (SDK: ameba-dsp)

## Build Example

### Using SDK ameba-dsp

#### Using SDK ameba-dsp with Xplorer

1. **import example speechmind_demo into project_dsp/example**
   select configuration: `HIFI5_PROD_1123_asic_UPG` or `HIFI5_PROD_1123_asic_wUPG`

2. **amend Build Properties for project dsp**
    *2.1 add include path (-I)*
    ```
    ${workspace_loc}/../lib/aivoice/include
    ${workspace_loc}/../lib/aivoice/examples/speechmind_demo/platform/ameba_dsp
    ${workspace_loc}/../lib/aivoice/examples/speechmind_demo/platform/ameba_dsp/aidl
    ```

    *2.2 add library search path (-L)*
    ```
    ${workspace_loc}/../lib/aivoice/prebuilts/lib/ameba_dsp/$(TARGET_CONFIG)
    ${workspace_loc}/../lib/xa_nnlib/v2.3.0/bin/$(TARGET_CONFIG)/Release
    ${workspace_loc}/../lib/lib_hifi5/v3.1.0/bin/$(TARGET_CONFIG)
    ```

    *2.3 add librarys (-l)*
      * if use c library resources
        -laivoice -lafe_kernel -lkernel -lafe_res_2mic50mm -lvad_v7_200K -lkws_xiaoqiangxiaoqiang_nihaoxiaoqiang_v4_300K -lasr_cn_v8_2M -lfst_cn_cmd_ac40 -lcJSON -ltomlc99 -ltflite_micro -lxa_nnlib -lhifi5_dsp -laivoice_hal
      * if use binary resources
        -laivoice -lafe_kernel -lkernel -lcJSON -ltomlc99 -ltflite_micro -lxa_nnlib -lhifi5_dsp -laivoice_hal

    *2.4 add symbols (-D)*
      * if use c library resources  
        USE_BINARY_RESOURCE 0
      * if use binary resources  
        USE_BINARY_RESOURCE 1

    **NOTE: if multiple definition of `app_example` occurs, please exclude other examples when build this example**

#### Using SDK ameba-dsp with Command Line

1. **add all aivoice example dependencies and linked resources into project**

    * if use c library resources

    ```sh
    ./add_all_settings_to_project.sh /path/to/project_dsp/ speechmind_demo
    ```

    * if use binary resources
  
    ```sh
    ./add_all_settings_to_project.sh /path/to/project_dsp/ speechmind_demo --no_lib_resources
    ```

    this shell script inserts every dependencies and linked resources into `Release.bts` and `.project`.

    **NOTE: For proper operation, please run this script only in a cleaning working directory, exactly as retrieved from the git repository**

2. **build dsp_all.bin**

    ```sh
    cd dsp/source/project/auto_build/
    ./auto_build.sh
    ```

    binary image `dsp_all.bin` will be automatically generated under `dsp/source/project/image/`.

    **NOTE: To build another example, please remove all added dependencies and linked resources from `Release.bts` and `.project`.**

### Pack Binary Resources

If you are using binary resources instead of C library resources, please pack them into one file by following steps:

1. **select resources from ameba-rtos menuconfig**
  
  ```sh
  cd ameba-rtos/amebalite_gcc_project/
  ./menuconfig.py
  ```

  Select from menuconfig:
    --------MENUCONFIG FOR General---------
    CONFIG APPLICATION  --->
      AI Config  --->
        [*] Enable TFLITE MICRO
        [*] Enable AIVoice
        [*]     Select AFE Resource
                    AFE (afe_res_2mic50mm)  --->
        [*]     Select VAD Resource
                    VAD (vad_v7_200K)  --->
        [*]     Select KWS Resource
                    KWS (kws_xiaoqiangxiaoqiang_nihaoxiaoqiang_v4_300K)  --->
        [*]     Select ASR Resource
                    ASR (asr_cn_v8_2M)  --->
        [*]     Select FST Resource
                    FST (fst_cn_cmd_ac40)  --->

2. **pack resources using tools/pack_resources**

  ```sh
  cd ameba-rtos/component/aivoice
  ./tools/pack_resources/pack_resources_for_dsp.sh
  ```

  This tool will select resources from menuconfig and pack them into one file `aivoice_models.bin` under current directory.

3. **download aivoice_models.bin**

  After packing resources, download `aivoice_models.bin` to your device.
  The start address of downloading is `0x08A00000`, which is defined in `speechmind_demo/platform/ameba_dsp/voice_service.c`:
  
  ```c
  #define AIVOICE_BIN_FLASH_ADDRESS_START (0x08A00000)
  ```
