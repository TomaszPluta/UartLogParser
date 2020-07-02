#include<windows.h>
#include<stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include <map>



class FileLogger{
public:

	FileLogger(std::string name){
		fileLog.open(name + ".txt");
		if (!fileLog.is_open()){
		  throw std::runtime_error("Could not open file" + name + ".txt");
		}
	}

	~FileLogger(){
		fileLog.close();
	}

	void Add(std::string s){
		fileLog<<s;
	}
private:

	std::ofstream fileLog;

};



class Subscriber{
public:
	void notify(std::string s){
		std::cout<<"notified:" + s<<std::endl;
	}
};


class Publisher{
public:
	Publisher(){}
	Publisher(std::initializer_list<Subscriber> subscriber) : m_subscribers{subscriber}{};

	void addSubscriber(Subscriber s){
		m_subscribers.push_back(s);
	}

	void notifySubscribers(std::string msg){
		for(auto s: m_subscribers){
			s.notify(msg);
		}
	}

private:
	std::vector<Subscriber> m_subscribers;
};





class RegisteredKeyword : public Publisher{
public:
	RegisteredKeyword(std::string keyword) : m_keyword{keyword}{}
	RegisteredKeyword(std::string keyword, std::initializer_list<Subscriber> subscribers) : Publisher{subscribers}, m_keyword{keyword} {}
	std::string m_keyword;
};



class FrameParser{
public:

	FrameParser(){};
	FrameParser(std::initializer_list<RegisteredKeyword> rk) : m_registeredKeywords{rk}{}
	void addRegisteredkeyword(RegisteredKeyword rk){
		m_registeredKeywords.push_back(rk);
	}

	void OnReceivedFrame(std::string frame){
		for (auto k : m_registeredKeywords){
			if ( frame.find(k.m_keyword) != std::string::npos){
				k.notifySubscribers(frame);
			}
		}

	}
	std::vector<RegisteredKeyword> m_registeredKeywords;
};



class FrameParserExtractor{
public:

	FrameParserExtractor(){};
	FrameParserExtractor(std::initializer_list<RegisteredKeyword> rk) : m_registeredKeywords{rk}{}
	void addRegisteredkeyword(RegisteredKeyword rk){
		m_registeredKeywords.push_back(rk);
	}

	void OnReceivedFrame(std::string frame){
		for (auto k : m_registeredKeywords){
			if (size_t pos = frame.find(k.m_keyword) != std::string::npos){
				k.notifySubscribers(frame.substr(pos));
			}
		}

	}
	std::vector<RegisteredKeyword> m_registeredKeywords;
};




class SerialCom{
public:

	SerialCom(std::string com, int baud){
		 hComm = CreateFile("COM6", GENERIC_READ | GENERIC_WRITE,  0,  NULL, OPEN_EXISTING,   0,  NULL);

		 dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

		  GetCommState(hComm, &dcbSerialParams);
		  dcbSerialParams.BaudRate = 460800;
		  dcbSerialParams.ByteSize = 8;
		  dcbSerialParams.StopBits = ONESTOPBIT;
		  dcbSerialParams.Parity   = NOPARITY;

		  SetCommState(hComm, &dcbSerialParams);
		  if (hComm == INVALID_HANDLE_VALUE){
			  throw std::runtime_error("Can not open com" + com);
		  }
		  else{
			  std::cout<<"start spying..."<<std::endl;
		  }
	}


	~SerialCom(){
		  CloseHandle(hComm);
		  std::cout<<"...stop spying"<<std::endl;
	}


	std::string doRead(void){
		  char TempChar;
		  DWORD NoBytesRead;
		  std::string rxLine;

		  do
		   {
		     ReadFile( hComm, &TempChar, sizeof(TempChar), &NoBytesRead, NULL);
		     rxLine += TempChar;
		     if (TempChar == '\n'){
		    	// std::cout<<rxLine<<std::endl;

		    	 return rxLine;
		     }
		    }

		  while (NoBytesRead > 0);
	}


private:
	  HANDLE hComm;
	  DCB dcbSerialParams = { 0 };
};


