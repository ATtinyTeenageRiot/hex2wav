#ifndef HEX2WAV_SIGNALWRITER_H
#define HEX2WAV_SIGNALWRITER_H

#include <stdlib.h>

using namespace std;

template <typename T>
void write(std::ofstream& stream, const T& t) {
    stream.write((const char*)&t, sizeof(T));
}

class SignalWriter
{
private:

public:

    void writeWavFromSignal(vector<float> sig, string fileName )
    {
        writeWAVData<float>(fileName, 44100, 1, sig);
    }

    template <typename SampleType>
    void writeWAVData(string outFile, int sampleRate, short channels, vector<float> sig)
    {
        std::ofstream stream(outFile.c_str(), std::ios::binary);                // Open file stream at "outFile" location

        int bufSize = (int) sig.size() * channels * sizeof(SampleType);

        /* Header */
        stream.write("RIFF", 4);                                        // sGroupID (RIFF = Resource Interchange File Format)
        write<int>(stream, 36 + bufSize);                               // dwFileLength
        stream.write("WAVE", 4);                                        // sRiffType

        /* Format Chunk */
        stream.write("fmt ", 4);                                        // sGroupID (fmt = format)
        write<int>(stream, 16);                                         // Chunk size (of Format Chunk)
        write<short>(stream, 3);                                        // Format (1 = PCM)
        write<short>(stream, channels);                                 // Channels
        write<int>(stream, sampleRate);                                 // Sample Rate
        write<int>(stream, sampleRate * channels * sizeof(SampleType)); // Byterate
        write<short>(stream, channels * sizeof(SampleType));            // Frame size aka Block align
        write<short>(stream, 8 * sizeof(SampleType));                   // Bits per sample

        /* Data Chunk */
        stream.write("data", 4);                                        // sGroupID (data)
        stream.write((const char*)&bufSize, 4);                         // Chunk size (of Data, and thus of bufferSize)
        stream.write((const char*)&sig[0], bufSize);                        // The samples DATA!!!
    }

};

#endif // HEX2WAV_SIGNALWRITER_H
