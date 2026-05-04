# AIVoice Offline Flow Example

## Description

This folder contains AIVoice offline flow example.

It shows how to use AIVoice full flow (afe+kws+asr) with pre-recorded audio and will run only once after EVB reset.

**Audio functions such as recording and playback are not integrated.**

The pre-recorded audio used in this example is a 3 channel audio, channel 1 and channel 2 are microphone signals with 2mic50mm array and channel 3 is AEC reference signal. Content in example audio is “你好小强” “打开空调” “你好小强” “播放音乐”.

**Note: AFE res, kws lib, fst lib should match the content of audio, otherwise AIVoice can not detect.**

## Supported IC

1. RTL8721Dx KM4 (SDK: ameba-rtos)
2. RTL8721F KM4 (SDK: ameba-rtos)
3. RTL8726E DSP (SDK: ameba-dsp)
4. RTL8713E DSP (SDK: ameba-dsp)
5. RTL8730E CA32 (SDK: ameba-rtos)
6. RTL8730E CA32 Linux (SDK:ameba-linux)

## Build Example

### Using SDK ameba-rtos

#### RTL8730E PSRAM

1. **Configure menuconfig**:
    `cd amebasmart_gcc_project`
    `./menuconfig.py`
    CONFIG Link Option  --->
        IMG2(Application) running on PSRAM or FLASH? (FLASH)  --->
    CONFIG APPLICATION  --->
        AI Config  --->
            [*] Enable TFLITE MICRO
            [*] Enable AIVoice

2. **Build and Download**:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

##### Expected Result (RTL8730E Psram)

**full_flow_offline result**
```
[AFE] multi-kws-beam = 0, 1, 2
---------------------SPEECH COMMANDS---------------------
Command ID1, 打开空调
Command ID2, 关闭空调
Command ID3, 制冷模式
Command ID4, 制热模式
Command ID5, 加热模式
Command ID6, 送风模式
Command ID7, 除湿模式
Command ID8, 调到十六度
Command ID9, 调到十七度
Command ID10, 调到十八度
Command ID11, 调到十九度
Command ID12, 调到二十度
Command ID13, 调到二十一度
Command ID14, 调到二十二度
Command ID15, 调到二十三度
Command ID16, 调到二十四度
Command ID17, 调到二十五度
Command ID18, 调到二十六度
Command ID19, 调到二十七度
Command ID20, 调到二十八度
Command ID21, 调到二十九度
Command ID22, 调到三十度
Command ID23, 开高一度
Command ID24, 开低一度
Command ID25, 高速风
Command ID26, 中速风
Command ID27, 低速风
Command ID28, 增大风速
Command ID29, 减小风速
Command ID30, 自动风
Command ID31, 最大风量
Command ID32, 中等风量
Command ID33, 最小风量
Command ID34, 自动风量
Command ID35, 左右摆风
Command ID36, 上下摆风
Command ID37, 播放音乐
Command ID38, 暂停播放
Command ID39, 接听电话
Command ID40, 挂断电话
---------------------------------------------------------

[AIVOICE] rtk_aivoice version: v1.6.0#S87db3f2#N89da3ed#Ad90cdbe
[AIVOICE] rtk_aivoice_model afe version: afe_2mic_asr_v1.5_AfePara_2mic50_v2.0_bf_v0.0_20250401
[AIVOICE] rtk_aivoice_model vad version: vad_v7_opt
[AIVOICE] rtk_aivoice_model kws version: kws_xqxq_v4.1_opt
[AIVOICE] rtk_aivoice_model asr version: asr_cn_v8_opt
[AIVOICE] rtk_aivoice_log_format version: v2
[user] afe output 1 channels raw audio, others: {"abnormal_flag":0,"ssl_angle":-10}
[AIVOICE] [KWS] result: {"id":2,"keyword":"ni-hao-xiao-qiang","score":0.766761064529419}
[user] wakeup. {"id":2,"keyword":"ni-hao-xiao-qiang","score":0.766761064529419}
[user] voice angle 90.0
[user] vad. status = 1, offset = 425
[user] vad. status = 0, offset = 1865
[AIVOICE] [ASR] result: {"type":0,"commands":[{"rec":"打开空调","id":1}]}
[user] asr. {"type":0,"commands":[{"rec":"打开空调","id":1}]}
[user] voice angle 90.0
[user] vad. status = 1, offset = 525
[AIVOICE] [KWS] result: {"id":2,"keyword":"ni-hao-xiao-qiang","score":0.81332826614379883}
[user] wakeup. {"id":2,"keyword":"ni-hao-xiao-qiang","score":0.81332826614379883}
[user] voice angle 90.0
[user] vad. status = 1, offset = 445
[user] vad. status = 0, offset = 1765
[AIVOICE] [ASR] result: {"type":0,"commands":[{"rec":"播放音乐","id":37}]}
[user] asr. {"type":0,"commands":[{"rec":"播放音乐","id":37}]}
[user] voice angle 90.0
```

