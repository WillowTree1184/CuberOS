# src/Preloader/Makefile

# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

ARCH		?= x86_64
BUILD_ROOT	?= $(CURDIR)/../../build/$(ARCH)

SRC			?= $(CURDIR)
INC_ROOT	:= $(SRC)/Headers

LDFLAGS		:= -entry:Main

.PHONY: all build clean

all: build clean

# Build
build: Build/Preloader.app
	@mkdir -p "$(BUILD_ROOT)/Preloader"
	@cp -r Build/* "$(BUILD_ROOT)/Preloader"

Build/%.app: %.cpp
	@mkdir -p Build
	$(CXX) "$<" -o "$@"

# Clean
clean:
	@rm -f "Launcher.o" "$(TARGET_EFI)" "$(TARGET_LIB)"