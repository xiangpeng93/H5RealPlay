/**	@file StreamClient.h
 *	@note HangZhou Hikvision System Technology Co., Ltd. All Right Reserved.
 *	@brief ��ý��ͻ��ˡ�
 *
 *	@author		zhaojian
 *	@date		2012/07/30
 *
 *	@note ��ʷ��¼��
                    2013/06/10 ������ý��ͻ��˰汾��4.1.0��������ϸ��־��ע�ͣ��ع�����
 *	@note 
 *
 *	@warning 
 */


#ifndef __STREAMCLIENT_H__
#define __STREAMCLIENT_H__

// ���䷽ʽ
#define RTPRTSP_TRANSMODE  0x9000			// RTP over RTSP
#define RTPUDP_TRANSMODE   0x9002			// RTP over UDP
#define RTPMCAST_TRANSMODE 0x9004			// RTP over multicast

// ����������
#define STREAM_HEAD         0x0001          // ϵͳͷ����
#define STREAM_DATA         0x0002          // ������
#define STREAM_PLAYBACK_FINISH 0x0064       // �طš����ػ򵹷Ž�����ʶ

/**	@struct _ABS_TIME_
 *  @brief ����ʱ��ط�ʱ������ṹ�塣
 *
 */
typedef struct _ABS_TIME_
{
    unsigned int dwYear;    ///< ��
    unsigned int dwMonth;   ///< ��
    unsigned int dwDay;     ///< ��
    unsigned int dwHour;    ///< ʱ
    unsigned int dwMintes;  ///< ��
    unsigned int dwSeconds; ///< ��
}ABS_TIME, *pABS_TIME;

/**	@struct tagPOINT_FRAME
 *  @brief ��̨���ƾֲ�����Ŵ�
 *
 * Ϊ�����ϰ汾��δʹ��
 */
typedef struct tagPOINT_FRAME
{
    int xTop;
    int yTop;				
    int xBottom;			
    int yBottom;			
    int bCounter;			
}POINT_FRAME, *LPPOINT_FRAME;

#if defined(_MSC_VER)
    typedef signed __int64	INT64;
#elif defined(__GNUC__) || defined(__SYMBIAN32__)
    #if defined(__LP64__)
        typedef signed long INT64;
    #else
        typedef signed long long INT64;
    #endif
#endif

/**	@struct errorInfo_platform
 *  @brief ������Ϣ�ṹ��
 *
 */
typedef struct errorInfo_platform
{
    char moduleID[32];      ///< ģ��ID
    char businessID[32];    ///< ҵ��ID
    INT64 timestamp;        ///< ʱ���
    int errorCode;          ///< ������
    char errorMsg[32];      ///< ��������
}ERRORINFO_PLATFORM,*PERRORINFO_PLATFORM;

/**	@struct errorStackInfo_platform
 *  @brief �����ջ��Ϣ�ṹ��
 *
 */
typedef struct errorStackInfo_platform
{
    int* count;                     ///< ��ǰ�����ջ���
    PERRORINFO_PLATFORM perrorInfo; ///< �����ջ��Ϣ
}ERRORSTACKINFO_PLATFORM,*PERRORSTACKINFO_PLATFORM;

// �����ϰ汾����ý�壨δʹ�ã�
class IHikClientAdviseSink;

#if (defined(WIN32) || defined(WIN64))
    #if defined(STREAMCLIENT_EXPORTS)
        #define HPR_STREAMCLIENT_DECLARE extern "C" __declspec(dllexport)
    #else
        #define HPR_STREAMCLIENT_DECLARE extern "C" __declspec(dllimport)
    #endif
    #ifndef CALLBACK
        #define CALLBACK __stdcall
    #endif
#else
    #define HPR_STREAMCLIENT_DECLARE extern "C"
    #ifndef CALLBACK
        #define CALLBACK
    #endif
#endif

/**************************/
/*��Ϣ�ص�������������*/
/**********************************************
sessionhandle �Ự���
userdata      �û��Զ�������
errCode       ������
param1        ������Ϣ�ṹ��
param2        ��ʱδ����
param3        ��ʱδ����
param4        ��ʱδ����
�ɹ�����0 ʧ�ܷ���-1
************************************************/
typedef int (CALLBACK *pStreamClientMsgFunc)(int sessionhandle, void* userdata, int errCode, void* param1, void* param2, void* param3, void* param4);

/**************************/
/*ԭʼ���ݣ�����ý����������յ����ݣ��ص�������������*/
/***********************************************
sessionhandle �Ự���
userdata      �û��Զ�������
pdata         �������ݣ�STREAM_HEAD ϵͳͷ���ݣ�STREAM_DATA �����ݣ�STREAM_PLAYBACK_FINISH �طš����ػ򵹷Ž�����
datalen       �������ݳ���
�ɹ�����0 ʧ�ܷ���-1
***********************************************/
typedef int (CALLBACK *pStreamClientDataFunc)(int sessionhandle, void* userdata, int datatype, void* pdata, int datalen);

