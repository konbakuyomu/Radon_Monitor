# 1. 包含源文件设置
#----------------------------------------

# QF_LIB 库源文件
set(QF_LIB_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/qf_lib/source/wire.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qf_lib/source/bmp280.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qf_lib/source/appManage.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qf_lib/source/button.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qf_lib/source/keyValueMap.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qf_lib/source/keyValueTransation.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qf_lib/source/beatTask.c
    ${CMAKE_CURRENT_SOURCE_DIR}/qf_lib/source/ringBuffer.c
)

# FreeRTOS内核源文件
set(FREERTOS_CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/freeRTOS/source/tasks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freeRTOS/source/queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freeRTOS/source/list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freeRTOS/source/event_groups.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freeRTOS/source/timers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freeRTOS/source/stream_buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freeRTOS/source/croutine.c
)

# FreeRTOS端口源文件
set(FREERTOS_PORT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/freeRTOS/portable/MemMang/heap_4.c
    ${CMAKE_CURRENT_SOURCE_DIR}/freeRTOS/portable/GCC/ARM_CM4F/port.c
)

# 2. 合并所有源文件
#----------------------------------------
set(MIDDLEWARE_SOURCES
    ${QF_LIB_SOURCES}
    ${FREERTOS_CORE_SOURCES}
    ${FREERTOS_PORT_SOURCES}
)

# 3. 添加源文件到项目
#----------------------------------------
target_sources(${PROJECT_NAME} PRIVATE ${MIDDLEWARE_SOURCES})

# 4. 包含头文件设置
#----------------------------------------

# QF_LIB 头文件路径
set(QF_LIB_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/qf_lib/include
)

# FreeRTOS头文件路径
set(FREERTOS_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/freeRTOS/portable/GCC/ARM_CM4F
    ${CMAKE_CURRENT_SOURCE_DIR}/freeRTOS/include
)

# 5. 添加头文件到项目
#----------------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
    ${QF_LIB_INCLUDE_DIRS}    
    ${FREERTOS_INCLUDE_DIRS}
)