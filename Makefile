CXX = gcc
src = $(wildcard src/*.c)
obj = $(src:.c=.o)

LDFLAGS = -std=c11 -g

hxd: $(obj)
	@mkdir -p bin
	$(CXX) -o bin/$@ $^ $(LDFLAGS) $(OSFLAGS)

clean:
	rm $(obj)
