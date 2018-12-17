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
		if ((packet.seqnum - base + 2 * N) % (2 * N) < N)
		{
			cout << "���շ��������ڵ�baseֵΪ" << base << endl;
			pUtils->printPacket("���շ����汨��", packet);
			buffer[packet.seqnum % N] = packet;
			buffer[packet.seqnum % N].acknum = 1;
			if (base == packet.seqnum)
			{
				Message msg;
				while (buffer[base % N].acknum == 1)
				{
					memcpy(msg.data, buffer[base % N].payload, sizeof(buffer[base % N].payload));
					pns->delivertoAppLayer(RECEIVER, msg);
					buffer[base % N].acknum = -1;
					base = (base + 1) % (2 * N);
				}
				sndpkt.acknum = base;
				sndpkt.checksum = pUtils->calculateCheckSum(sndpkt);
				cout << "���շ��ڴ��ı������к�" << base << endl;
				pUtils->printPacket("���շ�����ȷ�ϱ���", sndpkt);
				pns->sendToNetworkLayer(SENDER, sndpkt);
				return;
			}
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
	cout << "���շ��ڴ��ı������к�" << base << endl;
	pUtils->printPacket("���շ����·����ϴε�ȷ�ϱ���", sndpkt);
	pns->sendToNetworkLayer(SENDER, sndpkt);
}
