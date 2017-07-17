#pragma once 
#ifndef __FFMPEG_SEEION_MANGER 
#define __FFMPEG_SEEION_MANGER 
typedef void(__stdcall *CallBackData)(unsigned char *buffer, int len);
typedef void(__stdcall *FileNameCallBack)(const char *fileName);

enum CODEID { }; 
enum PACKTYPE{ DO_NOTHING, MPEG2_TS, PS }; 
int __stdcall init_dll(); 
int __stdcall fini_dll(); 
//创建会话，返回值为会话句柄，输入参数为目标流格式，若不需要转流可以不输入该参数 
int __stdcall CreateSession(const CODEID id = (CODEID)-1); 
//创建会话，输入为会话句柄,若要删除所有句柄，输入参数为空 
int __stdcall DestorySession(const int sessionId = -1); 
//选择打包类型 
int __stdcall SetPackType(int sessionId, PACKTYPE type); 
//设置码流回调 
int __stdcall SetCallback(int sessionId, CallBackData callback);

int __stdcall SetFileNameCallback(int sessionId, FileNameCallBack callback);
//发送码流数据至指定会话 
int __stdcall SendData(int sessionId, unsigned char *data, int len); 
#endif