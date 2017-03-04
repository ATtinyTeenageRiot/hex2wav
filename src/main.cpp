#include <iostream>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <vector>

#include "RtAudio.h"
#include "duktape.h"
#include "duk_console.h"

#include "hex2wavjs.h"
#include "duktape_fileio.h"

// Platform-dependent sleep routines.
#if defined( __WINDOWS_ASIO__ ) || defined( __WINDOWS_DS__ ) || defined( __WINDOWS_WASAPI__ )
  #include <windows.h>
  #define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds )
#else // Unix variants
  #include <unistd.h>
  #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

#define BASE_RATE 0.005
#define TIME   1.0

struct OutputData {
  unsigned int channels;
  unsigned int count;
  unsigned int index;
};

std::vector<double> hex2wav_audio;

duk_context *ctx;

char * hex2wav_input_filename;
char hex2wav_no_file[] = "";

duk_context * init_duktape()
{
    duk_context *ctx = duk_create_heap_default();
    duk_console_init(ctx, 0 /*flags*/);
    duk_push_c_function(ctx, fileio_read_file, 1 /*nargs*/);
    duk_put_global_string(ctx, "readFile");
    duk_push_c_function(ctx, fileio_write_file, 2 /*nargs*/);
    duk_put_global_string(ctx, "writeFile");

    return ctx;
}


/* Adder: add argument values. */
static duk_ret_t push_audio_array(duk_context *ctx) {
  double audio = (double) duk_to_number(ctx, 0); //get from argument 0
  hex2wav_audio.push_back(audio);
  duk_push_number(ctx, 1);
  return 1;  /* one return value */
}

/* Adder: add argument values. */
static duk_ret_t get_file_argument(duk_context *ctx) {
  duk_push_string(ctx, hex2wav_input_filename);
  return 1;  /* one return value */
}


//// Interleaved buffers
int output( void *outputBuffer, void * /*inputBuffer*/, unsigned int nBufferFrames,
            double /*streamTime*/, RtAudioStreamStatus /*status*/, void *data )
{

  return 0;
}

void setup_duktape()
{
    ctx = init_duktape();
    duk_push_c_function(ctx, push_audio_array, 1);
    duk_put_global_string(ctx, "pushAudio");
    duk_push_c_function(ctx, get_file_argument, 1);
    duk_put_global_string(ctx, "getHexFile");
}

void destroy_duktape()
{
    duk_destroy_heap(ctx);
}


int main(int argc, char* argv[]) {
    setup_duktape();
    //init dac
    RtAudio dac;
    if ( dac.getDeviceCount() < 1 ) {
      std::cout << "\nNo audio devices found!\n";
      exit( 1 );
    }

    if (argc == 2)
    {
        hex2wav_input_filename = argv[1];
    }else{
        hex2wav_input_filename = hex2wav_no_file;
    }

    // Let RtAudio print messages to stderr.
    dac.showWarnings( true );
    duk_eval_string_noresult(ctx, js_file);
    destroy_duktape();
    return 0;
}
