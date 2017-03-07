#ifndef HEX2WAV_SIGNALPLAYER_H
#define HEX2WAV_SIGNALPLAYER_H

#include "RtAudio.h"

// Platform-dependent sleep routines.
#if defined( __WINDOWS_ASIO__ ) || defined( __WINDOWS_DS__ ) || defined( __WINDOWS_WASAPI__ )
  #include <windows.h>
  #define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds )
#else // Unix variants
  #include <unistd.h>
  #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

using namespace std;


struct HexAudioData {
  unsigned int channels;
  vector<float> hex2wav_audio;
  int frameCounter;
};

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

class SignalPlayer
{
private:
    RtAudio * adc;
    RtAudio::StreamParameters oParams, iParams;
    unsigned int bufferFrames;
public:
    HexAudioData hexAudioData;

    SignalPlayer()
    {
        //init dac
        adc = new RtAudio();
        if ( adc->getDeviceCount() < 1 ) {
          std::cout << "\nNo audio devices found!\n";
          exit( 1 );
        }

        // Let RtAudio print messages to stderr.
        adc->showWarnings( true );


        oParams.deviceId = adc->getDefaultOutputDevice();
        oParams.nChannels = 2;
        oParams.firstChannel = 0;

        //init hex audio data
        hexAudioData.frameCounter = 0;
        hexAudioData.channels = 2;

        bufferFrames = 512;
    }

    ~SignalPlayer()
    {
        delete adc;
    }

    void playSignal(const vector<float> * sig)
    {
        for (int i = 0; i< (int) sig->size(); i++) {
            //printf("%.0f\n", hex_signal.at(i));
            hexAudioData.hex2wav_audio.push_back(sig->at(i));
        }

        adc->openStream( &this->oParams, NULL, RTAUDIO_FLOAT32, 44100, &this->bufferFrames, pulse, (void *)&hexAudioData );
        adc->startStream();

        while ( adc->isStreamRunning() ) SLEEP( 5 );

        SLEEP( 0.1 );

        if ( adc && adc->isStreamOpen() ) adc->closeStream();
        //if ( adc ) delete adc;
    }

};

#endif // HEX2WAV_SIGNALPLAYER_H
