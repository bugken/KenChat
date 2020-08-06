#ifndef HTTP_HPP
#define HTTP_HPP
#include <iostream>

using namespace std;

//处理Http包
class HttpHandle
{
public:
    //获取唯一实例
    static HttpHandle *instance();
    //解析HTTP包
    bool parseHttpMessage(string& httpBuff, string& jsonBuff);
    //构建HTTP服务端响应包
    bool buildHttpResponse(const string& jsonBuff, string& httpBuff);
    //构建HTTP客户端端请求包
    bool buildHttpResquest(const string& jsonBuff, string& httpBuff);
private:
	HttpHandle(const HttpHandle &);
    HttpHandle();
	HttpHandle& operator = (const HttpHandle &);
};

#endif

