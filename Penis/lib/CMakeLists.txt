add_library(penis_lib

    source/read_ir.cpp
    source/write_ir.cpp

    List/source/list_construction.cpp
    List/source/list_error_handler.cpp
    List/source/list_src.cpp
    List/source/list_pop.cpp
    List/source/list_push.cpp

    MyLib/Logger/logging.cpp
    MyLib/Assert/print_error.cpp
    MyLib/My_stdio/my_stdio.cpp
)

target_include_directories(penis_lib
    PUBLIC
        .
        ./include
        ./List/include

        ./MyLib
        ./MyLib/My_stdio
        ./MyLib/Logger
        ./MyLib/Assert
)

target_compile_features (penis_lib
    PRIVATE
        c_std_11
)

target_compile_options (penis_lib
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
            # -flto
            -DNDEBUG
        >
)

target_link_options (penis_lib
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
            # -flto
            -DNDEBUG
        >
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # to generate compile_commands.json

# cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++
# cmake --build build
