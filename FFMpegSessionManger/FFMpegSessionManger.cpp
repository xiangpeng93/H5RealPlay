// FFMpegSessionManger.cpp : ���� DLL Ӧ�ó���ĵ���������
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

//�����Ự������ֵΪ�Ự������������ΪĿ������ʽ��������Ҫת�����Բ�����ò���
int __stdcall CreateSession(CODEID id)
{
	int nRet = 0;
	nRet = g_sessionManger->CreateSessionByCount();
	return nRet;
};
//�����Ự������Ϊ�Ự���
int __stdcall DestorySession(int sessionId)
{
	int nRet = 0;
	nRet = g_sessionManger->DestorySessionById(sessionId);
	return nRet;
};

//ѡ��������
int __stdcall SetPackType(int sessionId, PACKTYPE type)
{
	return 0;
};

//���������ص�
int __stdcall SetCallback(int sessionId, CallBackData callback)
{
	int nRet = 0;
	nRet = g_sessionManger->SetCallBackFunc(sessionId, callback);
	return nRet;
};

//���������ص�
int __stdcall SetFileNameCallback(int sessionId, FileNameCallBack callback)
{
	int nRet = 0;
	nRet = g_sessionManger->SetFileNameCallBackFunc(sessionId, callback);
	return nRet;
};

//��������������ָ���Ự
int __stdcall SendData(int sessionId, unsigned char *data, int len)
{
	int nRet = 0;
	if (sessionId != -1)
		nRet = g_sessionManger->SendDataById(sessionId, data, len);
	else
		nRet = -1;
	return nRet;
};



