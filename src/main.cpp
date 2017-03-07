#include <stdio.h>
#include <errno.h>
#include "hex2wav.h"

using namespace std;

char * hex2wav_input_filename;
char hex2wav_no_file[] = "";

int main(int argc, char* argv[]) {

    hexFileDecoder hexDec;
    WavCodeGenerator wg;
    SignalPlayer signalPlayer;

    if (argc == 2)
    {
        hex2wav_input_filename = argv[1];
    }else{
        hex2wav_input_filename = hex2wav_no_file;
    }

    std::cout << "converting hex to wav..\n";

    vector<int> hex_decoded = hexDec.decodeHex(string(hex2wav_input_filename));
    signal_type hex_signal = wg.generateSignal(&hex_decoded);

    std::cout << "convert done..\n";

    printf("filename: %s\n", hex2wav_input_filename);
    printf("signal size: %i\n", (int) hex_signal.size());

    signalPlayer.playSignal(&hex_signal);


    return 0;
}
