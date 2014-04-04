CPP = g++
CFLAGS = -g -c
LD = g++
LDFLAGS = 
OBJS = main.o core.o drawing.o pathfinder.o

rsff: $(OBJS)
	$(LD) -o rsff $(OBJS) $(LDFLAGS)

%.o: %.cxx %.hxx
	$(CPP) -o $@ $(CFLAGS) $<

clean:
	rm -f *.o rsff
