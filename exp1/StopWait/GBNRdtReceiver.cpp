#include "stdafx.h"
#include "GBNRdtReceiver.h"


GBNRdtReceiver::GBNRdtReceiver() : expectedSeqNum(0), N(4)
{
	sndpkt.acknum = -1;
	sndpkt.checksum = 0;
	sndpkt.seqnum = -1;
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++)
	{
		sndpkt.payload[i] = '.';
	}
	sndpkt.checksum = pUtils->calculateCheckSum(sndpkt);
}

GBNRdtReceiver::GBNRdtReceiver(int n) :expectedSeqNum(0), N(n)
{
	sndpkt.acknum = -1;
	sndpkt.checksum = 0;
	sndpkt.seqnum = -1;
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++)
	{
		sndpkt.payload[i] = '.';
	}
	sndpkt.checksum = pUtils->calculateCheckSum(sndpkt);
}


GBNRdtReceiver::~GBNRdtReceiver()
{
}

void GBNRdtReceiver::receive(Packet & packet)
{
	if (pUtils->calculateCheckSum(packet) == packet.checksum)
	{
		if (expectedSeqNum == packet.seqnum)
		{
			Message msg;
			memcpy(msg.data, packet.payload, sizeof(packet.payload));
			pns->delivertoAppLayer(RECEIVER, msg);
			sndpkt.acknum = packet.seqnum;
			sndpkt.checksum = pUtils->calculateCheckSum(sndpkt);
			pUtils->printPacket("接收方发送确认报文", sndpkt);
			pns->sendToNetworkLayer(SENDER, sndpkt);
			expectedSeqNum = (expectedSeqNum + 1) % N;
			return;
		}
		else
		{
			pUtils->printPacket("接收方没有正确收到发送方的报文,报文序号不对", packet);
		}
	}
	else
	{
		pUtils->printPacket("接收方没有正确收到发送方的报文,数据校验错误", packet);
	}
	cout << "接收方期待的报文序列号" << expectedSeqNum << endl;
	pUtils->printPacket("接收方重新发送上次的确认报文", sndpkt);
	pns->sendToNetworkLayer(SENDER, sndpkt);
}
