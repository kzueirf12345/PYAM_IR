cmake_minimum_required (VERSION 3.20)
project (pyam_ir CXX ASM_NASM)

add_library (pyam_ir
    STATIC

    src/operations.c
)

enable_language (C)

target_include_directories(pyam_ir
    PUBLIC
        ./include
)

target_compile_features (pyam_ir
    PRIVATE
        c_std_11
)

target_compile_options (pyam_ir
    PRIVATE
        -fdiagnostics-color=always

        -Wall
        -Wextra
        -fstack-protector-strong
        -fcheck-new
        -fstrict-overflow

        -march=native

        $<$<CONFIG:Debug>:
            -Og
            -g
            -g3
            -DDEBUG
            -ggdb
            -fsanitize=address,leak,undefined
        >

        $<$<CONFIG:Release>:
            -O2
            -flto
            -DNDEBUG
        >
)

target_link_options (pyam_ir
    PRIVATE
        -march=native

        $<$<CONFIG:Debug>:
            -Og
            -g
            -g3
            -DDEBUG
            -ggdb
            -fsanitize=address,leak,undefined
        >

        $<$<CONFIG:Release>:
            -O2
            -flto
            -DNDEBUG
        >
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # to generate compile_commands.json

# cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++
# cmake --build build