/**************************/
/*PS��װ���ݻص������������壬�����յ�����֧��ת��װ��PS�������ݻص��������ݻص���*/
/*����֧��PSת��װʱ���ص�ԭʼ����*/
/***********************************************
sessionhandle �Ự���
userdata      �û��Զ�������
pdata         �������ݣ�STREAM_HEAD ϵͳͷ���ݣ�STREAM_DATA �����ݣ�STREAM_PLAYBACK_FINISH �طš����ػ򵹷Ž�����
datalen       �������ݳ���
�ɹ�����0 ʧ�ܷ���-1
***********************************************/
typedef int (CALLBACK *pStreamClientPsDataFunc)(int sessionhandle, void* userdata, int datatype, void* pdata, int datalen);

//�����ϰ汾����ý�壨δʹ�ã�
typedef int (CALLBACK *pDataRec)(int sid, int iusrdata, int idatatype, char* pdata, int ilen);
typedef int (CALLBACK *pMsgBack)(int sid, int opt, int param1, int param2);

// �����ϰ汾����ý�壨δʵ�֣�
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_SetOcxFlag(bool bOcx);

/**	@fn	HPR_STREAMCLIENT_DECLARE HPR_INT32 CALLBACK StreamClient_InitLib(HPR_VOID)
 *	@brief ��ʼ����ý��ͻ��ˣ��м������ܡ�
 *	@param ��.
 *	@return	�ɹ�����0��ʧ�ܷ��ش����루��ϸ���Ͳ����������ĵ���
 */
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_InitLib(void);

/**	@fn	HPR_STREAMCLIENT_DECLARE HPR_INT32 CALLBACK StreamClient_FiniLib(HPR_VOID)
 *	@brief ����ʼ����ý��ͻ��ˣ��м������ܡ�
 *	@param ��.
 *	@return	�ɹ�����0��ʧ�ܷ��ش����루��ϸ���Ͳ����������ĵ���
 */
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_FiniLib(void);

/**	@fn	HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_CreateSession(void)
 *	@brief ������ý��ͻ��˻Ự���
 *	@param ��.
 *	@return	�ɹ����ػỰ������ڵ����㣬ʧ�ܷ��ظ���
 */
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_CreateSession(void);

/**	@fn	HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_DestroySession(int sessionhandle)
 *	@brief ���ٻỰ
 *	@param sessionhandle[in]  ��Ҫ���ٵĻỰ�����StreamClient_CreateSession��������ֵ��.
 *	@return	�ɹ�����0��ʧ�ܷ��ش����루��ϸ���Ͳ����������ĵ���
 */
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_DestroySession(int sessionhandle);

/**	@fn	HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_Start(int sessionhandle, void* windowhandle, char *url, char *useragent, int transmethod,\
                                                                char *deviceusername, char *devicepasswd)
 *	@brief ����ʵʱԤ��
 *	@param sessionhandle[in]    �Ự�����StreamClient_CreateSession��������ֵ��.
 *	@param windowhandle[in]     ���ھ����δʹ�ã���NULL��.
 *	@param url[in]              ʵʱԤ��url��url��ʽ�μ���ý��4.0�ͻ���������ʹ��˵����.doc��.
 *	@param useragent[in]        �ͻ������ƣ����磺StreamClient��.
 *	@param transmethod[in]      ���䷽ʽ��RTPRTSP_TRANSMODE��RTPUDP_TRANSMODE��.
 *	@param deviceusername[in]   ʵʱԤ���豸���û���.
 *	@param devicepasswd[in]     ʵʱԤ���豸������.
 *	@return	�ɹ�����0��ʧ�ܷ��ش����루��ϸ���Ͳ����������ĵ���
 */
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_Start(int sessionhandle, void* windowhandle, char *url, char *useragent, int transmethod,\
												  char *deviceusername, char *devicepasswd);

/**	@fn	HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_DownLoad(int sessionhandle, void* windowhandle, char* url, char* useragent, int transmethod, \
                                                                    char *deviceusername, char *devicepasswd, pABS_TIME from = NULL, pABS_TIME to = NULL)
 *	@brief �������ع���
 *	@param sessionhandle[in]    �Ự�����StreamClient_CreateSession��������ֵ��.
 *	@param windowhandle[in]     ���ھ����δʹ�ã���NULL��.
 *	@param url[in]              ����url��url��ʽ�μ���ý��4.0�ͻ���������ʹ��˵����.doc��.
 *	@param useragent[in]        �ͻ������ƣ����磺StreamClient��.
 *	@param transmethod[in]      ���䷽ʽ��RTPRTSP_TRANSMODE��RTPUDP_TRANSMODE��.
 *	@param deviceusername[in]   ����¼���Ӧ�豸���û���.
 *	@param devicepasswd[in]     ����¼���Ӧ�豸������.
 *	@param from[in]             ����¼���ļ��Ŀ�ʼ����.
 *	@param to[in]               ����¼���ļ��Ľ�������.
 *	@return	�ɹ�����0��ʧ�ܷ��ش����루��ϸ���Ͳ����������ĵ���
 */
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_DownLoad(int sessionhandle, void* windowhandle, char* url, char* useragent, int transmethod, \
												  char *deviceusername, char *devicepasswd, pABS_TIME from = NULL, pABS_TIME to = NULL);

