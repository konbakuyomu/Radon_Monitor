cmake_minimum_required(VERSION 3.25)  
# ------------------------------------------------------------------------------
# 1. 基础项目设置
# ------------------------------------------------------------------------------
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)  
set(PROJECT_NAME HC32F460_Project)  
# ------------------------------------------------------------------------------
# 2. 芯片架构配置
# ------------------------------------------------------------------------------
set(CMAKE_SYSTEM_NAME Generic)  
set(CMAKE_SYSTEM_PROCESSOR Cortex-M4.fp.sp)    # 与 HC32L072 不同
# ------------------------------------------------------------------------------
# 3. 工具链配置（使用 ARMCLANG 工具链）
# ------------------------------------------------------------------------------
file(TO_CMAKE_PATH "D:/Keil_v5/ARM/ARMCLANG/bin" COMPILER_ROOT_PATH)  
find_program(ARM_CC armclang.exe PATHS ${COMPILER_ROOT_PATH} REQUIRED)  
find_program(ARM_CXX armclang.exe PATHS ${COMPILER_ROOT_PATH} REQUIRED)    # C++ 同样使用 armclang
find_program(ARM_AS armasm.exe PATHS ${COMPILER_ROOT_PATH} REQUIRED)  
find_program(ARM_LINKER armlink.exe PATHS ${COMPILER_ROOT_PATH} REQUIRED)  
find_program(ARM_FROMELF fromelf.exe PATHS ${COMPILER_ROOT_PATH} REQUIRED)  
set(CMAKE_C_COMPILER   ${ARM_CC})  
set(CMAKE_CXX_COMPILER ${ARM_CXX})  
set(CMAKE_ASM_COMPILER ${ARM_AS})  
set(CMAKE_LINKER       ${ARM_LINKER})  
# ------------------------------------------------------------------------------
# 4. 链接脚本设置
# ------------------------------------------------------------------------------
set(SECTIONS_SCRIPT_PATH "${CMAKE_SOURCE_DIR}/core/linker/HC32F460xE.sct")  
# ------------------------------------------------------------------------------
# 5. 针对芯片的编译选项设置
# ------------------------------------------------------------------------------
set(C_TARGET_FLAG   "--target=arm-arm-none-eabi -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -Oz -flto")
set(CXX_TARGET_FLAG "--target=arm-arm-none-eabi -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -Oz -flto")
set(ASM_TARGET_FLAG "--cpu Cortex-M4.fp.sp --li")
# ------------------------------------------------------------------------------
# 6. 编译规则设置
# ------------------------------------------------------------------------------
# 注意：此处添加了 -D__UVISION_VERSION="538"，与 Keil 的编译参数一致
set(CMAKE_C_FLAGS "${C_TARGET_FLAG} \
    -xc -std=c11 \
    -mlittle-endian \
    -ffunction-sections -fdata-sections \
    -DUSE_DDL_DRIVER -D__DEBUG \
    -DHC32F460"
)

set(CMAKE_CXX_FLAGS "${CXX_TARGET_FLAG} \
    -xc++ -std=c++14 \
    -mlittle-endian \
    -ffunction-sections -fdata-sections \
    -DHC32F460 -DUSE_DDL_DRIVER -D__DEBUG \
    -fno-exceptions -fno-rtti"
)
set(CMAKE_ASM_FLAGS "${ASM_TARGET_FLAG} -g \
    --apcs=interwork \
    --pd \"__UVISION_VERSION SETA 538\""
)  
# ------------------------------------------------------------------------------
# 7. 链接器选项设置
# ------------------------------------------------------------------------------
add_link_options(
    --cpu=Cortex-M4.fp.sp
    --strict
    --scatter "${SECTIONS_SCRIPT_PATH}"
    --summary_stderr
    --info=summarysizes,sizes,totals,unused,veneers
    --map
    --lto
    --load_addr_map_info
    --xref
    --callgraph
    --symbols
    --keep=*Handler
)  
# ------------------------------------------------------------------------------
# 8. 项目初始化及全局设置
# ------------------------------------------------------------------------------
set(CMAKE_C_COMPILER_FORCED TRUE)  
set(CMAKE_CXX_COMPILER_FORCED TRUE)  
set(CMAKE_ASM_COMPILER_FORCED TRUE)  
project(${PROJECT_NAME} LANGUAGES C CXX ASM)  
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  
set(CMAKE_C_STANDARD 11)  
set(CMAKE_CXX_STANDARD 14)  
# ------------------------------------------------------------------------------
# 9. 可执行目标与源文件配置
# ------------------------------------------------------------------------------
add_executable(${PROJECT_NAME})  
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")  
target_sources(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/Core/startup/startup_hc32f460.s")  
# ------------------------------------------------------------------------------
# 10. 添加子目录（仅包含子目录，子目录中已配置各自的包含和编译规则）
# ------------------------------------------------------------------------------
add_subdirectory(Core)  
add_subdirectory(Drivers)  
add_subdirectory(Board_Support_Package)  
add_subdirectory(Middlewares)  
add_subdirectory(Configs)  
add_subdirectory(Source)  
# ------------------------------------------------------------------------------
# 11. 输出文件设置与后处理
# ------------------------------------------------------------------------------
set(MAP_FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")  
set(HEX_FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex")  
set(BIN_FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin")  
set(ELF_FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}.elf")  
# 使用 fromelf.exe 进行输出文件转换
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${ARM_FROMELF} --i32combined --output="${HEX_FILE}" "$<TARGET_FILE:${PROJECT_NAME}>"
    COMMAND ${ARM_FROMELF} --bin "$<TARGET_FILE:${PROJECT_NAME}>" -o "${BIN_FILE}"
    COMMAND ${ARM_FROMELF} --text -c --output=- "$<TARGET_FILE:${PROJECT_NAME}>" > "${PROJECT_NAME}.list"
    COMMENT "Building ${HEX_FILE}, ${BIN_FILE} and generating listing"
) 