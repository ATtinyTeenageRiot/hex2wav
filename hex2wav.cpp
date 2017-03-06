#include <iostream>
#include <vector>


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
	
	void getDifferentialManchesterCodedSignal(std::vector<int>* hexdata)
	{

		int laenge = (int) hexdata->size();
		std::vector<float> signal((laenge*8)*manchesterNumberOfSamplesPerBit);

		int counter = 0;

		for (int count = 0; count < laenge; count++) {
			int dat = hexdata->at(count);
			for (int n;n<8;n++) {

				if((dat&0x80)==0)
				{
					manchesterEdge(false,counter,&signal); // low bit
				}
				else
				{
					manchesterEdge(true,counter,&signal); // high bit
				}
				
				counter += manchesterNumberOfSamplesPerBit;
				dat = dat << 1;
			}
		}
	}
	
	void manchesterEdge(bool flag, int pointerIntoSignal, std::vector<float>* signal)
	{

		int value = 0;
		if( !useDifferentialManchsterCode ) // non differential manchester code
		{
			if(flag) 
				value=1;
			else 
				value=-1;
			
			if(invertSignal) value = value * -1;  // correction of an inverted audio signal line
			
			for(int n = 0; n < manchesterNumberOfSamplesPerBit; n++)
			{
				if( n < manchesterNumberOfSamplesPerBit/2) 
					signal->at(pointerIntoSignal) = -value;
				else 
					signal->at(pointerIntoSignal) = value;
					
				pointerIntoSignal++;
			}
		}
		else // differential manchester code ( inverted )
		{
			if(flag) manchesterPhase = -manchesterPhase; // toggle phase
			for(int n = 0; n < manchesterNumberOfSamplesPerBit; n++)
			{
				if( n == (manchesterNumberOfSamplesPerBit/2)) manchesterPhase = -manchesterPhase; // toggle phase
				signal->at(pointerIntoSignal)=manchesterPhase;
				pointerIntoSignal++;
			}		
		}
	}

	void manchesterCoding(std::vector<int>* hexdata)
	{
		int laenge = (int) hexdata->size();
		std::vector<float> signal((1+startSequencePulses+laenge*8)*manchesterNumberOfSamplesPerBit);
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
			int dat=hexdata->at(count);
			//System.out.println(dat);
			/** create one byte **/			
			for(int n=0;n<8;n++) // first bit to send: MSB
			{
				if((dat&0x80)==0) 	manchesterEdge(false,counter,&signal); // generate falling edges ( 0 bits )
				else 				manchesterEdge(true,counter,&signal); // rising edge ( 1 bit )
				counter+=manchesterNumberOfSamplesPerBit;	
				dat=dat<<1; // shift to next bit
			}
		}
//		return signal;	
	}
	
	
	void setSignalSpeed(bool fullSpeedFlag)
	{
		if( fullSpeedFlag ) 
			manchesterNumberOfSamplesPerBit = 4; // full speed
		else
			manchesterNumberOfSamplesPerBit = 8; // half speed
	}


	void flankensignal(std::vector<int>* hexdata)
	{
		int intro = startSequencePulses*lowNumberOfPulses+numStartBits*highNumberOfPulses+numStopBits*lowNumberOfPulses;
		int laenge = (int) hexdata->size();
		int sigState=-1;
		int counter=0;

		std::vector<float> signal(intro+laenge*8*highNumberOfPulses);
		
		/** generate start sequence **/
		int numOfPulses = lowNumberOfPulses;
		
		for (int n=0; n < startSequencePulses; n++)
		{
			for(int k=0; k < numOfPulses; k++)
			{
				signal.at(counter++) = sigState;
			}
			sigState *= -1;
		}

		/** start: create 2 high-Bits **/
		numOfPulses = highNumberOfPulses;
		for (int n=0; n < numStartBits; n++)
		{
			for(int k=0; k < numOfPulses; k++)
			{
				signal.at(counter++) = sigState;
			}
			sigState *= -1;
		}

		/** create data signal **/
		for(int count = 0; count < laenge; count++)
		{
			int dat = hexdata->at(count);
			/** create one byte **/			
			for(int n=0; n < 8; n++)
			{
				if((dat&0x80)==0)numOfPulses=lowNumberOfPulses;
				else numOfPulses=highNumberOfPulses;
				dat = dat<<1; // shift to next bit				
				for(int k=0; k < numOfPulses; k++)
				{
					signal.at(counter++) = sigState;
				}
				sigState *= -1;
			}
		}

		/** stop: create 1 low-Bit **/
		numOfPulses=lowNumberOfPulses;
		for (int n=0; n < numStopBits; n++)
		{
			for(int k=0; k < numOfPulses; k++)
			{
				signal.at(counter++) = sigState;
			}
			sigState *= -1;
		}

// todo
//		/** cut to long signal */
//		var sig2=new Array(counter);
//		
//		for(int n=0; n<sig2.size(); n++) 
//		{
//			sig2[n] = signal[n];
//		}
//		
//		return sig2;
	}
};