/**	@fn	HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_PlayBackByTime(int sessionhandle, void* windowhandle, char* url, char* useragent, int transmethod, \
                                                                          char *deviceusername, char *devicepasswd, pABS_TIME from, pABS_TIME to)
 *	@brief ������ʱ�俪���ط�
 *	@param sessionhandle[in]    �Ự�����StreamClient_CreateSession��������ֵ��.
 *	@param windowhandle[in]     ���ھ����δʹ�ã���NULL��.
 *	@param url[in]              ����ʱ��ط�url��url��ʽ�μ���ý��4.0�ͻ���������ʹ��˵����.doc��.
 *	@param useragent[in]        �ͻ������ƣ����磺StreamClient��.
 *	@param transmethod[in]      ���䷽ʽ��RTPRTSP_TRANSMODE��RTPUDP_TRANSMODE��.
 *	@param deviceusername[in]   ����ʱ��طŶ�Ӧ�豸���û���.
 *	@param devicepasswd[in]     ����ʱ��طŶ�Ӧ�豸������.
 *	@param from[in]             ����ʱ��ط�¼���ļ��Ŀ�ʼ����.
 *	@param to[in]               ����ʱ��ط�¼���ļ��Ľ�������.
 *	@return	�ɹ�����0��ʧ�ܷ��ش����루��ϸ���Ͳ����������ĵ���
 */
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_PlayBackByTime(int sessionhandle, void* windowhandle, char* url, char* useragent, int transmethod, \
												  char *deviceusername, char *devicepasswd, pABS_TIME from, pABS_TIME to);

// �����ϰ汾����ý�壨δʵ�֣�
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_Describe(int sessionhandle, void* windowhandle, char *url, char *useragent, int transmethod,\
                                                                  char *deviceusername, char *devicepasswd);
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_Setup(int sessionhandle);
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_Play(int sessionhandle);
// end

/**	@fn	HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_Stop(int sessionhandle)
 *	@brief ֹͣȡ��������ʵʱԤ�������ء��طŵȣ�
 *	@param sessionhandle[in]    �Ự�����StreamClient_CreateSession��������ֵ��.
 *	@return	�ɹ�����0��ʧ�ܷ��ش����루��ϸ���Ͳ����������ĵ���
 */
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_Stop(int sessionhandle);

/**	@fn	HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_Pause(int sessionhandle)
 *	@brief ��ͣȡ�����������ء��طŵȣ�
 *	@param sessionhandle[in]    �Ự�����StreamClient_CreateSession��������ֵ��.
 *	@return	�ɹ�����0��ʧ�ܷ��ش����루��ϸ���Ͳ����������ĵ���
 */
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_Pause(int sessionhandle);

/**	@fn	HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_Resume(int sessionhandle)
 *	@brief �ָ�ȡ�����������ء��طŵȣ�
 *	@param sessionhandle[in]    �Ự�����StreamClient_CreateSession��������ֵ��.
 *	@return	�ɹ�����0��ʧ�ܷ��ش����루��ϸ���Ͳ����������ĵ���
 */
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_Resume(int sessionhandle);

/**	@fn	HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_RandomPlay(int sessionhandle, int from, int to)
 *	@brief ���ʱ��ط�seek����
 *	@param sessionhandle[in]    �Ự�����StreamClient_CreateSession��������ֵ��.
 *	@param from[in]             ���ʱ��ط�seek�Ŀ�ʼʱ��.
 *	@param to[in]               ���ʱ��ط�seek�Ľ���ʱ��.
 *	@return	�ɹ�����0��ʧ�ܷ��ش����루��ϸ���Ͳ����������ĵ���
 */
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_RandomPlay(int sessionhandle, int from, int to);

/**	@fn	HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_RandomPlayByAbs(int sessionhandle, pABS_TIME from, pABS_TIME to)
 *	@brief ����ʱ��ط�seek����
 *	@param sessionhandle[in]    �Ự�����StreamClient_CreateSession��������ֵ��.
 *	@param from[in]             ����ʱ��ط�seek�Ŀ�ʼʱ��.
 *	@param to[in]               ����ʱ��ط�seek�Ľ���ʱ��.
 *	@return	�ɹ�����0��ʧ�ܷ��ش����루��ϸ���Ͳ����������ĵ���
 */
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_RandomPlayByAbs(int sessionhandle, pABS_TIME from, pABS_TIME to);

/**	@fn	HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_ChangeRate(int sessionhandle, float scale)
 *	@brief �޸ĻطŻ����صı���
 *	@param sessionhandle[in]    �Ự�����StreamClient_CreateSession��������ֵ��.
 *	@param scale[in]            ����ֵ��������Чֵ�μ���ý��4.0�ͻ���������ʹ��˵����.doc��.
 *	@return	�ɹ�����0��ʧ�ܷ��ش����루��ϸ���Ͳ����������ĵ���
 */
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_ChangeRate(int sessionhandle, float scale);

/**	@fn	HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_SetMsgCallBack(int sessionhandle, pStreamClientMsgFunc pMsgFunc, void* userdata)
 *	@brief ������Ϣ�ص�
 *	@param sessionhandle[in]    �Ự�����StreamClient_CreateSession��������ֵ��.
 *	@param pMsgFunc[in]         ��Ϣ�ص�����ָ��.
 *	@param userdata[in]         �û��Զ�������.
 *	@return	�ɹ�����0��ʧ�ܷ��ش����루��ϸ���Ͳ����������ĵ���
 */
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_SetMsgCallBack(int sessionhandle, pStreamClientMsgFunc pMsgFunc, void* userdata);

