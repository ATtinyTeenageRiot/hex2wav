
CC=g++

ifeq ($(shell uname), Linux)

else ifeq ($(shell uname), Darwin)
LIBS = -lpthread -framework CoreAudio -framework CoreFoundation -lm
DEFINES = -DPACKAGE_NAME=\"RtAudio\" -DPACKAGE_TARNAME=\"rtaudio\" -DPACKAGE_VERSION=\"4.1.2\" -DPACKAGE_STRING=\"RtAudio\ 4.1.2\" -DPACKAGE_BUGREPORT=\"gary@music.mcgill.ca\" -DPACKAGE_URL=\"\" -DPACKAGE=\"rtaudio\" -DVERSION=\"4.1.2\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_DLFCN_H=1 -DLT_OBJDIR=\".libs/\" -DSTDC_HEADERS=1 -DHAVE_SYS_IOCTL_H=1 -DHAVE_UNISTD_H=1 -DHAVE_LIBPTHREAD=1 -I.   -D__MACOSX_CORE__
else ifeq ($(shell uname), OpenBSD)

else

endif

SOURCES = main.cpp ../rtaudio-4.1.2/RtAudio.cpp
CFLAGS = -W -Wall $(CFLAGS_EXTRA) $(MODULE_CFLAGS)
INCLUDES = -I../rtaudio-4.1.2/
LIBRARY = $(LIBS)

all: $(PROG)

$(PROG): $(SOURCES)
	$(CC) -x c++ $(INCLUDES) $(DEFINES) $(SOURCES) -o $@ $(CFLAGS) $(LIBRARY)	

clean:
	rm -rf *.gc* *.dSYM *.exe *.obj *.o a.out $(PROG)