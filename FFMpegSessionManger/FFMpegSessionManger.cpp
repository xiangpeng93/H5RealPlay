// FFMpegSessionManger.cpp : 定义 DLL 应用程序的导出函数。
//

#include "SessionManger.h"

SessionManger *g_sessionManger;
int __stdcall init_dll()
{
	g_sessionManger = new SessionManger;
	return 0;
};
int __stdcall fini_dll()
{
	delete g_sessionManger;
	return 0;
};

//创建会话，返回值为会话句柄，输入参数为目标流格式，若不需要转流可以不输入该参数
int __stdcall CreateSession(CODEID id)
{
	int nRet = 0;
	nRet = g_sessionManger->CreateSessionByCount();
	return nRet;
};
//创建会话，输入为会话句柄
int __stdcall DestorySession(int sessionId)
{
	int nRet = 0;
	nRet = g_sessionManger->DestorySessionById(sessionId);
	return nRet;
};

//选择打包类型
int __stdcall SetPackType(int sessionId, PACKTYPE type)
{
	return 0;
};

//设置码流回调
int __stdcall SetCallback(int sessionId, CallBackData callback)
{
	int nRet = 0;
	nRet = g_sessionManger->SetCallBackFunc(sessionId, callback);
	return nRet;
};

//设置码流回调
int __stdcall SetFileNameCallback(int sessionId, FileNameCallBack callback)
{
	int nRet = 0;
	nRet = g_sessionManger->SetFileNameCallBackFunc(sessionId, callback);
	return nRet;
};

//发送码流数据至指定会话
int __stdcall SendData(int sessionId, unsigned char *data, int len)
{
	int nRet = 0;
	if (sessionId != -1)
		nRet = g_sessionManger->SendDataById(sessionId, data, len);
	else
		nRet = -1;
	return nRet;
};



