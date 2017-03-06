#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

#define signal_type vector<float>

class BootFrame {
	private:
		int command;
		int pageIndex;
		int totalLength;
		int crc;
		int pageStart;
		int pageSize;
		int frameSize;
		float silenceBetweenPages;

	public:
		BootFrame()
		{
			command=0;
			pageIndex=4;
			totalLength=0;
			crc=0x55AA;
			pageStart = 7;
			pageSize  = 64;
			frameSize = pageStart + pageSize;
			silenceBetweenPages=0.02;	
		}

		void setProgCommand() {
			command=2;
		};


		void setRunCommand() {
			command=3;
		};


		void setTestCommand() {
			command=1;
		};


		int * addFrameParameters(int data[]) {

				data[0]=command;
				data[1]=pageIndex&0xFF;
				data[2]=(pageIndex>>8)&0xFF;

				data[3]=totalLength&0xFF;
				data[4]=(totalLength>>8)&0xFF;

				data[5]=crc&0xFF;
				data[6]=(crc>>8)&0xFF;

				return data;
		};


		void setFrameSize(int frameSize) {
				frameSize = frameSize;
		};

		int getFrameSize() {
				return frameSize;
		};

		void setCommand(int command) {
				command = command;
		};

		int getCommand() {
				return command;
		};

		void setPageIndex(int pageIndex) {
			pageIndex = pageIndex;
		};

		int getPageIndex() {
			return pageIndex;
		};

		void setTotalLength(int totalLength) {
			totalLength = totalLength;
		};

		int getTotalLength() {
			return totalLength;
		};

		void setCrc(int crc) {
			crc = crc;
		};

		int getCrc() {
			return crc;
		};

		void setPageStart(int pageStart) {
			pageStart = pageStart;
		};

		int getPageStart() {
			return pageStart;
		};

		void setPageSize(int pageSize) {
			pageSize = pageSize;
		};

		int getPageSize() {
			return pageSize;
		};

		void setSilenceBetweenPages(float silenceBetweenPages) {
			silenceBetweenPages = silenceBetweenPages;
		};