/**	@fn	HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_SetDataCallBack(int sessionhandle, pStreamClientDataFunc pDataFunc, void* userdata)
 *	@brief �������ݻص�
 *	@param sessionhandle[in]    �Ự�����StreamClient_CreateSession��������ֵ��.
 *	@param pDataFunc[in]        ���ݻص�����ָ��.
 *	@param userdata[in]         �û��Զ�������.
 *	@return	�ɹ�����0��ʧ�ܷ��ش����루��ϸ���Ͳ����������ĵ���
 */
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_SetDataCallBack(int sessionhandle, pStreamClientDataFunc pDataFunc, void* userdata);

// �����ϰ汾����ý�壨δʵ�֣�
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_Set_DevicePushdata(int sessionhandle, bool buse = false);
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_Set_StandardStream(int sessionhandle, bool bstandardstream = false);
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_SetRTPTCPPortRange(unsigned short count,\
                                                                            unsigned short baseport);
// end

/**	@fn	HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_SetRTPUDPPortRange(unsigned short count,\
                                                                              unsigned short baseport)
 *	@brief ����UDP�˿ڷ�Χ
 *	@param count[in]    �˿ڶ���.
 *	@param baseport[in] ��ʼ�˿�.
 *	@return	�ɹ�����0��ʧ�ܷ��ش����루��ϸ���Ͳ����������ĵ���
 */
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_SetRTPUDPPortRange(unsigned short count,\
																	  unsigned short baseport);

// �����ϰ汾����ý�壨δʵ�֣�
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_SetRTPUDPRetrans(int sessionhandle, bool buse);
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_SetPortShare(int level);
// end

// �����ϰ汾����ý�壨δʵ�֣�
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_Backward(int sessionhandle, pABS_TIME pfrom, pABS_TIME pto = NULL);
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_GetCurTime(int sessionhandle, unsigned long *utime);
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_SetVolume(int sessionhandle, unsigned short volume);
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_OpenSound(int sessionhandle, bool bExclusive = false);
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_CloseSound(int sessionhandle);
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_ThrowBFrameNum(int sessionhandle, unsigned int nNum);
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_GrabPic(int sessionhandle, const char* szPicFileName,\
                                                                 unsigned short byPicType);
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_ResetWndHandle(int sessionhandle, void* pWndSiteHandle);
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_RefreshPlay(int sessionhandle);
//HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_RegisterDrawFun(int sessionhandle,\
//                                                                         void(CALLBACK *DrawFun)(long nPort, HDC hDC, long nUser), long nUser);
// end
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_GetVideoParams(int sessionhandle, int *ibri, int *icon, int *isat, int *ihue);
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_SetVideoParams(int sessionhandle, int ibri, int icon, int isat, int ihue);
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_PTZControl(int sessionhandle, unsigned int ucommand, int iparam1, int iparam2, int iparam3, int iparam4);
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_PTZSelZoomIn(int sessionhandle, LPPOINT_FRAME pStruPointFrame);
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_OneByOne(int sessionhandle);
// end

/**	@fn	HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_ForceIFrame(int sessionhandle)
 *	@brief ǿ��I֡
 *	@param sessionhandle[in]    �Ự�����StreamClient_CreateSession��������ֵ��.
 *	@return	�ɹ�����0��ʧ�ܷ��ش����루��ϸ���Ͳ����������ĵ���
 */
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_ForceIFrame(int sessionhandle);

// �����ϰ汾����ý�壨δʵ�֣�
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_InputAudioData(int sessionhandle, char* pDataBuf, int iDataSize);

HPR_STREAMCLIENT_DECLARE int CALLBACK InitStreamClientLib(void);
HPR_STREAMCLIENT_DECLARE int CALLBACK FiniStreamClientLib(void);
HPR_STREAMCLIENT_DECLARE int CALLBACK HIKS_CreatePlayer(IHikClientAdviseSink* pSink, void *pWndSiteHandle, pDataRec pRecFunc, pMsgBack pMsgFunc = 0, int TransMethod = 0);
HPR_STREAMCLIENT_DECLARE int CALLBACK HIKS_OpenURL(int hSession, const char* pszURL, int iusrdata);
HPR_STREAMCLIENT_DECLARE int CALLBACK HIKS_Play(int hSession);
HPR_STREAMCLIENT_DECLARE int CALLBACK HIKS_RandomPlay(int hSession, unsigned long timepos);
HPR_STREAMCLIENT_DECLARE int CALLBACK HIKS_Pause(int hSession);
HPR_STREAMCLIENT_DECLARE int CALLBACK HIKS_Resume(int hSession);
HPR_STREAMCLIENT_DECLARE int CALLBACK HIKS_Stop(int hSession);
HPR_STREAMCLIENT_DECLARE int CALLBACK HIKS_ChangeRate(int hSession, int scale);
HPR_STREAMCLIENT_DECLARE int CALLBACK HIKS_Destroy(int hSession);

