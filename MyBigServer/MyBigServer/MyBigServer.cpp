// MyBigServer.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "iostream"
#include "SocketServer.h"
#include<string>
using namespace std;
//�ص�����
static void CALLBACK OnReceive(SOCKET s, int ServerPort, const char *ClientIP, void* pServer);
CSocketServer g_SocketServer;
int main()
{
	
	g_SocketServer.SetSocketPort(30000);
	g_SocketServer.SetListenCount(5);
	int iStartServer = 0;
	if (g_SocketServer.CreateServer() > 0)
	{
		iStartServer = g_SocketServer.StartServer(OnReceive);
	}
	if (iStartServer <= 0)
	{
		cout << "creat server fail!";
		return false;
	}
	cout << "My Big Server,Yeah!";
	while (1)
	{
		Sleep(10000);
		//cout << "main thread wakeup!!" << endl;
	};

    return 0;
}

static void CALLBACK OnReceive(SOCKET s, int ServerPort, const char *ClientIP, void* pServer)
{
	int nRet = -1;
	char buf[DATA_BUFSIZE * 5];	//��Ž��յ�������
	string szSendBuf = "Welcome to my HomePage";
	nRet = g_SocketServer.SendToSocket(s, szSendBuf.c_str(), szSendBuf.size());
	g_SocketServer.SetClientFlag(s);//
	while (1)
	{
		ZeroMemory(buf, DATA_BUFSIZE * 5);
		
		
		nRet = g_SocketServer.RecvFromSocket(s, buf, DATA_BUFSIZE * 5, 60 * 60 * 24*365);//������1*365����û�����������Ϊ����
		if (nRet > 0)//���������ݽ���
		{
			//ISCS��OPS��ѯ������״̬����
			//������04H
			char resdata[256] = {0};
			int len = 0;
			
			if (buf[7] == 0x04)
			{
				long RegisterNum = buf[10] << 8 | buf[11]; //�Ĵ����ĸ���
			//�ݶ�����ļĴ�������Ϊ1����
				for (int i = 0; i < 8; i++)
				{
					resdata[i] = buf[i];
				}
				int HeadLength = 3 + 2 * RegisterNum;
				resdata[4] = HeadLength>>8;  //��8λ
				resdata[5] = HeadLength & 15; //��8λ
				resdata[8] = RegisterNum*2;  //�ֽڸ��� =  �Ĵ�������*2 
				for (int i = 1; i <= RegisterNum; i++)
				{
					resdata[7+2*i] = 0;
					resdata[8+2*i] = 0; //  0:ͨ������  1��ͨ���ж�
					
				}
				
				g_SocketServer.SendToSocket(s, resdata, HeadLength+6);



			}
			//ISCS��OPSϵͳ����CCTV�Ŀ�����Ϣ ��
			//������10H
			if (buf[7] == 0x10)
			{
				for (int i = 0; i < 12; i++)
				{
					resdata[i] = buf[i];
				}
				resdata[4] = 0;
				resdata[5] = 6;
				g_SocketServer.SendToSocket(s, resdata, 12);
				int camerID = 0;
				int screenID = -1;
				int moveOrder = 0;
				int opticalZoom = 0;

				screenID = buf[19];
				camerID = buf[23];
				moveOrder = buf[16];
				opticalZoom = buf[15];
				//(a)����ָ��   buf[14];
				switch (buf[14])
				{
					case 0:  //00000000 : ��Ч
				         break;
					case 1:  //00000001 //����������������� 
						
						screenID = buf[19];
						camerID = buf[23];
						cout << "��������� " << camerID << "������ " << screenID;
						break;
					case 2:  //00000010 ֹͣ�������������
					
						screenID = buf[19];
						camerID = buf[23];
						cout << "�ر������" << camerID;
						break;
					case 3: //00000011 :  ����Ԥ��λ

						break;
					case 4:  //00000100 :  ����Ԥ��λ

						break;
					case 5:  //00000101 :  ��̨���� ������������ֹͣ��
						screenID = buf[19];
						if (moveOrder != 0)//�����ת��ָ�Ϊ��
						{
							int verticalOrder = moveOrder>>2 &3; //��ֱ����
							int HorizontalOrder = moveOrder & 3;//ˮƽ����
							if (HorizontalOrder != 0)
							{
								switch (HorizontalOrder)
								{
								case 1:
									cout << "��ת" << endl;
									break;
								case 2:
									cout << "��ת" << endl;
									break;
								case 3:
									cout << "ֹͣ" << endl;
									break;
								default:
									cout << "wrong HorizontalOrder Number" << HorizontalOrder << endl;
								}
							}
							if (verticalOrder != 0)
							{
								switch (verticalOrder)
								{
								case 1:
									cout << "up" << endl;
									break;
								case 2:
									cout << "down" << endl;
									break;
								case 3:
									cout << "stop" << endl;
									break;
									default:
										cout << "wrong veticalOrder" << verticalOrder << endl;
								}
							}

						}
						if(opticalZoom!=0) //�����������ڲ�Ϊ��
						{
							int opticalOrder = opticalZoom &3;
							if (opticalOrder != 0)
							{
								switch (opticalOrder)
								{
								case 1:
									cout << "opticalOrde Big" << endl;
									break;
								case 2:
									cout << "opticalOrde Small" << endl;
									break;
								case 3:
									cout << "opticalOrde stop" << endl;
									break;
								default:
									cout << "wrong opticalOrder" << opticalOrder << endl;
								}
							}
						}
						break;
				}

			}
			//const string t = buf;
			//const string temp = "���Ѿ��յ������Ϣ��" + t;
			//cout << buf << endl;
			////m_SocketServer.SendToSocket(s, temp.c_str(), temp.size());
			//g_SocketServer.BroadCastMsg(temp.c_str(), temp.size());
		}
		else
		{

			break;
		}
	}
	cout << "�Ͽ����ӣ�" << s << endl;
	g_SocketServer.DeleteSocket(s);
}