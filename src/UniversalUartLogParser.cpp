#include "UniversalUartLogParser.h"




int main()
{
	FileLogger allLogs("All");
	SerialCom serialCom("COM6", 460800);

	Subscriber sub;
	RegisteredKeyword rk(std::string("WARN"));
	rk.addSubscriber(sub);
	FrameParser fp;
	fp.addRegisteredkeyword(rk);

	while (1){
		std::string rxLine = serialCom.doRead();
		std::cout<<rxLine<<std::endl;
		allLogs.Add(rxLine);
		fp.OnReceivedFrame(rxLine);
	}

	return 0;
}
