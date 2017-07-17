#pragma once
#include "common.h"
#include "FFMpegSessionManger.h"
class Session;

class SessionManger
{
public:
	SessionManger();
	~SessionManger();

	int CreateSessionByCount();
	int DestorySessionById(const int sessionId = -1);

	int SetCallBackFunc(int sessionId, CallBackData callback);
	int SetFileNameCallBackFunc(int sessionId, FileNameCallBack callback);
	int SendDataById(int sessionId, unsigned char * buffer, int len);
private:
	map<int, Session *> m_mSession;
	int m_iSessionId[MAX_SESSION];
};
