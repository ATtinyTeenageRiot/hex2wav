#ifndef WAVCODEGENERATOR_H
#define WAVCODEGENERATOR_H

using namespace std;


class WavCodeGenerator {
private:
    int sampleRate;
    bool fullSpeedFlag;
    bool debug;
    BootFrame frameSetup;
public:

    WavCodeGenerator()
    {
        sampleRate = 44100;
        fullSpeedFlag = true;
        debug = false;
    }


    void setDebug(bool debug)
    {
        this->debug = debug;
    }

    void appendSignal(signal_type * sig1, signal_type * sig2)
    {
        int l2 = (int) sig2->size();
        for(int n=0;n<l2;n++) sig1->push_back(sig2->at(n));
    }

    void setSignalSpeed (bool fullSpeedFlag)
    {
        this->fullSpeedFlag = fullSpeedFlag;
    };


    signal_type generatePageSignal(signal_type * data)
    {
            HexToSignal * h2s = new HexToSignal(fullSpeedFlag);

            int frameData[this->frameSetup.getFrameSize()];

            int data_size = (int) data->size();

            // copy data into frame data
            for(int n=0; n < this->frameSetup.getPageSize(); n++)
            {
                if ( n < data_size ) frameData[ n + this->frameSetup.getPageStart() ] = data->at(n);
                else frameData[ n + this->frameSetup.getPageStart() ] = 0xFF;
            }

            frameSetup.addFrameParameters(frameData);

            signal_type signal = h2s->manchesterCoding(frameData, this->frameSetup.getFrameSize());

            delete h2s;
            return signal;
    };


    signal_type silence(float duration)
    {
            signal_type signal(duration * sampleRate);
            int signal_size = (int) signal.size();

            for (int i = 0; i < signal_size; i++) {
                signal.at(i) = 0;
            }

            return signal;
    };


    signal_type makeRunCommand()
    {
            HexToSignal * h2s= new HexToSignal(fullSpeedFlag);
            int frameData[frameSetup.getFrameSize()];

            //need to zero all array members
            for(int i = 0; i < frameSetup.getFrameSize(); i++)
            {
                frameData[i] = 0;
            }

            frameSetup.setRunCommand();
            frameSetup.addFrameParameters(frameData);

            signal_type signal = h2s->manchesterCoding(frameData, frameSetup.getFrameSize());

            delete h2s;
            return signal;
    };

    signal_type makeTestCommand()
    {
            HexToSignal * h2s= new HexToSignal(fullSpeedFlag);
            int frameData[frameSetup.getFrameSize()];

            //need to zero all array members
            for(int i = 0; i < frameSetup.getFrameSize(); i++)
            {
                frameData[i] = 0;
            }

            frameSetup.setTestCommand();
            frameSetup.addFrameParameters(frameData);

            signal_type signal = h2s->manchesterCoding(frameData, frameSetup.getFrameSize());

            delete h2s;
            return signal;
    };

    signal_type generateSignal(vector<int>* data)
    {

        signal_type signal;

        int pl = frameSetup.getPageSize();
        int total = (int) data->size();
        int sigPointer=0;
        int pagePointer=0;

        frameSetup.setProgCommand(); // we want to programm the mc

        while(total>0)
        {
            frameSetup.setPageIndex(pagePointer++);
            frameSetup.setTotalLength(data->size());

            signal_type partSig(pl);

            for(int n=0; n < pl; n++)
            {
                if(n+sigPointer> (int) data->size()-1) partSig[n]=0xFF;
                else partSig[n]=data->at(n+sigPointer);
            }

            sigPointer += pl;

            signal_type sig = generatePageSignal(&partSig);

//            cout << "tot:" << total << "\n";
//            for (int i = 0;i<sig.size();i++) {
             //   printf("%0.f\n", sig.at(i));
//            }

            appendSignal(&signal,&sig);

            signal_type silence = this->silence(frameSetup.getSilenceBetweenPages());

//			printf("siglen silence %i\n", (int)silence.size() );

            appendSignal(&signal, &silence);

            if(this->debug) printf("getSilenceBetweenPages: %i\n", (int) (frameSetup.getSilenceBetweenPages() * sampleRate));
            if(this->debug) printf("append silence: %i\n", (int)silence.size() );
            if(this->debug) printf("siglen: %i\n", (int)signal.size() );

            total-=pl;
        }

        signal_type runsignal = makeRunCommand();
        appendSignal(&signal,&runsignal); // send mc "start the application"
        // added silence at sound end to time out sound fading in some wav players like from Mircosoft

        for(int k=0; k<10; k++)
        {
            signal_type silencesignal = this->silence(frameSetup.getSilenceBetweenPages());
            appendSignal(&signal,&silencesignal);
        }

        return signal;
    }


};


#endif // WAVCODEGENERATOR_H
