set(CMAKE_SYSTEM_NAME               Generic)
set(CMAKE_SYSTEM_PROCESSOR          arm)

set(CMAKE_EXECUTABLE_SUFFIX_ASM     ".axf")
set(CMAKE_EXECUTABLE_SUFFIX_C       ".axf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX     ".axf")

set(ARM_COMPILER_TOOL "armcc") # armcc or armclang
# set(ARMCC_TOOLCHAIN_DIR "D:/Develop/MDK-ARM/v535/ARM/ARMCC/bin")
# set(ARMCLANG_TOOLCHAIN_DIR "D:/Develop/MDK-ARM/v535/ARM/ARMCLANG/bin")

# 判断 ARM_COMPILER_TOOL 的值
if(ARM_COMPILER_TOOL STREQUAL "armcc")


### 以下是 ARMCC 相关的设置或操作

    message(STATUS "使用的是 ARMCC 编译器")
    # 可以在这里添加与 ARMCC 相关的设置或操作
    # # 判断 ARMCC_TOOLCHAIN_DIR 是否为空
    if(DEFINED ARMCC_TOOLCHAIN_DIR AND NOT "${ARMCC_TOOLCHAIN_DIR}" STREQUAL "")
        message(STATUS "ARMCC_TOOLCHAIN_DIR is set to: ${ARMCC_TOOLCHAIN_DIR}")
        # 在这里可以执行与 ARMCC_TOOLCHAIN_DIR 相关的操作
        find_program(ARMCC_COMPILER armcc PATHS ${ARMCC_TOOLCHAIN_DIR})
        set(CMAKE_C_COMPILER ${ARMCC_COMPILER})
        set(CMAKE_CXX_COMPILER ${ARMCC_COMPILER})
        find_program(CMAKE_AR armar PATHS ${ARMCC_TOOLCHAIN_DIR})
        find_program(CMAKE_LINKER armlink PATHS ${ARMCC_TOOLCHAIN_DIR})
        find_program(CMAKE_FROMELF fromelf PATHS ${ARMCC_TOOLCHAIN_DIR})
        set(CMAKE_OBJCOPY           ${CMAKE_FROMELF})
        set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
        if(NOT EXISTS "${CMAKE_FROMELF}")
            message(STATUS ${CMAKE_FROMELF})
            message(FATAL_ERROR "fromelf not found")
        endif()
    else()
        message(STATUS "usage armcc")
        # 在这里可以指定默认路径或者处理错误
        set(CMAKE_C_COMPILER        armcc)
        set(CMAKE_ASM_COMPILER      ${CMAKE_C_COMPILER})
        set(CMAKE_CXX_COMPILER      ${CMAKE_C_COMPILER})
        set(CMAKE_LINKER            armlink)
        set(CMAKE_OBJCOPY           fromelf)
        set(CMAKE_SIZE              ${CMAKE_OBJCOPY} --text -z)
        set(CMAKE_AR                armar)
    endif()

    set(IGNORE_PARAMS 
        "--diag_suppress 111,177,178,1295,188"
    )

    # MCU specific flags
    set(TARGET_FLAGS "--cpu Cortex-M0+")

    set(CMAKE_C_FLAGS
        "--c99 -c  ${TARGET_FLAGS} -g -O2 ${IGNORE_PARAMS}"
    )
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --apcs=interwork --split_sections")
    set(CMAKE_CXX_FLAGS "${TARGET_FLAGS} --cpp11 ${IGNORE_PARAMS}")
    set(CMAKE_ASM_FLAGS "${TARGET_FLAGS} -c")

    # set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} --scatter=${CMAKE_SOURCE_DIR}/py32f002bx5.sct")
    add_link_options(
        --scatter=${CMAKE_SOURCE_DIR}/py32f002bx5.sct
    )


elseif(ARM_COMPILER_TOOL STREQUAL "armclang")

### 以下是 ARMCLANG 相关的设置或操作

    message(STATUS "使用的是 ARMCLANG 编译器")
    # 可以在这里添加与 ARMCLANG 相关的设置或操作
    # # 判断 ARMCC_TOOLCHAIN_DIR 是否为空
    if(DEFINED ARMCC_TOOLCHAIN_DIR AND NOT "${ARMCC_TOOLCHAIN_DIR}" STREQUAL "")
        message(STATUS "ARMCC_TOOLCHAIN_DIR is set to: ${ARMCC_TOOLCHAIN_DIR}")
        # 在这里可以执行与 ARMCC_TOOLCHAIN_DIR 相关的操作
        find_program(ARMCC_COMPILER armcc PATHS ${ARMCC_TOOLCHAIN_DIR})
        set(CMAKE_C_COMPILER ${ARMCC_COMPILER})
        set(CMAKE_CXX_COMPILER ${ARMCC_COMPILER})
        find_program(CMAKE_AR armar PATHS ${ARMCC_TOOLCHAIN_DIR})
        find_program(CMAKE_LINKER armlink PATHS ${ARMCC_TOOLCHAIN_DIR})
        find_program(CMAKE_FROMELF fromelf PATHS ${ARMCC_TOOLCHAIN_DIR})
        set(CMAKE_OBJCOPY           ${CMAKE_FROMELF})
        set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
        if(NOT EXISTS "${CMAKE_FROMELF}")
            message(STATUS ${CMAKE_FROMELF})
            message(FATAL_ERROR "fromelf not found")
        endif()
    else()
        message(STATUS "usage armclang")
        # 在这里可以指定默认路径或者处理错误
        set(CMAKE_C_COMPILER        armclang)
        set(CMAKE_ASM_COMPILER      armasm)
        set(CMAKE_CXX_COMPILER      ${CMAKE_C_COMPILER})
        set(CMAKE_LINKER            armlink)
        set(CMAKE_OBJCOPY           fromelf)
        set(CMAKE_SIZE              ${CMAKE_OBJCOPY} --text)
        set(CMAKE_AR                armar)
    endif()

    set(IGNORE_PARAMS 
        # "--diag_suppress 177,178,1295,188"
    )

    # MCU specific flags
    set(TARGET_FLAGS "-mcpu=cortex-m0plus")

    set(CMAKE_C_FLAGS "-xc -std=c99 --target=arm-arm-none-eabi ${TARGET_FLAGS} ${IGNORE_PARAMS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -c -fno-rtti -funsigned-char -fshort-enums -fshort-wchar")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -gdwarf-4 -Os -ffunction-sections -Wno-packed -Wno-missing-variable-declarations ")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-missing-prototypes -Wno-missing-noreturn -Wno-sign-conversion -Wno-nonportable-include-path -Wno-reserved-id-macro ")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-macros -Wno-documentation-unknown-command -Wno-documentation -Wno-license-management -Wno-parentheses-equality")
    set(CMAKE_CXX_FLAGS "--target=arm-arm-none-eabi ${TARGET_FLAGS} ${IGNORE_PARAMS}")
    set(CMAKE_ASM_FLAGS "--cpu cortex-m0plus")

    # set(CMAKE_C_LINK_FLAGS "${TARGET_FLAGS}")
    # set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} --strict --summary_stderr")
    # set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} --info sizes,totals,unused,veneers --load_addr_map_info --xref --callgraph --symbols")
    # set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} --scatter=${CMAKE_SOURCE_DIR}/py32f002bx5.sct")


    add_link_options(
        # -c -fno-rtti -funsigned-char -fshort-enums -fshort-wchar
        # -gdwarf-4 -Os -ffunction-sections -Wno-packed -Wno-missing-variable-declarations 
        # -Wno-missing-prototypes -Wno-missing-noreturn -Wno-sign-conversion -Wno-nonportable-include-path -Wno-reserved-id-macro
        # -Wno-unused-macros -Wno-documentation-unknown-command -Wno-documentation -Wno-license-management -Wno-parentheses-equality
        --scatter=${CMAKE_SOURCE_DIR}/py32f002bx5.sct
    )



else()
    message(WARNING "未识别的编译器工具: ${ARM_COMPILER_TOOL}")
endif()