#### RTL8721Dx/RTL8721F KM4

> **Note: This chip only supports single microphone input and does not support ASR functionality.**
> **Default: afe_kws_vad flow with 1mic array afe resource**

1. **Configure menuconfig**:
   * Enable the following options:
    ```< CONFIG APPLICATION  ---> AI Config  ---> [*] Enable TFLITE MICRO```
    ```< CONFIG APPLICATION  ---> AI Config  ---> [*] Enable AIVoice -> Select AFE Resource (afe_res_1mic)```

2. **Build and Download**:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

##### Expected Result (RTL8721Dx KM4)

**full_flow_offline result**
```
[AIVOICE] rtk_aivoice version: v1.6.0#S87db3f2#N89da3ed#Ad90cdbe
[AIVOICE] rtk_aivoice_model afe version: afe_1mic_asr_v1.1_AfePara_1mic_ASR_v0.0_20240626_COM_v0.3_20250528
[AIVOICE] rtk_aivoice_model vad version: vad_v8_opt
[AIVOICE] rtk_aivoice_model kws version: kws_xqxq_v5_opt
[AIVOICE] rtk_aivoice_log_format version: v2
[user] afe output 1 channels raw audio, others:
[AIVOICE] [KWS] result: {"id":2,"keyword":"ni-hao-xiao-qiang","score":0.80648398399353027}
[user] wakeup. {"id":2,"keyword":"ni-hao-xiao-qiang","score":0.80648398399353027}
[user] vad. status = 1, offset = 1685
[user] vad. status = 0, offset = 3085
[user] vad. status = 1, offset = 4765
[user] vad. status = 0, offset = 6205
```

### Using SDK ameba-dsp

#### Using SDK ameba-dsp with Xplorer

1. **import example full_flow_offline into project_dsp/example**
   select configuration: `HIFI5_PROD_1123_asic_UPG` or `HIFI5_PROD_1123_asic_wUPG`

2. **amend Build Properties for project dsp**
    *2.1 add include path (-I)*
    ```
    ${workspace_loc}/../lib/aivoice/include
    ${workspace_loc}/../lib/aivoice/examples/full_flow_offline/platform/ameba_dsp
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
    ./add_all_settings_to_project.sh /path/to/project_dsp/ full_flow_offline
    ```

    * if use binary resources

    ```sh
    ./add_all_settings_to_project.sh /path/to/project_dsp/ full_flow_offline --no_lib_resources
    ```

    this shell script inserts every dependencies and linked resources into `Release.bts` and `.project`.

2. **build dsp_all.bin**

    ```sh
    cd dsp/source/project/auto_build/
    ./auto_build.sh
    ```

    binary image `dsp_all.bin` will be automatically generated under `dsp/source/project/image/`.

    **NOTE: For proper operation, please run this script only in a cleaning working directory, exactly as retrieved from the git repository. To build another example, please remove all added dependencies and linked resources from `Release.bts` and `.project`.**

3. **if use binary resources, pack up binary resources as following steps show**

   ```sh
   cd ameba-rtos/amebalite_gcc_project/
   ./menuconfig.py
   ```

    Select resources:

    --------MENUCONFIG FOR General---------
    CONFIG DSP Enable  --->
    ...
    CONFIG APPLICATION  --->
       Graphics Libraries Configuration  --->
       ...
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



#### Expected Result (RTL8726E/RTL8713E DSP)

The expected result on amebalite dsp is the same as [Expected Result (RTL8730E Psram)](#expected-result-amebasmart-psram)

### Using SDK RTL8730E Linux

1. There are dependencies between libraries, refer to `./Makefile` and `rtk-aivoice-algo.bb`.
   bb file is `pgos/sources/yocto/meta-realtek/meta-sdk/recipes-rtk/aivoice/rtk-aivoice-algo.bb`.

2. use ```bitbake rtk-aivoice-algo``` to compile the aivoice algo module,
   the generated executable is `rtk_aivoice_algo`.

#### Expected Result (RTL8730E Linux)

The expected result on amebasmart linux is the same as [Expected Result (RTL8730E Psram)](#expected-result-amebasmart-psram)

### Pack Up Binary Resources

If you want to use binary resources instead of c library resources, you need to do the following:

#### Using SDK ameba-rtos menuconfig

1. **Switch to ameba-rtos**

   ```sh
   cd ameba-rtos/amebalite_gcc_project/
   ```

2. **Select resources in menuconfig**

   ```sh
   ./menuconfig.py
   ```

    --------MENUCONFIG FOR General---------
    CONFIG DSP Enable  --->
    ...
    CONFIG APPLICATION  --->
       Graphics Libraries Configuration  --->
       ...
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

3. **Pack up binary resources**
