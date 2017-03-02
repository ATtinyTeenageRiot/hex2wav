SOURCES = main.cpp ../v7/v7.c
CFLAGS = -W -Wall -I../v7 $(CFLAGS_EXTRA) $(MODULE_CFLAGS)

all: $(PROG)

$(PROG): $(SOURCES)
		g++ -x c++ $(SOURCES) -o $@ $(CFLAGS) -lm

$(PROG).exe:$(SOURCES)
		cl $(SOURCES) /I../.. /MD /Fe$@

clean:
	rm -rf *.gc* *.dSYM *.exe *.obj *.o a.out $(PROG)
