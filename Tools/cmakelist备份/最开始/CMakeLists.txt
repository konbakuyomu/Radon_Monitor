cmake_minimum_required(VERSION 3.25)

# ------------------------------------------------------------------------------
# 1. 基础项目设置
# ------------------------------------------------------------------------------

# 指定交叉编译时生成静态库（防止试编译出可执行文件时出错）
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# 项目名称
set(PROJECT_NAME HC32F460_Project)

# ------------------------------------------------------------------------------
# 2. 芯片架构配置
# ------------------------------------------------------------------------------

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR Cortex-M4.fp.sp)  # 注意这里与HC32L072不同

# ------------------------------------------------------------------------------
# 3. 工具链配置（使用 ARMCLANG 工具链）
# ------------------------------------------------------------------------------

# 工具链根目录
file(TO_CMAKE_PATH "D:/Keil_v5/ARM/ARMCLANG/bin" COMPILER_ROOT_PATH)

# 查找各个工具程序
find_program(ARM_CC armclang.exe PATHS ${COMPILER_ROOT_PATH} REQUIRED)
find_program(ARM_CXX armclang.exe PATHS ${COMPILER_ROOT_PATH} REQUIRED) # C++也使用armclang
find_program(ARM_AS armasm.exe PATHS ${COMPILER_ROOT_PATH} REQUIRED)
find_program(ARM_LINKER armlink.exe PATHS ${COMPILER_ROOT_PATH} REQUIRED)
find_program(ARM_FROMELF fromelf.exe PATHS ${COMPILER_ROOT_PATH} REQUIRED)

# 指定编译器和链接器
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

# 设置与芯片对应的编译标志（ARMCLANG 风格）
set(C_TARGET_FLAG   "--target=arm-arm-none-eabi -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard")
set(CXX_TARGET_FLAG "--target=arm-arm-none-eabi -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard")
set(ASM_TARGET_FLAG "--cpu Cortex-M4.fp.sp --li")

# ------------------------------------------------------------------------------
# 6. 编译规则设置
# ------------------------------------------------------------------------------

# C 语言编译选项
set(CMAKE_C_FLAGS_INIT "${C_TARGET_FLAG} \
    -xc -std=c11 \
    -mlittle-endian -gdwarf-4 -O0 \
    -ffunction-sections \
    -fno-rtti -funsigned-char -fshort-enums -fshort-wchar \
    -Wno-packed -Wno-missing-variable-declarations -Wno-missing-prototypes \
    -Wno-missing-noreturn -Wno-sign-conversion -Wno-nonportable-include-path \
    -Wno-reserved-id-macro -Wno-unused-macros -Wno-documentation-unknown-command \
    -Wno-documentation -Wno-license-management -Wno-parentheses-equality -Wno-reserved-identifier \
    -DHC32F460 -DUSE_DDL_DRIVER -D__DEBUG"
)

# C++ 语言编译选项
set(CMAKE_CXX_FLAGS_INIT "${CXX_TARGET_FLAG} \
    -xc++ -std=c++14 \
    -mlittle-endian -gdwarf-4 -O0 \
    -ffunction-sections \
    -fno-rtti -funsigned-char -fshort-enums -fshort-wchar \
    -Wno-packed -Wno-missing-variable-declarations \
    -Wno-missing-noreturn -Wno-sign-conversion -Wno-nonportable-include-path \
    -Wno-reserved-id-macro -Wno-unused-macros -Wno-documentation-unknown-command \
    -Wno-documentation -Wno-license-management -Wno-parentheses-equality -Wno-reserved-identifier \
    -DHC32F460 -DUSE_DDL_DRIVER -D__DEBUG"
)

# 汇编器选项
set(CMAKE_ASM_FLAGS_INIT "${ASM_TARGET_FLAG} -g \
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

# 添加可执行目标
add_executable(${PROJECT_NAME})

# 设置输出文件名
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")

# 添加启动文件
target_sources(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/Core/startup/startup_hc32f460.s")

# ------------------------------------------------------------------------------
# 10. 添加子目录（按照指定顺序）
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


# 输出文件转换：使用 fromelf.exe 将 .axf (ELF) 文件转换为 .hex 和 .bin 文件
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${ARM_FROMELF} --i32combined --output="${HEX_FILE}" "$<TARGET_FILE:${PROJECT_NAME}>"
    COMMAND ${ARM_FROMELF} --bin "$<TARGET_FILE:${PROJECT_NAME}>" -o "${BIN_FILE}"
    COMMAND ${ARM_FROMELF} --text -c --output=- "$<TARGET_FILE:${PROJECT_NAME}>" > "${PROJECT_NAME}.list"
    COMMENT "Building ${HEX_FILE}, ${BIN_FILE} and generating listing"
)