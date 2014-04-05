CPP = g++
CFLAGS = -g -c
LD = g++
LDFLAGS = -lglut -lGLU -lGL -lm
OBJS = main.o core.o drawing.o pathfinder.o parser.o

rsff: $(OBJS)
	$(LD) -o rsff $(OBJS) $(LDFLAGS)

%.o: %.cxx %.hxx
	$(CPP) -o $@ $(CFLAGS) $<

clean:
	rm -f *.o rsff
