// MyBigServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "iostream"
#include "SocketServer.h"
#include<string>
using namespace std;
//回调函数
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
	char buf[DATA_BUFSIZE * 5];	//存放接收到的内容
	string szSendBuf = "Welcome to my HomePage";
	nRet = g_SocketServer.SendToSocket(s, szSendBuf.c_str(), szSendBuf.size());
	g_SocketServer.SetClientFlag(s);//
	while (1)
	{
		ZeroMemory(buf, DATA_BUFSIZE * 5);
		
		
		nRet = g_SocketServer.RecvFromSocket(s, buf, DATA_BUFSIZE * 5, 60 * 60 * 24*365);//连接上1*365天内没命令接收则认为掉线
		if (nRet > 0)//接收有数据进入
		{
			//ISCS向OPS轮询服务器状态报文
			//功能码04H
			char resdata[256] = {0};
			int len = 0;
			
			if (buf[7] == 0x04)
			{
				long RegisterNum = buf[10] << 8 | buf[11]; //寄存器的个数
			//暂定请求的寄存器个数为1个。
				for (int i = 0; i < 8; i++)
				{
					resdata[i] = buf[i];
				}
				int HeadLength = 3 + 2 * RegisterNum;
				resdata[4] = HeadLength>>8;  //高8位
				resdata[5] = HeadLength & 15; //低8位
				resdata[8] = RegisterNum*2;  //字节个数 =  寄存器个数*2 
				for (int i = 1; i <= RegisterNum; i++)
				{
					resdata[7+2*i] = 0;
					resdata[8+2*i] = 0; //  0:通信正常  1：通信中断
					
				}
				
				g_SocketServer.SendToSocket(s, resdata, HeadLength+6);



			}
			//ISCS向OPS系统发送CCTV的控制信息 ：
			//功能码10H
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
				//(a)控制指令   buf[14];
				switch (buf[14])
				{
					case 0:  //00000000 : 无效
				         break;
					case 1:  //00000001 //启动摄像机到监视器 
						
						screenID = buf[19];
						camerID = buf[23];
						cout << "启动摄像机 " << camerID << "到大屏 " << screenID;
						break;
					case 2:  //00000010 停止摄像机到监视器
					
						screenID = buf[19];
						camerID = buf[23];
						cout << "关闭摄像机" << camerID;
						break;
					case 3: //00000011 :  设置预置位

						break;
					case 4:  //00000100 :  调用预置位

						break;
					case 5:  //00000101 :  云台控制 （包含启动和停止）
						screenID = buf[19];
						if (moveOrder != 0)//摄像机转动指令不为空
						{
							int verticalOrder = moveOrder>>2 &3; //垂直参数
							int HorizontalOrder = moveOrder & 3;//水平参数
							if (HorizontalOrder != 0)
							{
								switch (HorizontalOrder)
								{
								case 1:
									cout << "左转" << endl;
									break;
								case 2:
									cout << "右转" << endl;
									break;
								case 3:
									cout << "停止" << endl;
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
						if(opticalZoom!=0) //摄像机焦距调节不为空
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
			//const string temp = "我已经收到你的消息：" + t;
			//cout << buf << endl;
			////m_SocketServer.SendToSocket(s, temp.c_str(), temp.size());
			//g_SocketServer.BroadCastMsg(temp.c_str(), temp.size());
		}
		else
		{

			break;
		}
	}
	cout << "断开连接！" << s << endl;
	g_SocketServer.DeleteSocket(s);
}