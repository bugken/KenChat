#include "http.hpp"
#include <muduo/base/Logging.h>
#include <string>
#include <iostream>
#include <cstdio>
using namespace std;

//构造函数
HttpHandle::HttpHandle(){}
//获取唯一实例
HttpHandle* HttpHandle::instance()
{
    static HttpHandle httpHandle;
    return &httpHandle;
}
//服务端解析客户端发来的HTTP请求包包
bool HttpHandle::parseHttpMessage(string& httpBuff, string& jsonBuff)
{
	/*如果没有HTTP信息，那么就当做纯json数据*/
	if (0 != httpBuff.find("Content-Length: "))
	{
		jsonBuff = httpBuff;
	}
	
#if 0
	//该方法没有验证
	//string函数解析http包，没有校验
	int pos = httpBuff.find("\r\n\r\n");
	string tmp = httpBuff.substr(pos + 4);
#else
	//C语言方式解析http包，有长度校验
	const char* pdata = httpBuff.c_str();
	int buffLen = httpBuff.length();
	const char* pFind = NULL;
	pFind = strstr(pdata, "\r\n\r\n");
	if (pFind == NULL)
	{
		return false;
	}
	char * JsonStr = (char*)pFind + 4;
	//从消息头中获取消息长度
	char tempStr[] = "Content-Length: ";
	int tempLen = strlen(tempStr);
	pFind = strstr(pdata, tempStr);
	if (pFind == NULL)
	{
		return false;
	}
	pFind += tempLen;

	int concentLen = atoi(pFind);
	//JsonStr-pdata:结果是从
	if (buffLen != concentLen + (JsonStr - pdata))
	{
		unsigned int size2 = (unsigned int)concentLen + (unsigned int)(JsonStr - pdata);
		cerr << buffLen << " not equal size2 " << size2 << endl;
		return false;
	}

	string tmpBuff(JsonStr);
	jsonBuff = tmpBuff;
#endif
	return true;
}
//服务端构建HTTP回应包
bool HttpHandle::buildHttpResponse(const string& jsonBuff, string& httpBuff)
{
	//两种方法都已经验证通过
	#if 1
	//C++ string函数构建
	httpBuff.append(		
		"HTTP/1.1 200 OK\r\n"
		"cache-control: private\r\n"
		"content-type: text/plain; charset=utf-8\r\n"
		"server: microsoft-iis/7.5\r\n"
		"x-aspnet-version: 4.0.30319\r\n"
		"x-powered-by: asp.net\r\n"
		"date: wed, 19 nov 2014 10:55:41 gmt\r\n"
		"Content-Length: ").append(to_string(jsonBuff.length())).append("\r\n\r\n").append(jsonBuff);
	#else
	//C语言方式构建
	char httpresp[10 * 1024];
	int destLen = sizeof(httpresp);
	int srcLen = jsonBuff.length();
	const char* srcBuff = jsonBuff.c_str();
	snprintf(httpresp, destLen,
		"HTTP/1.1 200 OK\r\n"
		"cache-control: private\r\n"
		"content-type: text/plain; charset=utf-8\r\n"
		"server: microsoft-iis/7.5\r\n"
		"x-aspnet-version: 4.0.30319\r\n"
		"x-powered-by: asp.net\r\n"
		"date: wed, 19 nov 2014 10:55:41 gmt\r\n"
		"Content-Length: %d\r\n\r\n"
		"%s", srcLen, srcBuff);

	string tmpBuff(httpresp);
	httpBuff = tmpBuff;
	#endif
    return true;
}
//客户端构建HTTP请求包
bool HttpHandle::buildHttpResquest(const string& jsonBuff, string& httpBuff)
{
	#if 1
	//C++ string函数构建
	httpBuff.append(		
		"POST / HTTP/1.1\r\n")
		.append("Content-Length: ").append(to_string(jsonBuff.length()))
		.append("content-type: text/plain; charset=utf-8\r\n"
		"Connection: Keep-Alive\r\n"
		"User-Agent: Apache-HttpClient/4.5.2 (Java/1.8.0_221)\r\n"
		"Accept-Encoding: gzip,deflate")
		.append("\r\n\r\n").append(jsonBuff);
	#else
	//C语言方式构建
	char httpresp[10 * 1024];
	int destLen = sizeof(httpresp);
	int srcLen = jsonBuff.length();
	const char* srcBuff = jsonBuff.c_str();
	snprintf(httpresp, destLen,
		"POST / HTTP/1.1\r\n"
		"Content-Length: %d\r\n"
		"content-type: text/plain; charset=utf-8\r\n"
		"Host: 192.168.107.128:8000\r\n"
		"Connection: Keep-Alive\r\n"
		"User-Agent: Apache-HttpClient/4.5.2 (Java/1.8.0_221)\r\n"
		"Accept-Encoding: gzip,deflate\r\n\r\n"
		"%s", srcLen, srcBuff);

	string tmpBuff(httpresp);
	httpBuff = tmpBuff;
	#endif
    return true;
}
