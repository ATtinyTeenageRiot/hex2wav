SOURCES = main.cpp ../rtaudio-4.1.2/RtAudio.cpp ../duktape-2.0.1/src/duktape.c ../duktape-2.0.1/extras/console/duk_console.c
DEFINES = -DPACKAGE_NAME=\"RtAudio\" -DPACKAGE_TARNAME=\"rtaudio\" -DPACKAGE_VERSION=\"4.1.2\" -DPACKAGE_STRING=\"RtAudio\ 4.1.2\" -DPACKAGE_BUGREPORT=\"gary@music.mcgill.ca\" -DPACKAGE_URL=\"\" -DPACKAGE=\"rtaudio\" -DVERSION=\"4.1.2\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_DLFCN_H=1 -DLT_OBJDIR=\".libs/\" -DSTDC_HEADERS=1 -DHAVE_SYS_IOCTL_H=1 -DHAVE_UNISTD_H=1 -DHAVE_LIBPTHREAD=1 -I.   -D__MACOSX_CORE__
CFLAGS = -W -Wall $(CFLAGS_EXTRA) $(MODULE_CFLAGS) -DDUK_CMDLINE_CONSOLE_SUPPORT
INCLUDES = -I../rtaudio-4.1.2/ -I../duktape-2.0.1/src/ -I../duktape-2.0.1/extras/console -I../soundfile-2.2/include -L../soundfile-2.2/lib
LIBRARY = -lpthread -framework CoreAudio -framework CoreFoundation -lm -lsoundfile


all: $(PROG)

$(PROG): $(SOURCES)
		g++ -x c++ $(INCLUDES) $(DEFINES) $(SOURCES) -o $@ $(CFLAGS) $(LIBRARY)	

clean:
	rm -rf *.gc* *.dSYM *.exe *.obj *.o a.out $(PROG)