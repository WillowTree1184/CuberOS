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