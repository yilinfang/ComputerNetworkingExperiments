// StopWait.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Global.h"
#include "RdtSender.h"
#include "RdtReceiver.h"
#include "StopWaitRdtSender.h"
#include "StopWaitRdtReceiver.h"
#include "GBNRdtSender.h"
#include "GBNRdtReceiver.h"
#include "TCPRdtSender.h"
#include "TCPRdtReceiver.h"

int main(int argc, char** argv[])
{
	//RdtSender *ps = new GBNRdtSender();
	//RdtReceiver * pr = new GBNRdtReceiver();
	//RdtSender *ps = new GBNRdtSender(16);
	//RdtReceiver * pr = new GBNRdtReceiver(16);
	RdtSender *ps = new TCPRdtSender();
	RdtReceiver * pr = new TCPRdtReceiver();
	//RdtSender *ps = new TCPRdtSender(32);
	//RdtReceiver * pr = new TCPRdtReceiver(32);
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile("D:\\ComputerNetworkingExperiments\\test1\\input.txt");
	pns->setOutputFile("D:\\ComputerNetworkingExperiments\\test1\\output.txt");
	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//ָ��Ψһ�Ĺ�����ʵ����ֻ��main��������ǰdelete
	delete pns;										//ָ��Ψһ��ģ�����绷����ʵ����ֻ��main��������ǰdelete
	
	return 0;
}

