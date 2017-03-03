SOURCES = main.cpp ../duktape-2.0.1/src/duktape.c ../duktape-2.0.1/extras/console/duk_console.c
CFLAGS = -W -Wall -I../duktape-2.0.1/src/ -I../duktape-2.0.1/extras/console $(CFLAGS_EXTRA) $(MODULE_CFLAGS) -DDUK_CMDLINE_CONSOLE_SUPPORT

all: $(PROG)

$(PROG): $(SOURCES)
		g++ -x c++ $(SOURCES) -o $@ $(CFLAGS) -lm

$(PROG).exe:$(SOURCES)
		cl $(SOURCES) /I../.. /MD /Fe$@

clean:
	rm -rf *.gc* *.dSYM *.exe *.obj *.o a.out $(PROG)
