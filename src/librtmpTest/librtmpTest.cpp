#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include "rtmp.h"
#pragma comment(lib,"librtmp.lib")
#include <stdio.h>
int InitSockets()
{
	WORD version;
	WSADATA wsaData;
	version = MAKEWORD(1, 1);
	return (WSAStartup(version, &wsaData) == 0);
}
void CleanupSockets()
{
	WSACleanup();
}
int main(int argc, char* argv[])
{
	InitSockets();
	RTMP rtmp = { 0 };
	RTMP_Init(&rtmp);
	rtmp.Link.timeout = 25;//超时设置
	//由于crtmpserver是每个一段时间(默认8s)发送数据包,需大于发送间隔才行
	bool bLiveStream = true;//是否直播
	if (bLiveStream)
	{
		RTMP_SetupURL(&rtmp, "rtmp://live.hkstv.hk.lxdns.com/live/hks");
		//设置直播标志
		rtmp.Link.lFlags |= RTMP_LF_LIVE;
	}
	else
	{
		RTMP_SetupURL(&rtmp, "rtmp://127.0.0.1:1935/vod/test.flv");
	}
	RTMP_SetBufferMS(&rtmp, 3600 * 1000);//1hour
	if (!RTMP_Connect(&rtmp, NULL))
	{
		printf("Connect Server Errn");
		WSACleanup();
		return -1;
	}
	if (!RTMP_ConnectStream(&rtmp, 0))
	{
		printf("Connect stream Errn");
		RTMP_Close(&rtmp);
		WSACleanup();
		return -1;
	}
	int buffsize = 1024 * 1024 * 10;
	char*buff = (char*)malloc(buffsize);
	double duration = -1;
	int nRead;
	FILE*fp = fopen("aaa.flv", "wb");
	long  countbuffsize = 0;
	//它直接输出的就是FLV文件,包括FLV头,可对流按照flv格式解析就可提取音频,视频数据
	while (nRead = RTMP_Read(&rtmp, buff, buffsize))
	{
		printf("%d\n", nRead);
		fwrite(buff, 1, nRead, fp);
		if (!bLiveStream&&duration < 0)
		{
			duration = RTMP_GetDuration(&rtmp);
			printf("duration:%fn\n", duration);
		}
		countbuffsize += nRead;
		printf("rdownland...:%0.2fkB\n", countbuffsize*1.0 / 1024);
		getchar();
	}
	fclose(fp);
	free(buff);
	buff = NULL;
	RTMP_Close(&rtmp);
	WSACleanup();
	return 0;
}