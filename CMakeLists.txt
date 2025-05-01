add_library (PYAM_IR_lib
    STATIC

    src/operations.c
)

target_include_directories(PYAM_IR_lib
    PUBLIC
        .
        ./src
)

target_compile_features (PYAM_IR_lib
    PRIVATE
        cxx_std_17
)

target_compile_options (PYAM_IR_lib
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

target_link_options (PYAM_IR_lib
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
