#ifndef HEXFILEDECODER_H
#define HEXFILEDECODER_H

using namespace std;

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

            //int checksum = hexToDec(line.substr(byteCount*2+9, 2));
            int sum = 0;

            for(int i= 0; i< (int) data.size(); i++) {
                sum = sum + data.at(i);
            }

            if(recordType != 1) {
                int addy = segment * 16 + address;
                addy = extended * 65536 + addy;

                for(int i= 0; i< (int) data.size(); i++) {
                    res->push_back(data.at(i));
                }
            }

        }

    }
};



#endif // HEXFILEDECODER_H
