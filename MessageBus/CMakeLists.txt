# 1. 包含源文件设置
#----------------------------------------

# 应用源文件
set(MessageBus_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/messageBus.c
)

# 2. 添加源文件到项目
#----------------------------------------
target_sources(${PROJECT_NAME} PRIVATE
    ${MessageBus_SOURCES}
)

# 3. 包含头文件设置
#----------------------------------------

# 应用头文件目录
set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

# 4. 添加头文件到项目
#----------------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
    ${INCLUDE_DIRS}
)


