L_CXX = clang++
L_L_CXXFLAGS = -std=c++20 -O2 -Wall -Wextra -Iinclude

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
	$(L_CXX) $(L_L_CXXFLAGS) -o $@ $^

build/%.o: src/%.cpp
	@mkdir -p build
	$(L_CXX) $(L_L_CXXFLAGS) -c -o $@ $<

runtime:
	$(MAKE) -C runtime

clean:
	rm -rf build bin
	$(MAKE) -C runtime clean