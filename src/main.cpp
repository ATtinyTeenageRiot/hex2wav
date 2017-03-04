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

std::vector<double> hex2wav_audio;

duk_context *ctx;

char * hex2wav_input_filename;
char hex2wav_no_file[] = "";

RtAudio * adc;

struct HexAudioData {
  unsigned int channels;
  std::vector<double> hex2wav_audio;
  int frameCounter;
};

HexAudioData hexAudioData;


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
  hexAudioData.hex2wav_audio.push_back(audio);
  duk_push_number(ctx, 1);
  return 1;  /* one return value */
}

/* Adder: add argument values. */
static duk_ret_t get_file_argument(duk_context *ctx) {
  duk_push_string(ctx, hex2wav_input_filename);
  return 1;  /* one return value */
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

// Interleaved buffers
int pulse( void *outputBuffer, void * /*inputBuffer*/, unsigned int nBufferFrames,
           double /*streamTime*/, RtAudioStreamStatus /*status*/, void *data )
{
  // Write out a pulse signal and ignore the input buffer.
  HexAudioData *hexData = (HexAudioData *) data;

  unsigned int i, j, buffSize, sampleSize;

  float sample;
  float *buffer = (float *) outputBuffer;

  buffSize = nBufferFrames;
  sampleSize = (int) hexData->hex2wav_audio.size();

 if ((sampleSize - hexData->frameCounter) < nBufferFrames)
    buffSize = sampleSize - hexData->frameCounter;

  //if ( status ) std::cout << "Stream over/underflow detected!" << std::endl;

//  printf("----------\n");
//  printf("frame %i\n" , hexData->frameCounter);
//  printf("buffSize %i\n" , buffSize);
//  printf("nBufferFrames %i\n" , nBufferFrames);
//  printf("sampleSize %i\n" , sampleSize);
//  printf("----------\n");

  for ( i=0; i <buffSize; i++ ) {
    //printf("%i sample %f\n",data->frameCounter, data->hex2wav_audio.at(data->frameCounter));
    sample = hexData->hex2wav_audio.at(hexData->frameCounter);
    //interleaved
    for ( j=0; j<hexData->channels; j++ ) *buffer++ = sample;
    //accumulate frame
    hexData->frameCounter++;
  }

  if (buffSize < nBufferFrames)
  {
      printf("stop");
      return 1;
  }else{
      return 0;
  }
}


int main(int argc, char* argv[]) {
    setup_duktape();
    //init dac

    RtAudio *adc = new RtAudio();

    if ( adc->getDeviceCount() < 1 ) {
      std::cout << "\nNo audio devices found!\n";
      exit( 1 );
    }

    // Let RtAudio print messages to stderr.
    adc->showWarnings( true );

    if (argc == 2)
    {
        hex2wav_input_filename = argv[1];
    }else{
        hex2wav_input_filename = hex2wav_no_file;
    }

    duk_eval_string_noresult(ctx, js_file);

    RtAudio::StreamParameters oParams, iParams;
    oParams.deviceId = adc->getDefaultOutputDevice();
    oParams.nChannels = 2;
    oParams.firstChannel = 0;

    unsigned int bufferFrames = 512;

    //rtaudio playback
    hexAudioData.frameCounter = 0;
    hexAudioData.channels = 2;

    adc->openStream( &oParams, NULL, RTAUDIO_FLOAT32, 44100, &bufferFrames, &pulse, (void *)&hexAudioData );
    adc->startStream();

    while ( adc->isStreamRunning() ) SLEEP( 5 );
    std::cout << "stream stopped via callback return value = 1.\n";
    SLEEP( 0.1 );

    if ( adc && adc->isStreamOpen() ) adc->closeStream();
    if ( adc ) delete adc;

    destroy_duktape();
    return 0;
}
