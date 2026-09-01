# ----- Project Configuration -----

PROJECT         := rogue-vita
PROJECT_TITLE   := Rogue Vita
PROJECT_TITLEID := VSDK00007

MAKEFLAGS += -j$(shell nproc)

# ----- Deployment Configuration -----

VITA3K_FS  := /mnt/c/Users/logan/AppData/Roaming/Vita3K/Vita3K
VITA3K_EXE := /mnt/c/Users/logan/Documents/Vita-Emul/Vita3K.exe
VITA_IP    := 192.168.1.42

# ----- Terminal Colors -----

GREEN  := \033[1;32m
RED    := \033[1;31m
YELLOW := \033[1;33m
RESET  := \033[0m

# ----- Make Helpers -----

.PHONY: all package clean clean-desktop re emul remul run rrun desktop rdesktop test

rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

INCLUDES := -I../common -Iinclude -Idvl/include

# ----- Source Files -----

SRC_C   := $(call rwildcard,src/,*.c)
SRC_CPP := $(call rwildcard,src/,*.cpp)

DVL_VITA_SRC_CPP := $(shell find dvl/src -type f -name '*.cpp' \
	! -path '*/desktop/*' \
	! -path '*/opengl/*')

DVL_DESKTOP_SRC_CPP := $(shell find dvl/src -type f -name '*.cpp' \
	! -path '*/vita/*' \
	! -path '*/vitagl/*')

# ----- Vita Toolchain -----

CC    := arm-vita-eabi-gcc
CXX   := arm-vita-eabi-g++
STRIP := arm-vita-eabi-strip

CFLAGS   := -Wall -Wextra -Werror $(INCLUDES)
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Werror $(INCLUDES)
LDFLAGS  := -Wl,-q

OBJS := \
	$(addprefix out/,$(SRC_C:src/%.c=%.o)) \
	$(addprefix out/,$(SRC_CPP:src/%.cpp=%.o)) \
	$(addprefix out/,$(DVL_VITA_SRC_CPP:%.cpp=%.o))

# ----- Desktop Toolchain -----

DESKTOP_CC  := gcc
DESKTOP_CXX := g++

DESKTOP_CFLAGS   := -Wall -Wextra -Werror -g -O0 $(INCLUDES)
DESKTOP_CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Werror -g -O0 $(INCLUDES)
DESKTOP_LIBS     := -lglfw -lGL -ldl -pthread

DESKTOP_OBJS := \
	$(addprefix out-desktop/,$(SRC_C:src/%.c=%.o)) \
	$(addprefix out-desktop/,$(SRC_CPP:src/%.cpp=%.o)) \
	$(addprefix out-desktop/,$(DVL_DESKTOP_SRC_CPP:%.cpp=%.o))

# ----- Vita Libraries -----

VITA3K ?= 0

ifeq ($(VITA3K),1)
	VITAGL_LIB := -lvitaGL_vita3k
else
	VITAGL_LIB := -lvitaGL
endif

LIBS := \
	$(VITAGL_LIB) \
	-lvitashark \
	-lSceShaccCgExt \
	-lSceShaccCg_stub \
	-ltaihen_stub \
	-lSceCommonDialog_stub \
	-lSceGxm_stub \
	-lSceDisplay_stub \
	-lSceCtrl_stub \
	-lSceAppMgr_stub \
	-lSceKernelDmacMgr_stub \
	-lmathneon \
	-lm \
	-lc

# ----- Asset Packaging -----

SHADER_ASSET_DIR := asset/shader
SHADER_ASSETS := $(shell find $(SHADER_ASSET_DIR) -type f)
SHADER_VPK_ARGS := $(foreach file,$(SHADER_ASSETS),--add $(file)=asset/shader/$(patsubst $(SHADER_ASSET_DIR)/%,%,$(file)))

COOKED_ASSET_DIR := asset/cooked
COOKED_ASSETS := $(shell find $(COOKED_ASSET_DIR) -type f)
COOKED_VPK_ARGS := $(foreach file,$(COOKED_ASSETS),--add $(file)=asset/cooked/$(patsubst $(COOKED_ASSET_DIR)/%,%,$(file)))

# ----- Vita Build -----

all:
	@echo "$(YELLOW)Starting build for $(PROJECT_TITLE)...$(RESET)"
	@$(MAKE) package && \
		echo "$(GREEN)✔ Success: $(PROJECT).vpk built successfully!$(RESET)" || \
		(echo "$(RED)✘ Error: Compilation or packaging failed!$(RESET)"; exit 1)

package: $(PROJECT).vpk

$(PROJECT).vpk: eboot.bin param.sfo $(SHADER_ASSETS) $(COOKED_ASSETS)
	vita-pack-vpk -s param.sfo -b eboot.bin \
		--add sce_sys/icon0.png=sce_sys/icon0.png \
		--add sce_sys/livearea/contents/bg.png=sce_sys/livearea/contents/bg.png \
		--add sce_sys/livearea/contents/startup.png=sce_sys/livearea/contents/startup.png \
		--add sce_sys/livearea/contents/template.xml=sce_sys/livearea/contents/template.xml \
		$(SHADER_VPK_ARGS) \
		$(COOKED_VPK_ARGS) \
		$(PROJECT).vpk

eboot.bin: $(PROJECT).velf
	vita-make-fself $(PROJECT).velf eboot.bin

param.sfo:
	vita-mksfoex -s TITLE_ID="$(PROJECT_TITLEID)" "$(PROJECT_TITLE)" param.sfo

$(PROJECT).velf: $(PROJECT).elf
	$(STRIP) -g $<
	vita-elf-create $< $@

$(PROJECT).elf: $(OBJS)
	$(CXX) $(LDFLAGS) $^ $(LIBS) -o $@

out/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

out/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $<

out/dvl/%.o: dvl/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) -o $@ $<

# ----- Desktop Build -----

desktop: rogue-vita-desktop

rogue-vita-desktop: $(DESKTOP_OBJS)
	$(DESKTOP_CXX) $^ $(DESKTOP_LIBS) -o $@
	@echo "$(GREEN)✔ Desktop build successful!$(RESET)"

out-desktop/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(DESKTOP_CXX) -c $(DESKTOP_CXXFLAGS) -o $@ $<

out-desktop/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(DESKTOP_CC) -c $(DESKTOP_CFLAGS) -o $@ $<

out-desktop/dvl/%.o: dvl/%.cpp
	@mkdir -p $(dir $@)
	$(DESKTOP_CXX) -c $(DESKTOP_CXXFLAGS) -o $@ $<

rdesktop:
	@$(MAKE) clean-desktop
	@$(MAKE) desktop

clean-desktop:
	rm -f rogue-vita-desktop
	rm -rf out-desktop/

# ----- Cleanup and Rebuild -----

clean:
	@echo "$(YELLOW)Cleaning up build artifacts...$(RESET)"
	rm -f *.velf *.elf *.vpk param.sfo eboot.bin rogue-vita-desktop
	rm -rf out/ out-desktop/

re:
	@$(MAKE) clean
	@$(MAKE) VITA3K=0

# ----- Vita3K Deployment -----

emul:
	@$(MAKE) VITA3K=1
	./script/deploy_emul.sh "$(PROJECT).vpk" "$(PROJECT_TITLEID)" "$(VITA3K_FS)" "$(VITA3K_EXE)"

remul:
	@$(MAKE) clean
	@$(MAKE) VITA3K=1
	./script/deploy_emul.sh "$(PROJECT).vpk" "$(PROJECT_TITLEID)" "$(VITA3K_FS)" "$(VITA3K_EXE)"

# ----- PlayStation Vita Deployment -----

run: eboot.bin
	@VITA_IP="$(VITA_IP)" \
	TITLE_ID="$(PROJECT_TITLEID)" \
	EBOOT_PATH="$(CURDIR)/eboot.bin" \
	ASSET_PATH="$(CURDIR)/asset" \
	./script/deploy_vita.sh

rrun:
	@$(MAKE) clean
	@$(MAKE) run VITA3K=0

# ----- Tests -----

test:
	@$(MAKE) -C dvl/tests run; \
		test_status=$$?; \
		$(MAKE) -C dvl/tests clean; \
		exit $$test_status