HPR_STREAMCLIENT_DECLARE int CALLBACK HIKS_GetVideoParams(int hSession, int *ibri, int *icon, int *isat, int *ihue);
HPR_STREAMCLIENT_DECLARE int CALLBACK HIKS_SetVideoParams(int hSession, int ibri, int icon, int isat, int ihue);
HPR_STREAMCLIENT_DECLARE int CALLBACK HIKS_PTZControl(int hSession, unsigned int ucommand, int iparam1, int iparam2, int iparam3, int iparam4);
HPR_STREAMCLIENT_DECLARE int CALLBACK HIKS_PTZSelZoomIn(int hSession, LPPOINT_FRAME pStruPointFrame);

HPR_STREAMCLIENT_DECLARE int CALLBACK HIKS_GetCurTime(int hSession, unsigned long* utime);
HPR_STREAMCLIENT_DECLARE int CALLBACK HIKS_SetVolume(int hSession, unsigned short volume);
HPR_STREAMCLIENT_DECLARE int CALLBACK HIKS_OpenSound(int hSession, bool bExclusive = false);
HPR_STREAMCLIENT_DECLARE int CALLBACK HIKS_CloseSound(int hSession);
HPR_STREAMCLIENT_DECLARE int CALLBACK HIKS_ThrowBFrameNum(int hSession, unsigned int nNum);
HPR_STREAMCLIENT_DECLARE int CALLBACK HIKS_GrabPic(int hSession, const char* szPicFileName, unsigned short byPicType);
HPR_STREAMCLIENT_DECLARE int CALLBACK HIKS_ResetWndHandle(int hSession, void *pWndSiteHandle);
HPR_STREAMCLIENT_DECLARE int CALLBACK HIKS_ResetDataCallBack(int hSession, pDataRec pRecFunc);
HPR_STREAMCLIENT_DECLARE int CALLBACK HIKS_GetHeader(int hSession, char* buf, int len);
HPR_STREAMCLIENT_DECLARE int CALLBACK HIKS_RefreshPlay(int hSession);
HPR_STREAMCLIENT_DECLARE bool CALLBACK HIKS_GetPlayPort(int hSession, unsigned long* port);
HPR_STREAMCLIENT_DECLARE bool CALLBACK HIKS_SetDDrawDevice(int hSession, unsigned long nDeviceNum);
HPR_STREAMCLIENT_DECLARE bool CALLBACK HIKS_InitDDrawDevice(void);
HPR_STREAMCLIENT_DECLARE void CALLBACK HIKS_ReleaseDDrawDevice(void);

HPR_STREAMCLIENT_DECLARE int CALLBACK HIKS_SetOcxFlag(bool bOcx);
HPR_STREAMCLIENT_DECLARE int CALLBACK HIKS_ForceIFrame(int hSession);
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_SetClusterAddr(char* rootaddrs);
//end

/**	@fn	HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_PlayBackByOppositeTime(int sessionhandle, char* url, char* useragent, int transmethod, \
                                                                                  char *deviceusername, char *devicepasswd, int from, int to)
 *	@brief �����ʱ�俪���ط�
 *	@param sessionhandle[in]    �Ự�����StreamClient_CreateSession��������ֵ��.
 *	@param windowhandle[in]     ���ھ����δʹ�ã���NULL��.
 *	@param url[in]              ���ʱ��ط�url��url��ʽ�μ���ý��4.0�ͻ���������ʹ��˵����.doc��.
 *	@param useragent[in]        �ͻ������ƣ����磺StreamClient��.
 *	@param transmethod[in]      ���䷽ʽ��RTPRTSP_TRANSMODE��RTPUDP_TRANSMODE��.
 *	@param deviceusername[in]   ���ʱ��طŶ�Ӧ�豸���û���.
 *	@param devicepasswd[in]     ���ʱ��طŶ�Ӧ�豸������.
 *	@param from[in]             ���ʱ��ط�¼���ļ��Ŀ�ʼ����.
 *	@param to[in]               ���ʱ��ط�¼���ļ��Ľ������ڣ�-1��ʾ���ŵ��ļ�������.
 *	@return	�ɹ�����0��ʧ�ܷ��ش����루��ϸ���Ͳ����������ĵ���
 */
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_PlayBackByOppositeTime(int sessionhandle, char* url, char* useragent, int transmethod, \
                                                                          char *deviceusername, char *devicepasswd, int from, int to);

/**	@fn	HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_SetPsDataCallBack(int sessionhandle, pStreamClientDataFunc pDataFunc, void* userdata)
 *	@brief ����PS��װ���ݻص�������������ת��PS���˺���ָ����������ص���
 *	@param sessionhandle[in]    �Ự�����StreamClient_CreateSession��������ֵ��.
 *	@param pDataFunc[in]        ���ݻص�����ָ��.
 *	@param userdata[in]         �û��Զ�������.
 *	@return	�ɹ�����0��ʧ�ܷ��ش����루��ϸ���Ͳ����������ĵ���
 */
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_SetPsDataCallBack(int sessionhandle, pStreamClientPsDataFunc pDataFunc, void* userdata);

/**	@fn	HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_GetErrMsgByErrCode(int errCode)
 *	@brief ���ݴ������ȡ����������Ϣ
 *	@param errCode[in]  ������.
 *	@return	����������Ϣ
 */
HPR_STREAMCLIENT_DECLARE const char* CALLBACK StreamClient_GetErrMsgByErrCode(int errCode);

