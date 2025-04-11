# 1. 包含源文件设置
#----------------------------------------

# 应用源文件
set(APP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/command_processor_c.c    
    ${CMAKE_CURRENT_SOURCE_DIR}/source/messageBusProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/uartProtocolProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/usartTask.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/buttonControl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source/commonControlTask.c
    ${CMAKE_CURRENT_SOURCE_DIR}/taskFactory.c
)

# 2. 添加源文件到项目
#----------------------------------------
target_sources(${PROJECT_NAME} PRIVATE
    ${APP_SOURCES}
)

# 3. 包含头文件设置
#----------------------------------------

# 应用头文件目录
set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 4. 添加头文件到项目
#----------------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
    ${INCLUDE_DIRS}
)