		float getSilenceBetweenPages() {
			return silenceBetweenPages;
		};

	};



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
			for (int n;n<8;n++) {

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

class WavCodeGenerator {
private:
	int sampleRate;
	bool fullSpeedFlag;
	BootFrame frameSetup;
public:
	
	WavCodeGenerator()
	{
		sampleRate = 44100;
		fullSpeedFlag = true;
	}
	
	void appendSignal(vector<float> * sig1, vector<float> * sig2)
	{
		int l2 = (int) sig2->size();
		for(int n=0;n<l2;n++) sig1->push_back(sig2->at(n));				
	}

	void setSignalSpeed (bool fullSpeedFlag) 
	{
		fullSpeedFlag = fullSpeedFlag;
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
			HexToSignal * h2s=new HexToSignal(fullSpeedFlag);
			int frameData[frameSetup.getFrameSize()];
			frameSetup.setRunCommand();
			frameSetup.addFrameParameters(frameData);
			signal_type signal=h2s->manchesterCoding(frameData, frameSetup.getFrameSize());
			delete h2s;
			return signal;
	};

	signal_type makeTestCommand()
	{
			HexToSignal * h2s=new HexToSignal(fullSpeedFlag);
			int frameData[frameSetup.getFrameSize()];
			frameSetup.setTestCommand();
			frameSetup.addFrameParameters(frameData);
			signal_type signal=h2s->manchesterCoding(frameData, frameSetup.getFrameSize());
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
			frameSetup.setTotalLength(total);

			signal_type partSig(pl);
						
			for(int n=0; n < pl; n++)
			{
				if(n+sigPointer>total-1) partSig[n]=0xFF;
				else partSig[n]=data->at(n+sigPointer);
			}

			sigPointer += pl;

			signal_type sig = generatePageSignal(&partSig);						
			appendSignal(&signal,&sig);
			
			signal_type silence = this->silence(frameSetup.getSilenceBetweenPages());
			
//			printf("siglen silence %i\n", (int)silence.size() );
			
			appendSignal(&signal, &silence);
			
			printf("siglen %i\n", (int)signal.size() );
			
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


class hexFileDecoder 
{
public:		
	vector<int> decodeHex(string file)
	{
		vector<int> res;
		vector<int> data;		
		string line;
				
		ifstream hexFile(file);
		
		if (hexFile.is_open())
		{
			while ( getline (hexFile,line) )
			{
				readLine(&res, line);			
			}
			hexFile.close();
		}		
		return res;
	}
	
	int hexToDec(string hexString)
	{
		int res;
		stringstream ss;
		ss << hex << hexString;
		ss >> res;
		
		return res;
	}	
	
	void readLine(vector<int> * res, string line)
	{
		bool eof = false;
		int segment = 0;
		int extended = 0;
		
//		cout << line << "\n";
		
		if (line[0] != ':')
		{
		} else {
			int byteCount = hexToDec(line.substr(1, 2));
			int address = hexToDec(line.substr(3, 4));
			int recordType =  hexToDec(line.substr(7, 2));

			vector<int> data;
			
			switch (recordType) {
				case 0:
					for (int i = 0; i < byteCount; i++) {
						data.push_back(hexToDec(line.substr(i*2+9, 2)));
					}
					break;
				case 1:
					eof = true;
					break;
				case 2:
					segment = hexToDec(line.substr(9, 2));
					break;
				case 3:
					break;
				case 4:
					extended = hexToDec(line.substr(9, 2));
					break;
				case 5:
					break;
				default:
					break;
			}
			
			int checksum = hexToDec(line.substr(byteCount*2+9, 2));
			int sum = 0;
			
			for(int i= 0; i<data.size(); i++) {
				sum = sum + data.at(i);
			}			
			
			if(recordType != 1) {
				int addy = segment * 16 + address;
				addy = extended * 65536 + addy;
				
				for(int i= 0; i<data.size(); i++) {
					res->push_back(data.at(i));
				}	
			}

		}
		
	}
};



static const int hexdata[] = {14,192,29,192,28,192,27,192,26,192,49,192,24,192,23,192,22,192,21,192,20,192,19,192,18,192,17,192,16,192,17,36,31,190,207,229,210,224,222,191,205,191,32,224,160,230,176,224,1,192,29,146,169,54,178,7,225,247,4,208,119,192,224,207,8,149,8,149,129,183,129,191,92,208,250,223,250,223,254,207,128,183,128,127,128,191,128,183,128,104,128,191,140,181,128,100,140,189,143,239,141,189,128,183,135,96,128,191,8,149,31,146,15,146,15,182,15146,17,36,47,147,63,147,143,147,159,147,175,147,191,147,128,145,97,0,144,145,98,0,160,145,99,0,176,145,100,0,48,145,96,0,38,224,35,15,45,55,48,240,41,232,35,15,3,150,161,29,177,29,3,192,2,150,161,29,177,29,32,147,96,0,128,147,97,0,144,147,98,0,160,147,99,0,176,147,100,0,128,145,101,0,144,145,102,0,160,145,103,0,176,145,104,0,1,150,161,29,177,29,128,147,101,0,144,147,102,0,160,147,103,0,176,147,104,0,191,145,175,145,159,145,143,145,63,145,47,145,15,144,15,190,15,144,31,144,24,149,138,181,130,96,138,189,138,181,129,96,138,189,131,183,136,127,131,96,131,191,120,148,137,183,130,96,137,191,152,223,134,177,136,119,134,104,134,185,55,154,8,149,248,148,255,207};

vector<int> hexvec (hexdata, hexdata + sizeof(hexdata) / sizeof(hexdata[0]) );

using namespace std;
int main(int argc, char *argv[]) {
	
	hexFileDecoder hexDec;
	WavCodeGenerator wg;
	
	
//	int sig_size =  (int) hex_signal.size();
//	int hex_size =  (int) hexvec.size();
//
//	printf("signal size: %i\n",sig_size);
//	printf("hex size: %i\n",hex_size);
//
//	
	vector<int> hex_decoded = hexDec.decodeHex("test.txt");
	signal_type hex_signal = wg.generateSignal(&hexvec);

	for (int i = 0;i<hex_signal.size();i++) {
		printf("%.1f\n", hex_signal.at(i));
	}	


//	for (int i = 0;i<hex_decoded.size();i++) {
//				printf("%i\n", hex_decoded.at(i));
//	}	

	
}