# src/CoafLinker/runtime/Makefile

# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>
# AI-Assisted: This file was generated with assistance from KimiAI (https://www.kimi.com)

CC = clang
CFLAGS = -x c++ -fPIC -ffreestanding -fno-exceptions -fno-rtti -O2 -Wall -Wextra -I../include

.PHONY: all clean

all: bin/libcoafcxxrt.a

bin/libcoafcxxrt.a: bin/_start.o bin/newdelete.o bin/cxxabi_minimal.o
	llvm-ar rcs $@ $^

bin/%.o: %.cpp
	@mkdir -p bin
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf bin