/**	@fn	HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_BackwardEx(int sessionhandle, const char* url, const char* useragent, int transmethod, \
                                                                      const char *deviceusername, const char *devicepasswd, pABS_TIME pfrom, pABS_TIME pto)
 *	@brief ��������
 *	@param sessionhandle[in]    �Ự�����StreamClient_CreateSession��������ֵ��.
 *	@param url[in]              ����ʱ�䵹��url��url��ʽ�μ���ý��4.0�ͻ���������ʹ��˵����.doc��.
 *	@param useragent[in]        �ͻ������ƣ����磺StreamClient��.
 *	@param transmethod[in]      ���䷽ʽ��RTPRTSP_TRANSMODE��RTPUDP_TRANSMODE��.
 *	@param deviceusername[in]   ���Ŷ�Ӧ�豸���û���.
 *	@param devicepasswd[in]     ���Ŷ�Ӧ�豸������.
 *	@param pfrom[in]            ����¼���ļ��Ŀ�ʼ����.
 *	@param pto[in]              ����¼���ļ��Ľ�������.
 *	@return	�ɹ�����0��ʧ�ܷ��ش����루��ϸ���Ͳ����������ĵ���
 */
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_BackwardEx(int sessionhandle, const char* url, const char* useragent, int transmethod, \
                                                              const char *deviceusername, const char *devicepasswd, pABS_TIME pfrom, pABS_TIME pto);

/**	@fn	HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_BackwardRandomPlay(int sessionhandle, pABS_TIME from, pABS_TIME to)
 *	@brief ����seek����
 *	@param sessionhandle[in]    �Ự�����StreamClient_CreateSession��������ֵ��.
 *	@param pfrom[in]             ����seek�Ŀ�ʼʱ��.
 *	@param pto[in]               ����seek�Ľ���ʱ��.
 *	@return	�ɹ�����0��ʧ�ܷ��ش����루��ϸ���Ͳ����������ĵ���
 */
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_BackwardRandomPlay(int sessionhandle, pABS_TIME pfrom, pABS_TIME pto);

/**	@fn	HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_PushData(int sessionHandle, const char *url, const char *userAgent, \
                                                                    const char *deviceUsername, const char *devicePasswd, const char* destIp, \
                                                                    unsigned short destPort, int transMethod = RTPUDP_TRANSMODE)
 *	@brief ������������
 *	@param sessionHandle[in]    �Ự�����StreamClient_CreateSession��������ֵ��.
 *	@param url[in]              ʵʱԤ��URL.
 *	@param userAgent[in]        �ͻ������ƣ����磺StreamClient��.
 *	@param deviceUsername[in]   �û���.
 *	@param devicePasswd[in]     ����.
 *	@param destIp[in]           Ŀ��IP.
 *	@param destPort[in]         Ŀ��˿�.
 *	@param transMethod[in]      ���䷽ʽ����֧��RTPUDP_TRANSMODE��.
 *	@return	�ɹ�����0��ʧ�ܷ��ش����루��ϸ���Ͳ����������ĵ���
 */
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_PushData(int sessionHandle, const char* url, const char* userAgent, \
                                                            const char* deviceUsername, const char* devicePasswd, const char* destIp, \
                                                            unsigned short destPort, int transMethod = RTPUDP_TRANSMODE);

/**	@fn	HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_SetExtractFrame(int sessionhandle, int iExtractFrame)
 *	@brief ��֡/����֡���������ڻطţ�
 *	@param sessionhandle[in]  �Ự�����StreamClient_CreateSession��������ֵ��.
 *	@param iExtractFrame[in]  ��֡��ʶ����0������֡��1����֡��.
 *	@return	�ɹ�����0��ʧ�ܷ��ش����루��ϸ���Ͳ����������ĵ���
 */
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_SetExtractFrame(int sessionhandle, int iExtractFrame);

/**	@fn	HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_SetRtspTimeout(int sessionhandle, unsigned int timeout)
 *	@brief ����RTSP���ʱʱ��
 *	@param timeout[in]     RTSP���ʱʱ��, �ԡ��롱Ϊ��λ.
 *	@return	void
 */
HPR_STREAMCLIENT_DECLARE int CALLBACK StreamClient_SetRtspTimeout(int sessionhandle, unsigned int timeout);

