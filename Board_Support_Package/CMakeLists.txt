# 1. 包含源文件设置
#----------------------------------------

# BSP板级支持包源文件
set(BSP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/cpuUtilization.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/hal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/drvHal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/ledConfig.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/keyConfig.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/usart1Config.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/heartbeatTaskConfig.c
)

# 2. 添加源文件到项目
#----------------------------------------
target_sources(${PROJECT_NAME} PRIVATE ${BSP_SOURCES})

# 3. 包含头文件设置
#----------------------------------------

# BSP板级支持包头文件路径
set(BSP_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 4. 添加头文件到项目
#----------------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
    ${BSP_INCLUDE_DIRS}
)
