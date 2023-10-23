# AI-driver

This folder contains both the headers (see `./inc`) and the precompiled AI driver library (see `./lib`) `libNetworkRuntime810_CM7_GCC.a` from ST Microelectronics obtained from STM32CubeIDE. Depending on toolchain, this library is linked within a makefile like so:

```
C_INCLUDES += -I./Ai_driver/inc
LDFLAGS += -L./Ai_driver/lib -l:libNetworkRuntime810_CM7_GCC.a
```

In case of use with **PlatformIO** linkage happens in `platformio.ini` via:

```
build_flags =
  ; ...
  -IAI_driver/inc
  -LAI_driver/lib -llibNetworkRuntime810_CM7_GCC.a
```

This library is subjected to licensing obtainable [here](/AI_driver/LICENSE.txt).