// ��������ֵ�����붨��
#define STREAM_CLIENT_NO_RTSP_SESSION               -3  ///< ��ý��ͻ��˻Ự�Ѿ��þ�
#define STREAM_CLIENT_NO_INIT                       -2  ///< ��ý��ͻ���δ��ʼ��
#define STREAM_CLIENT_ERR_NO_DEF                    -1  ///< ����δ����
#define STREAM_CLIENT_NOERROR                       0   ///< û�д���
#define STREAM_CLIENT_SAESSION_INVALID              1   ///< �Ự��Ч
#define STREAM_CLIENT_OVER_MAX_CONN                 2   ///< ������ý���û������������
#define STREAM_CLIENT_NETWORK_FAIL_CONNECT          3   ///< �����豸ʧ�ܡ��豸�����߻�����ԭ����������ӳ�ʱ�ȡ�
#define STREAM_CLIENT_DEVICE_OFF_LIEN               4   ///< �豸����
#define STREAM_CLIENT_DEVICE_OVER_MAX_CONN          5   ///< �豸�������������
#define STREAM_CLIENT_RECV_ERROR                    6   ///< ����ý���������������ʧ��
#define STREAM_CLIENT_RECV_TIMEOUT                  7   ///< ����ý��������������ݳ�ʱ
#define STREAM_CLIENT_SEND_ERROR                    8   ///< ����ý���������������ʧ��
#define STREAM_CLIENT_TRANSMETHOD_INVALID           9   ///< ���䷽ʽ��Ч������RTP/RTSP��RTP/UDP��RTP/MCAST��
#define STREAM_CLIENT_CREATESOCKET_ERROR            10  ///< ����SOCKETʧ��
#define STREAM_CLIENT_SETSOCKET_ERROR               11  ///< ����SOCKETʧ��
#define STREAM_CLIENT_BINDSOCKET_ERROR              12  ///< ��SOCKETʧ��
#define STREAM_CLIENT_LISTENSOCKET_ERROR            13  ///< ����SOCKETʧ��
#define STREAM_CLIENT_URL_FORMAT_ERROR              14  ///< URL��ʽ����
#define STREAM_CLIENT_RTSP_STATE_INVALID            15  ///< RTSP״̬��Ч
#define STREAM_CLIENT_RTSP_RSP_ERROR                16  ///< RTSP��Ӧ�����﷨����δ����������ֶΣ�
#define STREAM_CLIENT_RTSP_RSP_STATE_ERROR          17  ///< RTSP����״̬ʧ�ܣ�������200��302
#define STREAM_CLIENT_PARSE_SDP_FAIL                18  ///< ����SDPʧ��
#define STREAM_CLIENT_PARSE_RTSP_FAIL               19  ///< ����RTSP����ʧ�ܣ����磺��ȡRTSPĳ�ֶ�ʧ�ܣ�RTSP��
#define STREAM_CLIENT_MEDIACOUNT_LESS_ZERO          20  ///< ����SDPʱ��δ������traceID
#define STREAM_CLIENT_SEND_DESCRIBE_FAIL            21  ///< ����describe����ʧ��
#define STREAM_CLIENT_SEND_SETUP_FAIL               22  ///< ����setup����ʧ��
#define STREAM_CLIENT_SEND_PLAY_FAIL                23  ///< ����play����ʧ��
#define STREAM_CLIENT_SEND_PAUSE_FAIL               24  ///< ����pause����ʧ��
#define STREAM_CLIENT_SEND_TRERDOWN_FAIL            25  ///< ����teardown����ʧ��
#define STREAM_CLIENT_RECV_DESCRIBE_TIMEOUT         26  ///< ����describe��ʱ
#define STREAM_CLIENT_RECV_SETUP_TIMEOUT            27  ///< ����setup��ʱ
#define STREAM_CLIENT_RECV_PLAY_TIMEOUT             28  ///< ����play��ʱ
#define STREAM_CLIENT_RECV_PAUSE_TIMEOUT            29  ///< ����pause��ʱ
#define STREAM_CLIENT_RECV_TEARDOWN_TIMEOUT         30  ///< ����teardown��ʱ
#define STREAM_CLIENT_DESCRIBE_RSP_ERROR            31  ///< describe��Ӧ����
#define STREAM_CLIENT_SETUP_RSP_ERROR               32  ///< setup��Ӧ����
#define STREAM_CLIENT_PLAY_RSP_ERROR                33  ///< play��Ӧ����
#define STREAM_CLIENT_PAUSE_RSP_ERROR               34  ///< pause��Ӧ����
#define STREAM_CLIENT_TEARDOWN_RSP_ERROR            35  ///< teardown��Ӧ����
#define STREAM_CLIENT_REDIRECT_ERROR                36  ///< �ض���ʧ��
#define STREAM_CLIENT_UDP_GET_SERVER_PROT_FAIL	    37  ///< ��RTSP��setup��������������˿�ʧ��
#define STREAM_CLIENT_CREATE_UPD_CONNECT_FAIL	    38  ///< ����UDP�첽��������ʧ��
#define STREAM_CLIENT_OPEN_UDP_CONNECT_FAIL	        39  ///< ��UDP�첽��������ʧ��
#define STREAM_CLIENT_UDP_ASYNC_RECV_FAIL	        40  ///< UDPͶ���첽��������ʧ��
#define STREAM_CLIENT_OPT_BACK_DATE_ERROR	        41	///< ���ʱ��ط�ʱ�����
#define STREAM_CLIENT_ABS_BACK_DATE_ERROR	        42	///< ����ʱ��ط�ʱ�����
#define STREAM_CLIENT_MSG_CB_INVALID                43  ///< ��Ϣ�ص����ô���
#define STREAM_CLIENT_SEND_PTZ_FAILED               44  ///< ������̨��������ʧ��
#define STREAM_CLIENT_SEND_FORCEIFRAM_FAILED        45  ///< ����ǿ��I֡����ʧ��
#define STREAM_CLIENT_SEND_GETVEDIOPARAM_FAILED     46  ///< ���ͻ�ȡ��Ƶ��������ʧ��
#define STREAM_CLIENT_SEND_SETVEDIOPARAM_FAILED     47  ///< ����������Ƶ��������ʧ��
#define STREAM_CLIENT_RECV_PTZ_TIMEOUT              48  ///< ������̨�������ʱ
#define STREAM_CLIENT_RECV_FORCEIFRAM_TIMEOUT       49  ///< ����ǿ��I֡���ʱ
#define STREAM_CLIENT_RECV_GETVEDIOPARAM_TIMEOUT    50  ///< ���ջ�ȡ��Ƶ�������ʱ
#define STREAM_CLIENT_RECV_SETVEDIOPARAM_TIMEOUT    51  ///< ����������Ƶ�������ʱ
#define STREAM_CLIENT_FUNCTION_NO_ACHIEVE           52  ///< ����δʵ��
#define STREAM_CLIENT_CONFIG_RTSP_SESSION_FAILED    53  ///< ����RTSP�Ựʱ��ĳ������Ч
#define STREAM_CLIENT_FUNC_PARAM_INVALID            54  ///< ����������Ч
#define STREAM_CLIENT_SESSION_POINTER_INVALID       55  ///< �Ựָ����Ч
#define STREAM_CLIENT_MEMORY_LACK                   56  ///< �ڴ治��������ڴ�ʧ��
#define STREAM_CLIENT_SEND_SETPARAMETER_FAILED      57  ///< �������ò�������ʧ��
#define STREAM_CLIENT_RECV_SETPARAMETER_TIMEOUT     58  ///< �������ò������ʱ
#define STREAM_CLIENT_SEND_HEARTBEAT_FAILED         59  ///< ������������ʧ��
#define STREAM_CLIENT_RECV_HEARTBEAT_TIMEOUT        60  ///< �����������ʱ
#define STREAM_CLIENT_PUSHDATA_TRANSMETHOD_INVALID  61  ///< �������䷽ʽ��Ч����֧��RTPUDP_TRANSMODE��
#define STREAM_CLIENT_VTDU_PERMISSION_LACK          62  ///< Ȩ�޲��㣨VTDU��
#define STREAM_CLIENT_ANALYZE_DATA_FAILED           63  ///< ֡����ʧ��
#define STREAM_CLIENT_VTM_PARSE_ERROR               64  ///< �����������VTM��
#define STREAM_CLIENT_VTM_EMPTY_CLUSTER             65  ///< ��Ⱥ����vtdu���ڣ�VTM��
#define STREAM_CLIENT_VTM_OVERLOAD_ERROR            66  ///< ��Ⱥ����ʱ��Ȩ�޲��㣨VTM��
#define STREAM_CLIENT_VTM_LACK_PERMISSION           67  ///< Ȩ�����ƣ�VTM��
#define STREAM_CLIENT_VTM_SERVER_INTERNAL           68  ///< �������ڲ�����VTM��

