CXX = gcc
src = $(wildcard src/*.c)
obj = $(src:.c=.o)

LDFLAGS = -std=c11 -O3
OSFLAGS = -lm

hxd: $(obj)
	@mkdir -p bin
	$(CXX) -o bin/$@ $^ $(LDFLAGS) $(OSFLAGS)

install:
	@cp bin/hxd /usr/local/bin/hxd

uninstall:
	@rm /usr/local/bin/hxd

clean:
	rm $(obj)
