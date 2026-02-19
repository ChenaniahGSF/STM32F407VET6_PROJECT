<!-- Copilot instructions for STM32F407VET6_PROJECT -->
# 快速上手（给 AI 代理）

- **构建**: 在 `Debug` 目录运行 `make`（使用 GNU ARM 工具链）。See [Debug/makefile](Debug/makefile#L1).
- **工具链**: 依赖 `arm-none-eabi-gcc` / `arm-none-eabi-objcopy` / `arm-none-eabi-objdump` / `arm-none-eabi-size`（makefile header 显示为 GNU Tools for STM32 13.3.rel1）。
- **链接脚本**: 项目根有 `STM32F407VETX_FLASH.ld`（和 `STM32F407VETX_RAM.ld`），makefile 将其作为链接脚本。[STM32F407VETX_FLASH.ld](STM32F407VETX_FLASH.ld)
- **调试**: 项目包含 STCubeIDE 的 launch 文件 `STM32F407VET6_PROJECT Debug.launch`，使用 ST-Link / gdbserver，程序入口设置为 `main`。[STM32F407VET6_PROJECT Debug.launch](STM32F407VET6_PROJECT%20Debug.launch)

# 大体架构（必须知道的「为什么」与边界）

- 这是一个基于 STM32F4 HAL 的固件工程（CubeMX 生成的骨架）。主要目录：
  - [Core/Src/main.c](Core/Src/main.c#L1) — 应用入口，按固定顺序调用 `MX_*_Init()` 初始化外设，并在主循环中运行 `stim_mainloop()`、`xmodem_receive_mainfuncion()` 等。
  - [Core/Inc](Core/Inc) / [Core/Src](Core/Src) — HAL 外设封装与用户代码片段（注意 `/* USER CODE BEGIN/END */` 分界）。
  - [Drivers/STM32F4xx_HAL_Driver](Drivers/STM32F4xx_HAL_Driver) — HAL 实现。
  - [Third_Party](Third_Party) — 项目的外部模块集合（`logger`, `lwshell`, `smarttimer`, `xmodem`, `ee`/`ee24` 等），功能基本由这些模块提供。

- 设计要点/边界：CubeMX 负责生成外设初始化和 Make 工程（不要直接修改自动生成的 `Debug/makefile` 或 `Core/Startup` 启动文件；更改外设配置请修改 `STM32F407VET6_PROJECT.ioc` 并由 CubeMX 重新生成）。[STM32F407VET6_PROJECT.ioc](STM32F407VET6_PROJECT.ioc)

# 关键模块与交互点（可直接定位修改或调试）

- `stim_*`（事件/定时器框架）: 在 `Third_Party/smarttimer` 中实现；`main.c` 在启动前调用 `stim_init()` 并在循环中调用 `stim_mainloop()` — 负责轻量级定时事件。参见 [Third_Party/smarttimer/smarttimer.c](Third_Party/smarttimer/smarttimer.c#L450).
- `logger`: 全局日志接口（初始化于 `main.c`），检查 `Third_Party/logger` 子目录。
- `xmodem`: 文件传输接收在主循环中被周期性触发（`xmodem_receive_mainfuncion()`），实现位于 `Third_Party/xmodem`。
- EEPROM/存储：`Third_Party/ee` 与 `ee24` 提供持久化/仿真 EEPROM 功能，`main.c` 中有 `ee_test()` 调用用于示例/自检。
- 交互 shell：`lwshell` 与 `MultiButton` 提供命令行与按键输入的整合，位于 `Third_Party/lwshell` 和 `Third_Party/MultiButton`。

# 项目约定与编码模式（AI 应遵守）

- 生成/手写代码分离：不要修改自动生成的区域（`/* USER CODE BEGIN */` / `END` 外），也不要直接编辑 `Debug/makefile` 或 `Core/Startup/startup_*.s`。对外设或 pin 的改动，通过 `STM32F407VET6_PROJECT.ioc` + CubeMX 重新生成。
- 外设初始化函数命名为 `MX_<Periph>_Init()`（例如 `MX_USART1_UART_Init()`），通常在 `Core/Src` 下的对应 `.c` 文件中。
- 中断/回调：HAL 回调（例如 `HAL_TIM_PeriodElapsedCallback`）用于时基与按键节拍，重要处理放在 `USER CODE` 区块内。

# 构建/调试常用命令示例

- 在 Windows 开发机（仓库根）使用 GNU ARM 工具链：
```
cd Debug
make
```
- 产物 `STM32F407VET6_PROJECT.elf` 位于 `Debug`，可用 `arm-none-eabi-objcopy` 导出 hex，或用 STCubeIDE launch 通过 ST-Link 下载。

# 何处寻找实现与快速定位

- 启动流程：查看 [Core/Src/main.c](Core/Src/main.c#L1)（init 顺序、主循环）。
- 定时/事件：`Third_Party/smarttimer`（`stim_init`, `stim_mainloop`）。
- 串口/日志：`Third_Party/logger` 与 `Core/Src/usart.c`。
- 设备树/引脚：CubeMX `.ioc` 文件与 `Core/Inc/main.h`（引脚宏示例在 [Core/Inc/main.h](Core/Inc/main.h#L1)）。

# 不要假设/限制（给 AI 的注意事项）

- 仓库没有通用的闪存脚本或 CI，只包含 STCubeIDE 的 launch 配置；不要自动尝试 upload/flash 除非用户授权并提供工具/目标信息。
- 不要更改 CubeMX 生成的项目结构，除非同时更新 `.ioc` 并确认生成步骤。

# 需要用户确认或补充的点

- 首次运行需要在目标机器上安装 `arm-none-eabi` 工具链；请确认开发者使用的具体版本/flashing 工具（OpenOCD / st-flash / ST-Link CLI / STCubeIDE）。
- 是否要把一个简单的 `flash` 或 `openocd` 脚本加入仓库？如果需要，我可以草拟并提交。

---

请审阅上述内容有哪些遗漏或不准确的地方（例如你实际使用的烧录/调试工具、期望的 CI 流程），我会据此迭代更新此文件。 
