# 1. 添加源文件到项目
#----------------------------------------
target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/globalConfig.c
)

# 2. 添加头文件目录
#----------------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)