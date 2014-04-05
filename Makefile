CPP = g++
CFLAGS = -g -c
LD = g++
LDFLAGS = -lglut -lGLU -lGL -lm
OBJS = main.o core.o drawing.o pathfinder.o parser.o

rsff.out: $(OBJS)
	$(LD) -o rsff.out $(OBJS) $(LDFLAGS)

%.o: %.cxx %.hxx
	$(CPP) -o $@ $(CFLAGS) $<

clean:
	rm -f *.o rsff.out
