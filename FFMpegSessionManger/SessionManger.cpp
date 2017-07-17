#include "stdafx.h"
#include "SessionManger.h"
#include "Session.h"


SessionManger::SessionManger()
{
	memset(m_iSessionId, 0, MAX_SESSION);
	//初始化ffmpeg
	av_register_all();
};

SessionManger::~SessionManger()
{
	
};


//创建成功，返回值为SessionID，失败为-1
int SessionManger::CreateSessionByCount()
{
	for (int i = 0; i < MAX_SESSION; i++)
	{
		if (m_iSessionId[i] == 0)
		{
			Session *newSession = new Session;
			m_mSession[i] = newSession;
			m_iSessionId[i] = 1;
			newSession->Start();
			return i;
		}
	}
	return -1;
};

//删除会话成功，返回值值为0，失败为-1
int SessionManger::DestorySessionById(int sessionId)
{
	if (sessionId == -1)
	{
		for (int i = 0; i < MAX_SESSION; i++)
		{
			if (m_iSessionId[i] == 1)
			{
				Session *Session = m_mSession[i];
				Session->Stop();
				delete Session;
				m_mSession.erase(i);
				m_iSessionId[i] = 0;
				return 0;
			}
		}
	}
	else if (m_iSessionId[sessionId] == 1)
	{
		delete m_mSession[sessionId];
		m_mSession.erase(sessionId);
		m_iSessionId[sessionId] = 0;
		return 0;
	}
	return -1;
};

int SessionManger::SetCallBackFunc(int sessionId, CallBackData callback)
{
	if (m_iSessionId[sessionId] == 1)
	{
		m_mSession[sessionId]->SetCallBackFunc(callback);
		return 0;
	}
	return -1;
};

int SessionManger::SetFileNameCallBackFunc(int sessionId, FileNameCallBack callback)
{
	if (m_iSessionId[sessionId] == 1)
	{
		m_mSession[sessionId]->SetFileNameCallBackFunc(callback);
		return 0;
	}
	return -1;
};

int SessionManger::SendDataById(int sessionId, unsigned char * buffer, int len)
{
	if (m_iSessionId[sessionId] == 1)
	{
		m_mSession[sessionId]->SendDataToQueue(buffer,len);
		return 0;
	}
	return -1;
};
