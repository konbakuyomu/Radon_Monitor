# Technical Context: 氡气监测系统
*Version: 1.0*
*Created: 2024-07-07*
*Last Updated: 2024-07-07*

## Technology Stack
- **微控制器**: HC32F460, ARM Cortex-M4 内核, 主频96MHz
- **操作系统**: FreeRTOS v10.4.6, 实时多任务操作系统
- **编程语言**: C/C++ 混合编程
- **硬件抽象层**: HC32F460 LL库 + 自定义HAL层
- **通信协议**: 氡监测仪以太网通信协议(串口传输)
- **调试工具**: SEGGER J-Link
- **开发环境**: Keil MDK + Cursor IDE

## Development Environment Setup
1. **硬件环境**
   - HC32F460-LQFP100开发板
   - SEGGER J-Link调试器
   - USB-UART转换器（CH340或CP2102）

2. **软件环境**
   - 安装Keil MDK v5.35或更高版本
   - 安装HC32F460设备支持包
   - 安装SEGGER J-Link驱动程序和软件
   - 配置CMake构建系统
   - 安装Cursor IDE和C/C++扩展

3. **环境变量配置**
   - 设置KEIL_TOOL_PATH环境变量指向Keil安装目录
   - 设置HC32_SDK_PATH环境变量指向HC32F460 SDK目录

4. **项目设置**
   - 克隆代码仓库
   - 执行CMake配置: `cmake -B build -G "Unix Makefiles"`
   - 构建项目: `cmake --build build`

## Dependencies
- **HC32F460 LL库**: v1.0.0 - 华大半导体提供的底层驱动库
- **FreeRTOS**: v10.4.6 - 实时操作系统内核
- **qf_lib**: v1.2.0 - 自定义的嵌入式框架库
- **beatTask**: v1.0.0 - 简化定时任务创建和管理的库
- **MessageBus**: v1.0.0 - 自定义的消息总线实现
- **SEGGER RTT**: v7.54 - 实时调试和跟踪输出库

## Technical Constraints
- **内存限制**: ROM 512KB, RAM 128KB
- **功耗要求**: 低功耗模式下电流小于10mA
- **实时性要求**: 关键任务响应时间小于1ms
- **可靠性要求**: 无看门狗复位的连续运行时间大于30天
- **通信速率**: UART通信波特率115200bps
- **硬件接口**: UART、I2C、SPI、GPIO
- **外部时钟**: 8MHz晶振

## Build and Deployment
- **构建过程**:
  1. 使用CMake生成构建系统
  2. 执行编译生成.hex和.bin文件
  3. 生成固件包并计算校验和

- **部署流程**:
  1. 连接J-Link调试器到目标板
  2. 使用J-Flash工具烧录固件
  3. 复位设备并验证启动过程
  4. 执行基本功能测试

- **CI/CD设置**:
  - 使用GitHub Actions进行自动构建和单元测试
  - 代码提交触发静态代码分析
  - 发布标签触发固件构建和版本发布

## Testing Approach
- **单元测试**: 使用Catch2测试框架进行C++部分的单元测试
- **集成测试**: 使用自定义测试脚本进行系统集成测试
- **模拟器测试**: 使用QEMU模拟HC32F460进行部分功能测试
- **硬件测试**: 使用实际硬件进行完整功能验证
- **长期稳定性测试**: 在实验室环境下进行7天连续运行测试
- **性能测试**: 测量关键操作的CPU占用率和响应时间

---

*This document describes the technologies used in the project and how they're configured.* 