// ��Ϣ�ص�������(��ý��ͻ��˲���)
#define STREAM_CLIENT_SEND_HEARTBEAT_FAIL           4001  ///< ��������ʧ��
#define STREAM_CLIENT_HEARTBEAT_TIMEOUT             4002  ///< ������ʱ
#define STREAM_CLIENT_NOT_SUPPORT_PS_STREAM         4003  ///< ��֧��ת��װ��PS������ʶ����Ϣ���ݻص���ʹ��
#define STREAM_CLIENT_NO_TRANSFORM_PS_STREAM        4004  ///< �����Ѿ���PS��������ת��װ��ֱ�ӻص�ԭʼ��������Ϣ���ݻص���ʹ��

// ��Ϣ�ص�������(��ý�����������)
#define STREAM_CLIENT_ERR_FROM_SERVER               8000  ///< �ܵĴ����룬�����������������ж�
#define STREAM_CLIENT_CLICK                         8001  ///< ��Ȩ�޲��㣬���ߵ�
#define STREAM_CLIENT_LOCATION_FAILED               8002  ///< �طŶ�λʧ��

#define STREAM_CLIENT_DEVICE_NET_ERROR              8501    ///< ����
#define STREAM_CLIENT_DEVICE_NET_ERROR_PARAM        8502	///< ����Ĳ�������URL��ͨ�������ڵȣ�
#define STREAM_CLIENT_DEVICE_NET_ERROR_PASSWD       8503	///< �û����������
#define STREAM_CLIENT_DEVICE_NET_ERROR_CONNECT      8504	///< �豸�����ߣ������ӳ�ʱ

#define STREAM_CLIENT_VAG_STREAM_TIME_OUT			8601	//ȡ����ʱ
#define STREAM_CLIENT_VAG_STREAM_HEADER_NULL		8602	//û������ͷ
#define STREAM_CLIENT_VAG_START_STREAM_ERROR		8603	//vagȡ������ʧ��
#define STREAM_CLIENT_VAG_PARSE_URL					8604    //����urlʧ��                                                                                                        	//����URLʧ��
#define STREAM_CLIENT_VAG_LOGIN_VAG					8605	//��¼VAGʧ��
#define STREAM_CLIENT_VAG_QUERY_RES					8606	//��ѯVAG��Դ
#define STREAM_CLIENT_VAG_HIK_START_STREAM			8607	//�����豸ȡ��
#define STREAM_CLIENT_VAG_DAHUA_START_STREAM		8608	//���豸ȡ��
#define STREAM_CLIENT_VAG_INIT_DEV_CONNECTION		8609	//�����豸�������ӿ���ʧ��

// ��Ϣ�ص�������(VTM����)
#define STREAM_CLIENT_VTM_ERR						9000   ///< VTM����

#endif
