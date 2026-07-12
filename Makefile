# You should only use Makefile-based build if you know what you're doing.
# For most vitasdk projects, CMake is a better choice. See CMakeLists.txt for an example.

# --- PROJECT CONFIGURATION ---
PROJECT         := rogue-vita
PROJECT_TITLE   := Rogue Vita
PROJECT_TITLEID := VSDK00007

EMUL_DEST := /mnt/c/Users/logan/Documents/Vita-Game
VITA3K_FS  := /mnt/c/Users/logan/AppData/Roaming/Vita3K/Vita3K
VITA3K_EXE := /mnt/c/Users/logan/Documents/Vita-Emul/Vita3K.exe

# --- ANSI COLOR CODES ---
GREEN        := \033[1;32m
RED          := \033[1;31m
YELLOW       := \033[1;33m
RESET        := \033[0m

.PHONY: all package clean re emul remul

rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

CC := arm-vita-eabi-gcc
CXX := arm-vita-eabi-g++
STRIP := arm-vita-eabi-strip

INCLUDES := -I../common -Iinclude -Idvl/include

CFLAGS += -Wl,-q -Wall -Wextra -Werror $(INCLUDES)
CXXFLAGS += -Wl,-q -std=c++17 -Wall -Wextra -Wpedantic -Werror $(INCLUDES)

SRC_C :=$(call rwildcard, src/, *.c)
SRC_CPP :=$(call rwildcard, src/, *.cpp)
DVL_SRC_CPP :=$(call rwildcard, dvl/src/, *.cpp)

SHADER_ASSET_DIR := asset/shader
SHADER_ASSETS := $(shell find $(SHADER_ASSET_DIR) -type f)
SHADER_VPK_ARGS := $(foreach file,$(SHADER_ASSETS),--add $(file)=assets/shaders/$(patsubst $(SHADER_ASSET_DIR)/%,%,$(file)))

OBJS := $(addprefix out/, $(SRC_C:src/%.c=%.o)) \
		$(addprefix out/, $(SRC_CPP:src/%.cpp=%.o)) \
		$(addprefix out/, $(DVL_SRC_CPP:%.cpp=%.o))
OBJ_DIRS := $(sort $(dir $(OBJS)))

VITA3K ?= 0

ifeq ($(VITA3K),1)
	VITAGL_LIB := -lvitaGL_vita3k
else
	VITAGL_LIB := -lvitaGL
endif

# debug log on screen and vitaGL libs
LIBS += $(VITAGL_LIB) \
		-lvitashark \
		-lSceShaccCgExt \
		-lSceShaccCg_stub \
		-ltaihen_stub \
		-lSceCommonDialog_stub \
		-lSceGxm_stub \
		-lSceDisplay_stub \
		-lSceAppMgr_stub \
		-lSceKernelDmacMgr_stub \
		-lmathneon \
		-lm \
		-lc

# --- MAIN RULE WITH COLOR OVERRAYS ---
all: 
	@echo "$(YELLOW)Starting build for $(PROJECT_TITLE)...$(RESET)"
	@$(MAKE) package && \
		echo "$(GREEN)✔ Success: $(PROJECT).vpk built successfully!$(RESET)" || \
		(echo "$(RED)✘ Error: Compilation or packaging failed!$(RESET)"; exit 1)

package: $(PROJECT).vpk

$(PROJECT).vpk: eboot.bin param.sfo $(SHADER_ASSETS)
	vita-pack-vpk -s param.sfo -b eboot.bin \
		--add sce_sys/icon0.png=sce_sys/icon0.png \
		--add sce_sys/livearea/contents/bg.png=sce_sys/livearea/contents/bg.png \
		--add sce_sys/livearea/contents/startup.png=sce_sys/livearea/contents/startup.png \
		--add sce_sys/livearea/contents/template.xml=sce_sys/livearea/contents/template.xml \
		$(SHADER_VPK_ARGS) \
		$(PROJECT).vpk

eboot.bin: $(PROJECT).velf
	vita-make-fself $(PROJECT).velf eboot.bin

param.sfo:
	vita-mksfoex -s TITLE_ID="$(PROJECT_TITLEID)" "$(PROJECT_TITLE)" param.sfo

$(PROJECT).velf: $(PROJECT).elf
	$(STRIP) -g $<
	vita-elf-create $< $@

$(PROJECT).elf: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

$(OBJ_DIRS):
	mkdir -p $@

out/%.o : src/%.cpp | $(OBJ_DIRS)
	arm-vita-eabi-g++ -c $(CXXFLAGS) -o $@ $<

out/%.o : src/%.c | $(OBJ_DIRS)
	arm-vita-eabi-gcc -c $(CFLAGS) -o $@ $<

out/dvl/%.o : dvl/%.cpp | $(OBJ_DIRS)
	arm-vita-eabi-g++ -c $(CXXFLAGS) -o $@ $<

clean:
	@echo "$(YELLOW)Cleaning up build artifacts...$(RESET)"
	rm -f *.velf *.elf *.vpk param.sfo eboot.bin $(OBJS)
	rm -rf out/

re:
	$(MAKE) clean
	$(MAKE) VITA3K=0

emul:
	$(MAKE) VITA3K=1
	./script/deploy_emul.sh "$(PROJECT).vpk" "$(PROJECT_TITLEID)" "$(VITA3K_FS)" "$(VITA3K_EXE)"

remul:
	$(MAKE) clean
	$(MAKE) VITA3K=1
	./script/deploy_emul.sh "$(PROJECT).vpk" "$(PROJECT_TITLEID)" "$(VITA3K_FS)" "$(VITA3K_EXE)"