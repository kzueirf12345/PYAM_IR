.PHONY: all build clean rebuild \
		logger_build logger_clean logger_rebuild \
		stack_build stack_clean stack_rebuild	\
		clean_all clean_log clean_out clean_obj clean_deps clean_txt clean_bin clean_a
# precompile

PROJECT_NAME = pyam_ir

BUILD_DIR = ./build
INCLUDE_DIR = ./include
SRC_DIR = ./src
COMPILER = gcc

DEBUG_ ?= 1

ifeq ($(origin FLAGS), undefined)

FLAGS =	-Wall -Wextra -Waggressive-loop-optimizations \
		-Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts \
		-Wconversion -Wempty-body -Wfloat-equal \
		-Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op \
		-Wopenmp-simd -Wpacked -Wpointer-arith -Winit-self \
		-Wredundant-decls -Wshadow -Wsign-conversion \
		-Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods \
		-Wsuggest-final-types -Wswitch-default -Wswitch-enum -Wsync-nand \
		-Wundef -Wunreachable-code -Wunused -Wvariadic-macros \
		-Wno-missing-field-initializers -Wno-narrowing -Wno-varargs \
		-Wstack-protector -fcheck-new -fstack-protector -fstrict-overflow \
		-flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=81920 -Wstack-usage=81920 -pie \
		-fPIE -Werror=vla \

SANITIZER = -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,$\
		integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,$\
		shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

DEBUG_FLAGS = -D _DEBUG  -ggdb -Og -g3 -D_FORTIFY_SOURCES=3 $(SANITIZER)
RELEASE_FLAGS = -DNDEBUG -O2

ifneq ($(DEBUG_),0)
FLAGS += $(DEBUG_FLAGS)
else
FLAGS += $(RELEASE_FLAGS)
endif

endif

FLAGS += $(ADD_FLAGS)

LIBS =

DIRS =
BUILD_DIRS = $(DIRS:%=$(BUILD_DIR)/%)

SOURCES = operations.c

SOURCES_REL_PATH = $(SOURCES:%=$(SRC_DIR)/%)
OBJECTS_REL_PATH = $(SOURCES:%.c=$(BUILD_DIR)/%.o)
DEPS_REL_PATH = $(OBJECTS_REL_PATH:%.o=%.d)


all: build

build: lib$(PROJECT_NAME).a

lib$(PROJECT_NAME).a: $(OBJECTS_REL_PATH)
	ar -rcs lib$(PROJECT_NAME).a $(OBJECTS_REL_PATH) $(LIBS)

$(BUILD_DIR)/%.o : ./$(SRC_DIR)/%.c | ./$(BUILD_DIR)/ $(BUILD_DIRS)
	@$(COMPILER) $(FLAGS) -I$(INCLUDE_DIR) -c -MMD -MP $< -o $@		# Ебать ты рофл -I SRC_DIR

-include $(DEPS_REL_PATH)

$(BUILD_DIRS):
	mkdir $@

./$(BUILD_DIR)/:
	mkdir $@

clean_all: clean logger_clean stack_clean

clean: clean_obj clean_deps clean_out clean_a

clean_log:
	rm -rf ./log/*

clean_out:
	rm -rf ./*.out

clean_obj:
	rm -rf ./$(OBJECTS_REL_PATH)

clean_deps:
	rm -rf ./$(DEPS_REL_PATH)

clean_txt:
	rm -rf ./*.txt

clean_bin:
	rm -rf ./*.bin

clean_a:
	rm -rf ./*.a
