vpath %.cpp src
vpath %.c src
vpath %.o obj

WARNINGS=-Wall -Wextra -Wimport -Wundef -Wunused-macros -Wendif-labels -Woverloaded-virtual -Wparentheses -W -Wfloat-equal -Wshadow -Wpointer-arith\
 -Wcast-qual -Wcast-align -Wwrite-strings -Wmissing-noreturn -Wmissing-format-attribute -Wpacked -Wunreachable-code -Winline -Wdisabled-optimization -pedantic -Wfatal-errors\
 -Werror -Wno-error=shadow -Wno-error=unused-parameter -Wno-error=unused-but-set-variable -Wno-error=unused-variable -Wno-error=unused-macros -Wno-error=inline

OBJS=$(addprefix obj/,\
 main.o\
 Tile.o\
 TileEdge.o\
 Grid.o\
 Position.o)
 
INCLUDES=-Iinclude -I/usr/include/libpng -isystem include/external

LINKAGES=-lpng

ifeq ($(BUILD),debug)   
# "Debug" build - no optimization, and debugging symbols
CFLAGS = -O0 -g
else
# "Release" build - optimization, and no debug symbols
CFLAGS = -O2 -s -DNDEBUG
endif

CPPFLAGS=$(CFLAGS)
CPPFLAGS+=-std=c++2a

GPP=g++-8

all: main
	
debug:
	make "BUILD=debug"
	
main: $(OBJS)
	$(GPP) -o $@ $^ -no-pie $(CPPFLAGS) $(LINKAGES)
	chmod +x main
	
%.x: %.o
	$(GPP) -o $@ $(OBJS) -no-pie $(CPPFLAGS) $(LINKAGES)
	chmod +x $@

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)
	
obj/%.o: %.cpp
	$(GPP)  -c src/$*.cpp $(CPPFLAGS) $(INCLUDES) $(WARNINGS) -o $@
	echo -n "obj/" > obj/$*.d
	$(GPP) -MM -c $^ $(CPPFLAGS) $(INCLUDES) >> obj/$*.d
	
# obj/%.o: %.c
# 	gcc -c src/$*.c $(CFLAGS) $(INCLUDES) -o $@
# 	echo -n "obj/" > obj/$*.d
# 	gcc -MM -c $^ $(CFLAGS) $(INCLUDES) >> obj/$*.d

new: clean main
	
clean:
	-rm $(OBJS) main obj/*.d