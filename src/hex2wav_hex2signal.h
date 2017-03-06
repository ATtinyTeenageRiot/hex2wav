#ifndef HEX2SIGNAL_H
#define HEX2SIGNAL_H

using namespace std;

class HexToSignal {
private:
    int startSequencePulses;
    int numStartBits;
    int numStopBits;
    int manchesterPhase;    // double / current phase for differential manchester coding
    bool invertSignal; // correction of an inverted audio signal line
    int lowNumberOfPulses; // not for manchester coding, only for flankensignal
    int highNumberOfPulses; // not for manchester coding, only for flankensignal
    int manchesterNumberOfSamplesPerBit; // this value must be even
    bool useDifferentialManchsterCode;

public:
    HexToSignal(bool fullSpeedFlag = false) {

        startSequencePulses = 40;
        numStartBits        =  1;
        numStopBits         =  1;
        manchesterPhase     =  1;    // double / current phase for differential manchester coding
        invertSignal        =  true; // correction of an inverted audio signal line
        lowNumberOfPulses   =  2; // not for manchester coding, only for flankensignal
        highNumberOfPulses  =  3; // not for manchester coding, only for flankensignal
        manchesterNumberOfSamplesPerBit = 4; // this value must be even
        useDifferentialManchsterCode = true;

        setSignalSpeed(fullSpeedFlag);
    }

    signal_type getDifferentialManchesterCodedSignal(vector<int>* hexdata)
    {

        int counter = 0;
        int laenge = (int) hexdata->size();

        vector<float> signal((laenge*8)*this->manchesterNumberOfSamplesPerBit);

        for (int count = 0; count < laenge; count++) {
            int dat = hexdata->at(count);
            for (int n =0; n<8; n++) {

                if((dat&0x80)==0)
                {
                    this->manchesterEdge(false,counter,&signal); // low bit
                }
                else
                {
                    this->manchesterEdge(true,counter,&signal); // high bit
                }

                counter += this->manchesterNumberOfSamplesPerBit;
                dat = dat << 1;
            }
        }

        return signal;
    }

    void manchesterEdge(bool flag, int pointerIntoSignal, vector<float>* signal)
    {

        int value = 0;
        if( !this->useDifferentialManchsterCode ) // non differential manchester code
        {
            if(flag)
                value=1;
            else
                value=-1;

            if(this->invertSignal) value = value * -1;  // correction of an inverted audio signal line

            for(int n = 0; n < this->manchesterNumberOfSamplesPerBit; n++)
            {
                if( n < this->manchesterNumberOfSamplesPerBit/2 )
                    signal->at(pointerIntoSignal) = -value;
                else
                    signal->at(pointerIntoSignal) = value;

                pointerIntoSignal++;
            }
        }
        else // differential manchester code ( inverted )
        {
            if(flag) this->manchesterPhase = -(this->manchesterPhase); // toggle phase
            for(int n = 0; n < this->manchesterNumberOfSamplesPerBit; n++)
            {
                if( n == (this->manchesterNumberOfSamplesPerBit/2)) this->manchesterPhase = -(this->manchesterPhase); // toggle phase
                signal->at(pointerIntoSignal)=this->manchesterPhase;
                pointerIntoSignal++;
            }
        }
    }

    signal_type manchesterCoding(int hexdata[], int hexsize)
    {
        int laenge = hexsize;
        vector<float> signal((1+startSequencePulses+laenge*8)*manchesterNumberOfSamplesPerBit);
        int counter = 0;

        for (int n = 0; n < startSequencePulses; n++) {
                manchesterEdge(false,counter,&signal); // 0 bits: generate falling edges
                counter+=manchesterNumberOfSamplesPerBit;
        }

        manchesterEdge(true,counter,&signal); //  1 bit:  rising edge
        counter+=manchesterNumberOfSamplesPerBit;

        /** create data signal **/
        for(int count=0; count<laenge; count++)
        {
            int dat = hexdata[count];
            //System.out.println(dat);
            /** create one byte **/
            for(int n=0;n<8;n++) // first bit to send: MSB
            {
                if((dat&0x80)==0)
                {
                    this->manchesterEdge(false,counter,&signal); // generate falling edges ( 0 bits )
                }
                else
                {
                    this->manchesterEdge(true,counter,&signal); // rising edge ( 1 bit )
                }
                counter+=manchesterNumberOfSamplesPerBit;
                dat=dat<<1; // shift to next bit
            }
        }
        return signal;
    }


    void setSignalSpeed(bool fullSpeedFlag)
    {
        if( fullSpeedFlag )
            manchesterNumberOfSamplesPerBit = 4; // full speed
        else
            manchesterNumberOfSamplesPerBit = 8; // half speed
    }


};


#endif // HEX2SIGNAL_H
