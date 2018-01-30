#include <stdio.h>
#include <errno.h>
#include "hex2wav.h"

using namespace std;

string hex2wav_input_filename;
string hex2wav_output_filename;

/*funcion that show the help information*/
void showCreditInfo()
{
  cout<<"" << endl;
  cout<<" Hex2Wav e.·´¯`·.´¯`·.¸¸.·´¯`·.¸><(((º> build " << BUILD_HASH << " "<<endl;
  cout<<"" << endl;
  cout<<" Small program for uploading hex file to TinyAudioBoot"<<endl;
  cout<<" Original Java code by Chris Haberer, Hex decoder by Fredrik Olofsson, C++ port by Budi Prakosa"<<endl;
  cout<<"" << endl;
  cout<<"   Bootloader: https://github.com/ChrisMicro/TinyAudioBoot"<<endl;
  cout<<"   Hex2Wav: https://github.com/ATtinyTeenageRiot/hex2wav"<<endl;
  cout<<"   Part of: https://github.com/8BitMixtape/8Bit-Mixtape-NEO"<<endl;
  cout<<"" << endl;
}

void showhelpinfo(char *s)
{    
  cout<<"   Usage:   "<<s<<" [-option] <input hex file> <output wav file>"<<endl;
  cout<<"   option:  "<<"--no-sound: no sound output                     "<<endl;
  cout<<"            "<<"--no-file: no file output                       "<<endl;
  cout<<"            "<<"--dump: output signal to stdout                 "<<endl;
  cout<<"            "<<"--debug: show debug message                     "<<endl;
  cout<<"            "<<"--dump-hex: show hex on console                 "<<endl;
  cout<<"   example: "<<s<<" test.hex test.wav                           "<<endl;
  cout<<"            "<<s<<" --debug test.hex                            "<<endl<<endl;
}

void showDone()
{
    string str ="\n\n"
                 "    /\\___/\\     MI\n"
                 "   |       |    MI\n"
                 "  _  *   *  _\n"
                 "  -   /_\\   -\n"
                 "      ---\n\n"
                 "\nHex2Wav done..\n\n";
    cout << str;
}

int main(int argc, char* argv[]) {

    hexFileDecoder hexDec;
    WavCodeGenerator waveGen;

    #ifndef NORTAUDIO
    SignalPlayer signalPlayer;
    #endif

    SignalWriter signalWriter;

    int arg_pointer = 1;
    int found_opt = 0;
    int last_argc = 0;

    bool no_sound = false;
    bool no_file = false;
    bool debug = false;
    bool dump = false;
    bool dump_hex = false;
    bool zip_out = false;


    while (arg_pointer < argc) {
      if (strcmp(argv[arg_pointer], "--no-sound") == 0) {
            no_sound = true;
            found_opt++;
      }
      if (strcmp(argv[arg_pointer], "--no-file") == 0) {
            no_file = true;
            found_opt++;
      }
      if (strcmp(argv[arg_pointer], "--debug") == 0) {
            debug = true;
            found_opt++;
      }
      if (strcmp(argv[arg_pointer], "--dump") == 0) {
            dump = true;
            found_opt++;
      }
      if (strcmp(argv[arg_pointer], "--dump-hex") == 0) {
            dump_hex = true;
            found_opt++;
      }
      if (strcmp(argv[arg_pointer], "--zip-out") == 0) {
            zip_out = true;
            found_opt++;
      }      
      arg_pointer += 1;
    }

    last_argc = arg_pointer - found_opt - 1;

    if(!dump) showCreditInfo();

    if (last_argc == 1)
    {
        hex2wav_input_filename = string(argv[arg_pointer-1]);
        hex2wav_output_filename = hex2wav_input_filename;
        hex2wav_output_filename.append(".wav");
    } else if (last_argc == 2) {
        hex2wav_input_filename = argv[arg_pointer-2];
        hex2wav_output_filename = argv[arg_pointer-1];
    } else {
        showhelpinfo(argv[0]);
        exit(1);
    }

    if(no_sound && no_file) exit(1);

    if(!dump)
    {
        printf("input: <%s> ", hex2wav_input_filename.c_str());
        printf("output: <%s>\n", hex2wav_output_filename.c_str());
    }

    waveGen.setDebug(debug);

    vector<int> hex_decoded = hexDec.decodeHex(string(hex2wav_input_filename));
    signal_type hex_signal = waveGen.generateSignal(&hex_decoded);

    if(!dump)
    {
        printf("input hex size: %i\n", (int) hex_decoded.size());
        printf("signal size: %i\n", (int) hex_signal.size());
    }

    if (!no_file) signalWriter.writeWavFromSignal(hex_signal, hex2wav_output_filename);

    #ifndef NORTAUDIO
    if (!no_sound) signalPlayer.playSignal(&hex_signal);
    #endif

    if (dump) {
        for (int i = 0; i < (int) hex_signal.size(); i++)
        {
            printf("%f\n", hex_signal.at(i));
        }
    }


    // Dump hex file, for web platform
    if(dump_hex) {

       ifstream inFile;
       string x;

       inFile.open(hex2wav_input_filename);

       if (!inFile) {
            cout << "Unable to open file";
            exit(1); // terminate with error
       }

       cout << endl << ">>BEGIN_HEX<<" << endl;
       while (inFile >> x) {
           cout << x << "|" ;
       }
       cout << endl << ">>END_HEX<<" << endl;

       inFile.close();
    }

    if(!no_file && zip_out) {

        #ifdef ISLINUX

        #ifdef LINUX64
        string command = "./zip_64 -j ";
        #else
        string command = "./zip_32 -j ";
        #endif
        
        #else
        string command = "zip -j ";
        #endif
        string zipout = hex2wav_input_filename;
        zipout.append(".zip");
        
        command.append(" ");
        command.append(zipout);
        command.append(" ");
        command.append(hex2wav_input_filename);
        command.append(" ");
        command.append(hex2wav_output_filename);

        system(command.c_str());
    }

    if(!dump) showDone();

    return 0;
}
