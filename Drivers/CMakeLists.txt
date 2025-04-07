# 1. HC32 LL驱动源文件
#----------------------------------------
set(HC32F460_SOURCES
    HC32F460_Driver/source/hc32_ll_adc.c
    HC32F460_Driver/source/hc32_ll_aes.c
    HC32F460_Driver/source/hc32_ll_aos.c
    HC32F460_Driver/source/hc32_ll_can.c
    HC32F460_Driver/source/hc32_ll_clk.c
    HC32F460_Driver/source/hc32_ll_cmp.c
    HC32F460_Driver/source/hc32_ll_crc.c
    HC32F460_Driver/source/hc32_ll_dbgc.c
    HC32F460_Driver/source/hc32_ll_dcu.c
    HC32F460_Driver/source/hc32_ll_dma.c
    HC32F460_Driver/source/hc32_ll_efm.c
    HC32F460_Driver/source/hc32_ll_emb.c
    HC32F460_Driver/source/hc32_ll_event_port.c
    HC32F460_Driver/source/hc32_ll_fcg.c
    HC32F460_Driver/source/hc32_ll_fcm.c
    HC32F460_Driver/source/hc32_ll_gpio.c
    HC32F460_Driver/source/hc32_ll_hash.c
    HC32F460_Driver/source/hc32_ll_i2c.c
    HC32F460_Driver/source/hc32_ll_i2s.c
    HC32F460_Driver/source/hc32_ll_icg.c
    HC32F460_Driver/source/hc32_ll_interrupts.c
    HC32F460_Driver/source/hc32_ll_keyscan.c
    HC32F460_Driver/source/hc32_ll_mpu.c
    HC32F460_Driver/source/hc32_ll_ots.c
    HC32F460_Driver/source/hc32_ll_pwc.c
    HC32F460_Driver/source/hc32_ll_qspi.c
    HC32F460_Driver/source/hc32_ll_rmu.c
    HC32F460_Driver/source/hc32_ll_rtc.c
    HC32F460_Driver/source/hc32_ll_sdioc.c
    HC32F460_Driver/source/hc32_ll_spi.c
    HC32F460_Driver/source/hc32_ll_sram.c
    HC32F460_Driver/source/hc32_ll_swdt.c
    HC32F460_Driver/source/hc32_ll_tmr0.c
    HC32F460_Driver/source/hc32_ll_tmr4.c
    HC32F460_Driver/source/hc32_ll_tmr6.c
    HC32F460_Driver/source/hc32_ll_tmra.c
    HC32F460_Driver/source/hc32_ll_trng.c
    HC32F460_Driver/source/hc32_ll_usart.c
    HC32F460_Driver/source/hc32_ll_utility.c
    HC32F460_Driver/source/hc32_ll_wdt.c
    HC32F460_Driver/source/hc32_ll.c
    HC32F460_Driver/source/hc32f460_ll_interrupts_share.c
)

# 2. 驱动层源文件
#----------------------------------------
set(USER_SOURCES
    USER_Driver/source/24cxx.c
    USER_Driver/source/ev_hc32f460_lqfp100_v2_24cxx.c
    USER_Driver/source/w25qxx.c
    USER_Driver/source/ev_hc32f460_lqfp100_v2_w25qxx.c
    USER_Driver/source/wm8731.c
    USER_Driver/source/ev_hc32f460_lqfp100_v2_wm8731.c
    USER_Driver/source/ev_hc32f460_lqfp100_v2.c
    USER_Driver/source/usartDriver.cpp
    USER_Driver/source/commonDriver.cpp
)

# 3. 合并所有源文件
#----------------------------------------
set(DRIVER_SOURCES
    ${HC32F460_SOURCES}
    ${USER_SOURCES}
)

# 4. 添加源文件到项目
#----------------------------------------
target_sources(${PROJECT_NAME} PRIVATE ${DRIVER_SOURCES})

# 5. 包含目录设置
#----------------------------------------
# 5.1 HC32F460 驱动头文件路径
set(HC32F460_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/HC32F460_Driver/include
)

# 5.2 USER 驱动头文件路径
set(USER_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/USER_Driver/include
)

# 6. 添加所有包含路径
target_include_directories(${PROJECT_NAME} PRIVATE
    ${HC32F460_INCLUDE_DIRS}
    ${USER_INCLUDE_DIRS}
)
