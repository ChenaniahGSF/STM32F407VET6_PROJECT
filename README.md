# STM32F407VET6_PROJECT

**工程概述**
- **目的**: 基于 STM32F407 的固件工程，使用 STM32CubeMX + HAL 生成骨架，集成若干第三方轻量模块（日志、命令行、软件定时、xmodem、EEPROM 仿真等）。
- **适用场景**: 嵌入式设备固件开发、示例与教学、设备引导/升级与外设驱动集成。

**快速导航**
- **入口**: [Core/Src/main.c](Core/Src/main.c#L1)
- **启动文件**: [Core/Startup/startup_stm32f407vetx.s](Core/Startup/startup_stm32f407vetx.s#L1)
- **链接脚本**: [STM32F407VETX_FLASH.ld](STM32F407VETX_FLASH.ld) / [STM32F407VETX_RAM.ld](STM32F407VETX_RAM.ld)
- **Makefile（构建）**: [Debug/makefile](Debug/makefile#L1)

**关键模块与位置**
- **HAL 与外设初始化**: `Core/Inc`、`Core/Src` 中的 `MX_*_Init()` 系列函数由 CubeMX 生成。
- **Third_Party 功能：**
	- `Third_Party/smarttimer` — 轻量级事件/软件定时器框架（轮询驱动）。
	- `Third_Party/logger` — 日志接口与实现。
	- `Third_Party/lwshell` — 简易交互 shell。
	- `Third_Party/xmodem` — XMODEM 文件接收实现。
	- `Third_Party/ee`、`ee24` — EEPROM 仿真/持久化接口。

**构建（Windows，GNU ARM）**
1. 在仓库根运行：

```powershell
cd Debug
make
```

2. 产物：`Debug/STM32F407VET6_PROJECT.elf`（可用 `arm-none-eabi-objcopy` 导出 hex，或用 ST-Link 烧录）。

**调试**
- 推荐使用 STCubeIDE 的 launch 配置文件：`STM32F407VET6_PROJECT Debug.launch`（通过 ST-Link + gdbserver 进行调试）。
- 入口点和初始化顺序位于 [Core/Src/main.c](Core/Src/main.c#L1)。

**CubeMX 与代码生成约定**
- 请勿直接修改 CubeMX 自动生成的初始化区域（注意 `/* USER CODE BEGIN */` / `END` 标记）。
- 如需修改外设配置，请编辑 `STM32F407VET6_PROJECT.ioc` 并用 CubeMX 重新生成代码。

**关于 FreeRTOS 与 smarttimer（设计说明）**
- 工程同时包含轻量级 `smarttimer`（基于轮询）与 FreeRTOS（可选）。如果启用 FreeRTOS，建议优先使用 RTOS 的软件定时器与任务管理来替换或封装 `smarttimer`，以获得线程安全与更好调度控制。若短时间内不迁移，可将 `smarttimer` 放入单独的 FreeRTOS 任务中运行以避免竞态。

**常见操作/命令**
- 构建：

```powershell
cd Debug
make
```

- 导出 hex：

```powershell
arm-none-eabi-objcopy -O ihex Debug/STM32F407VET6_PROJECT.elf STM32F407VET6_PROJECT.hex
arm-none-eabi-size Debug/STM32F407VET6_PROJECT.elf
```

**何处查找实现**
- 主循环与初始化： [Core/Src/main.c](Core/Src/main.c#L1)
- 定时/事件： `Third_Party/smarttimer/smarttimer.c`（`stim_init`, `stim_mainloop`）
- 串口/日志： `Third_Party/logger` 与 `Core/Src/usart.c`
- xmodem： `Third_Party/xmodem`

**贡献与注意事项**
- 提交代码前请保持 CubeMX 生成区不被破坏。
- 如果要迁移 `smarttimer` 到 FreeRTOS，请先在单一任务内封装其主循环，验证无竞态后再逐步替换调用点。

如需我帮忙：可以生成把 `smarttimer` 封装为 FreeRTOS 任务的示例补丁，或直接帮你把 `stim_mainloop()` 从主循环迁移到任务中。
