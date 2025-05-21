add_executable(middleend

    source/main_middleend.cpp

    source/middleend.cpp
    source/dead_code_elim.cpp
    source/subexpression.cpp
    source/error_handler.cpp

    source/parse_flags_lang.cpp
)

target_link_libraries(middleend
    penis_lib
)

target_include_directories(middleend
    PUBLIC
        ./include

        ./MyLib
        ./MyLib/My_stdio
        ./MyLib/Logger
        ./MyLib/Assert

        ../include
)

target_compile_features (middleend
    PRIVATE
        c_std_11
)

target_compile_options (middleend
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

target_link_options (middleend
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
