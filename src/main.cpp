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


//// Interleaved buffers
//int output( void *outputBuffer, void * /*inputBuffer*/, unsigned int nBufferFrames,
//            double /*streamTime*/, RtAudioStreamStatus /*status*/, void *data )
//{
//  OutputData *oData = (OutputData*) data;

//  // In general, it's not a good idea to do file input in the audio
//  // callback function but I'm doing it here because I don't know the
//  // length of the file we are reading.
//  unsigned int count = fread( outputBuffer, oData->channels * sizeof( MY_TYPE ), nBufferFrames, oData->fd);
//  if ( count < nBufferFrames ) {
//    unsigned int bytes = (nBufferFrames - count) * oData->channels * sizeof( MY_TYPE );
//    unsigned int startByte = count * oData->channels * sizeof( MY_TYPE );
//    memset( (char *)(outputBuffer)+startByte, 0, bytes );
//    return 1;
//  }

//  return 0;
//}

int main(void) {
    duk_context *ctx = init_duktape();

    duk_push_c_function(ctx, push_audio_array, DUK_VARARGS);
    duk_put_global_string(ctx, "pushAudio");

    //init dac
    RtAudio dac;
    if ( dac.getDeviceCount() < 1 ) {
      std::cout << "\nNo audio devices found!\n";
      exit( 1 );
    }

    // Let RtAudio print messages to stderr.
    dac.showWarnings( true );



    duk_eval_string_noresult(ctx, js_file);


    duk_destroy_heap(ctx);
    return 0;
}
