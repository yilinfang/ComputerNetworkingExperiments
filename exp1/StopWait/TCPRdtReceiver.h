#pragma once
#include "RdtReceiver.h"
#include "Global.h"
class TCPRdtReceiver :
	public RdtReceiver
{
private:
	int N;
	int expectedSeqNum;
	Packet sndpkt;
public:
	TCPRdtReceiver();
	TCPRdtReceiver(int n);
	~TCPRdtReceiver();
	void receive(Packet &packet);
};

