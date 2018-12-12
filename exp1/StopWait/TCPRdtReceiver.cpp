#include "stdafx.h"
#include "TCPRdtReceiver.h"


TCPRdtReceiver::TCPRdtReceiver() :N(4), base(0)
{
	sndpkt.acknum = 0;
	sndpkt.checksum = 0;
	sndpkt.seqnum = -1;
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++)
	{
		sndpkt.payload[i] = '.';
	}
	sndpkt.checksum = pUtils->calculateCheckSum(sndpkt);
	buffer = new Packet[N];
	for (int i = 0; i < N; i++)
	{
		buffer[i].acknum = -1;
	}
}

TCPRdtReceiver::TCPRdtReceiver(int n) :N(n), base(0)
{
	sndpkt.acknum = 0;
	sndpkt.checksum = 0;
	sndpkt.seqnum = -1;
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++)
	{
		sndpkt.payload[i] = '.';
	}
	sndpkt.checksum = pUtils->calculateCheckSum(sndpkt);
	buffer = new Packet[N];
	for (int i = 0; i < N; i++)
	{
		buffer[i].acknum = -1;
	}
}


TCPRdtReceiver::~TCPRdtReceiver()
{
	delete[] buffer;
}

void TCPRdtReceiver::receive(Packet & packet)
{
	if (pUtils->calculateCheckSum(packet) == packet.checksum)
	{
		if ((packet.seqnum - base + N) % N < N / 2)
		{
			cout << "接收方滑动窗口的base" << base << endl;
			pUtils->printPacket("接收方缓存报文", packet);
			buffer[packet.seqnum] = packet;
			buffer[packet.seqnum].acknum = 1;
			if (base == packet.seqnum)
			{
				Message msg;
				while (buffer[base].acknum == 1)
				{
					memcpy(msg.data, buffer[base].payload, sizeof(buffer[base].payload));
					pns->delivertoAppLayer(RECEIVER, msg);
					buffer[base].acknum = -1;
					base = (base + 1) % N;
				}
				sndpkt.acknum = base;
				sndpkt.checksum = pUtils->calculateCheckSum(sndpkt);
				cout << "接收方期待的报文序列号" << base << endl;
				pUtils->printPacket("接收方发送确认报文", sndpkt);
				pns->sendToNetworkLayer(SENDER, sndpkt);
				return;
			}
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
	cout << "接收方期待的报文序列号" << base << endl;
	pUtils->printPacket("接收方重新发送上次的确认报文", sndpkt);
	pns->sendToNetworkLayer(SENDER, sndpkt);
}
