/*
 *
    Wave Generator for audio bootloader (c) by Christoph Haberer 2011
    HexDecoder in Javascript by Fredrik Olofsson
    C++ port by Budi Prakosa

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
*/

#ifndef HEX2WAV_H
#define HEX2WAV_H

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string.h>

#define signal_type vector<float>

#ifndef NORTAUDIO
#include "hex2wav_signalplayer.h"
#endif

#include "hex2wav_bootframe.h"
#include "hex2wav_hex2signal.h"
#include "hex2wav_hexfiledecoder.h"
#include "hex2wav_wavcodegenerator.h"
#include "hex2wav_signalwriter.h"

#endif // HEX2WAV_H
