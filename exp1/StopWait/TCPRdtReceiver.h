#pragma once
#include "RdtReceiver.h"
#include "Global.h"
class TCPRdtReceiver :
	public RdtReceiver
{
private:
	int N;
	int base;
	Packet sndpkt;
	Packet* buffer;
public:
	TCPRdtReceiver();
	TCPRdtReceiver(int n);
	~TCPRdtReceiver();
	void receive(Packet &packet);
};

