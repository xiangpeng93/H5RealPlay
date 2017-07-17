// FFMpegSessionMangerTestPrj.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include <thread>
#include <list>
#include <mutex>

#include "../include/StreamClient.h"
#include "../FFMpegSessionManger/FFMpegSessionManger.h"

#include "mongoose.h"

#pragma comment (lib,"../release/FFMpegSessionManger")
#pragma comment (lib,"../lib/StreamClient")

bool g_isExitThread = false;
int SessionId = -1;

std::mutex myMutex;
class ClientObj{

public:
	ClientObj(){

	}
	~ClientObj()
	{
	}
	void AddFile(std::string fileName)
	{
		m_fileList.push_back(fileName);
	}
	std::string GetFileNameByPreName(std::string fileName)
	{
		std::list<std::string>::iterator iter = m_fileList.begin();
		if (fileName == "null")
		{
			if (m_fileList.size() < 5)
				return *iter; 
			else
			{
				return *(m_fileList.rbegin()--);
			}
		}

		for (; iter != m_fileList.end() ; iter++)
		{
			if ((*iter) == fileName)
			{
				if (++iter == m_fileList.end())
					iter--;
				return *iter;
			}
		}
		return  *(m_fileList.rbegin()--);
	}
	
public:
	std::list<std::string> m_fileList;
};

static ClientObj g_client;
void __stdcall FileNameCallBackFunc(const char *fileName)
{
	printf("---------------------fileName : %s\n", fileName);
	myMutex.lock();

	g_client.AddFile(fileName);
	if (g_client.m_fileList.size() > 5)
	{
		std::string deletefileName = *(g_client.m_fileList.begin());
		DeleteFileA(deletefileName.c_str());
		g_client.m_fileList.erase(g_client.m_fileList.begin());
		printf("---------------------file size : %d\n", g_client.m_fileList.size());
	}
	myMutex.unlock();
};

int CALLBACK MsgFunc(int sessionhandle, void* userdata, int errCode, void* param1, void* param2, void* param3, void* param4)
{
	return 0;
}

int CALLBACK DataFunc(int sessionhandle, void* userdata, int datatype, void* pdata, int ilen)
{
	if (datatype != 2)
	{
		return 0;
	}
	//fwrite(pdata, ilen, 1, g_finput);
	if (SessionId != -1)
	{
		SendData(SessionId, (unsigned char *)pdata, ilen);
		//printf("send to %d , len %d\n", SessionId, ilen);
	}
	return 0;
}



static int event_handler(struct mg_connection *conn) {

	std::string strRsp = "http://10.11.160.105/";
	std::string strReqUrl = conn->uri;
	if (strReqUrl.find("/rest/real/filename/") >= 0)
	{
		strReqUrl.erase(strReqUrl.find("/rest/real/filename/"), strlen("/rest/real/filename/"));
		myMutex.lock();

		if (g_client.m_fileList.size() > 0)
		{
			std::string strFileName = g_client.GetFileNameByPreName(strReqUrl);
			
			strRsp += strFileName;
			mg_send_data(conn, strRsp.c_str(), strRsp.length() + 1);
			std::cout << strRsp.c_str() << std::endl;
		}
		myMutex.unlock();

	}
	
	return 0;
}

void ProcessHttpReq()
{
	struct mg_server *server = mg_create_server(NULL);
	
	mg_set_option(server, "document_root", ".");      // Serve current directory  
	mg_set_option(server, "listening_port", "80");  // Open port 8080  
	mg_add_uri_handler(server, "/rest/real/filename", event_handler);
	for (;;) {
		mg_poll_server(server, 1000);   // Infinite loop, Ctrl-C to stop  
		if (g_isExitThread)
		{
			break;
		}
	}
	mg_destroy_server(&server);
}

int main(int argc, char* argv[])
{
	std::thread  HttpThread(ProcessHttpReq);

	init_dll();
	int m_iSessionHandle = -1;
	StreamClient_InitLib();
	m_iSessionHandle = StreamClient_CreateSession();
	if (m_iSessionHandle < 0)
	{
		printf("StreamClient_CreateSession failed!");
		return 0;
	}
	
	StreamClient_SetMsgCallBack(m_iSessionHandle, MsgFunc, NULL);
	StreamClient_SetPsDataCallBack(m_iSessionHandle, DataFunc, NULL);

	int ret = StreamClient_Start(m_iSessionHandle, NULL, "rtsp://admin:hik123456@10.11.160.25/h264/ch1/main/av_stream", "StreamClient", RTPRTSP_TRANSMODE, "admin", "12345");
	if (ret < 0)
	{
		printf("StreamClient_Start failed!");
		StreamClient_DestroySession(m_iSessionHandle);
		m_iSessionHandle = -1;
	}
	for (int i = 0; i < 100000; i++)
	{
		printf("进行第 %d次测试. \n", i);
		SessionId = CreateSession();
		SetFileNameCallback(SessionId, FileNameCallBackFunc);
		Sleep(200000000);
		SessionId = -1;
		DestorySession(-1);
	}
	
	std::cin.get();

	if (m_iSessionHandle >= 0)
	{
		StreamClient_Stop(m_iSessionHandle);
		StreamClient_DestroySession(m_iSessionHandle);
		m_iSessionHandle = -1;
	}

	StreamClient_FiniLib();
	fini_dll();
	return 0;
}

