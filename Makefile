# Makefile

# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

export CC := clang
export LD := lld-link

export SRC_DIR	:= $(CURDIR)/src

export BUILDED_DIR_NAME := Builded

export LAUNCHER_DIR	:= $(SRC_DIR)/Launcher
MODULE_DIRS			:= $(LAUNCHER_DIR)

export PUBLISH_OUTPUT_DIR	:= $(SRC_DIR)/Builded
export SYSFS_DIR			:= $(PUBLISH_OUTPUT_DIR)/System

.PHONY: all
all: build

.PHONY: build
build: $(MODULE_DIRS)

.PHONY: $(MODULE_DIRS)
$(MODULE_DIRS):
	@$(MAKE) -C $@

.PHONY: publish
publish: build
	@$(MAKE) -C $(SRC_DIR)/Publish

.PHONY: run
run: publish
	@$(MAKE) -C $(SRC_DIR)/Run