class WavCodeGenerator {
private:
	int sampleRate;
	bool fullSpeedFlag;
	BootFrame * frameSetup;
public:
	
	WavCodeGenerator()
	{
		sampleRate = 44100;
		fullSpeedFlag = true;
		frameSetup = new BootFrame();
	}
	
	void appendSignal(std::vector<float> * sig1, std::vector<float> * sig2)
	{
		int l1 = (int) sig1->size();
		int l2 = (int) sig2->size();
		
//		var d=new Array(l1+l2);
		
		//for(int n=0;n<l1;n++) d[n] = sig1->at(n);
		//for(int n=0;n<l2;n++) d[n+l1] = sig2->at(n);		
		
		//return d;
	}

	void setSignalSpeed (bool fullSpeedFlag) 
	{
		fullSpeedFlag = fullSpeedFlag;
	};
	

	void generatePageSignal(int data[]) 
	{
//			var h2s=new HexToSignal(fullSpeedFlag);
//			var frameData=new Array(frameSetup.getFrameSize());

			// copy data into frame data
//			for(var n=0;n<this.frameSetup.getPageSize();n++)
//			{
//				if ( n < data.length ) frameData[n+this.frameSetup.getPageStart()]=data[n];
//				else frameData[n+this.frameSetup.getPageStart()]=0xFF;
//			}
//			
//			this.frameSetup.addFrameParameters(frameData);
//
//			var signal=h2s.manchesterCoding(frameData);
//
//			return signal;
	};	


	void silence(int duration)
	{

			std::vector<float> signal(duration * sampleRate);
			int signal_size = (int) signal.size();
			
			for (int i = 0; i < signal_size; i++) {
				signal.at(i) = 0;
			}
			
//			return signal;
	};	


	void makeRunCommand()
	{
			HexToSignal h2s=new HexToSignal(fullSpeedFlag);
//			var frameData=new Array(this.frameSetup.getFrameSize());
			frameSetup->setRunCommand();
//			frameSetup.addFrameParameters(frameData);
//			var signal=h2s.manchesterCoding(frameData);
//			return signal;
	};

	void makeTestCommand()
	{
				HexToSignal h2s=new HexToSignal(fullSpeedFlag);
	//			var frameData=new Array(this.frameSetup.getFrameSize());
				frameSetup->setTestCommand();
	//			frameSetup.addFrameParameters(frameData);
	//			var signal=h2s.manchesterCoding(frameData);
	//			return signal;
	};

	void generateSignal(/*data*/)
	{

//	var signal= [];
	frameSetup->setProgCommand(); // we want to programm the mc
	int pl = frameSetup->getPageSize();
//	var total=data.length;
	int sigPointer=0;
	int pagePointer=0;

//		while(total>0)
//		{
//			frameSetup->setPageIndex(pagePointer++);
//			frameSetup->setTotalLength(data.length);

//			var partSig=new Array(pl);
			
//			for(int n=0; n < pl; n++)
//			{
//				if(n+sigPointer>data.length-1) partSig[n]=0xFF;
//				else partSig[n]=data[n+sigPointer];
//			}

//			sigPointer += pl;

//			var sig=generatePageSignal(partSig);
						
//			signal=this.appendSignal(signal,sig);
//			signal=this.appendSignal(signal,this.silence(this.frameSetup.getSilenceBetweenPages()));
			
//			total-=pl;
//		}
		

		
	}	
	

};

using namespace std;
int main(int argc, char *argv[]) {
	
}