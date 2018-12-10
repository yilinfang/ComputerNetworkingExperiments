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
			pUtils->printPacket("���շ�����ȷ�ϱ���", sndpkt);
			pns->sendToNetworkLayer(SENDER, sndpkt);
			expectedSeqNum = (expectedSeqNum + 1) % N;
			return;
		}
		else
		{
			pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,������Ų���", packet);
		}
	}
	else
	{
		pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,����У�����", packet);
	}
	cout << "���շ��ڴ��ı������к�" << expectedSeqNum << endl;
	pUtils->printPacket("���շ����·����ϴε�ȷ�ϱ���", sndpkt);
	pns->sendToNetworkLayer(SENDER, sndpkt);
}
