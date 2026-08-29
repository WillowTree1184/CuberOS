# src/CoafLinker/Makefile

# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>
# AI-Assisted: This file was generated with assistance from KimiAI (https://www.kimi.com)

L_CXX = clang++
L_CXXFLAGS = -std=c++20 -O2 -Wall -Wextra -Iinclude -flto=thin

SRCS = src/main.cpp \
       src/ElfReader.cpp \
       src/SectionClassifier.cpp \
       src/SymbolTable.cpp \
       src/LayoutEngine.cpp \
       src/RelocProcessor.cpp \
       src/CoafWriter.cpp \
       src/Validator.cpp

OBJS = $(patsubst src/%.cpp,build/%.o,$(SRCS))

.PHONY: build clean

build: bin/coaf-ld runtime

bin/coaf-ld: $(OBJS)
	@mkdir -p bin
	$(L_CXX) $(L_CXXFLAGS) -o $@ $^

build/%.o: src/%.cpp
	@mkdir -p build
	$(L_CXX) $(L_CXXFLAGS) -c -o $@ $<

runtime:
	$(MAKE) -C runtime

clean:
	rm -rf build bin
	$(MAKE) -C runtime clean