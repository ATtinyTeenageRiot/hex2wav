#ifndef BOOTFRAME_H
#define BOOTFRAME_H

using namespace std;

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
           this->frameSize = frameSize;
        };

        int getFrameSize() {
           return frameSize;
        };

        void setCommand(int command) {
           this->command = command;
        };

        int getCommand() {
           return command;
        };

        void setPageIndex(int pageIndex) {
            this->pageIndex = pageIndex;
        };

        int getPageIndex() {
            return pageIndex;
        };

        void setTotalLength(int totalLength) {
            this->totalLength = totalLength;
        };

        int getTotalLength() {
            return totalLength;
        };

        void setCrc(int crc) {
            this->crc = crc;
        };

        int getCrc() {
            return crc;
        };

        void setPageStart(int pageStart) {
            this->pageStart = pageStart;
        };

        int getPageStart() {
            return pageStart;
        };

        void setPageSize(int pageSize) {
            this->pageSize = pageSize;
        };

        int getPageSize() {
            return pageSize;
        };

        void setSilenceBetweenPages(float silenceBetweenPages) {
            this->silenceBetweenPages = silenceBetweenPages;
        };

        float getSilenceBetweenPages() {
            return silenceBetweenPages;
        };

};


#endif // BOOTFRAME_H
