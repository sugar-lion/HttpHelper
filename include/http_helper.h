#ifndef __HTTP_HELPER_H__
#define __HTTP_HELPER_H__

#include <string>
#include <map>

class HttpCallBackInterface
{
public:
    virtual void onResponse(const unsigned char* response,unsigned int responseLen) = 0;
    virtual void onTimeOut() = 0;
    virtual void onConnectFailed() = 0;
};


class HttpRequestInterface
{
public:

    virtual void SetUrl(const std::string& url) = 0;
    virtual void AppendHeader(const std::string& key,const std::string& value) = 0;
    virtual void AppendHeaders(const std::map<std::string,std::string>& headers) = 0;
    virtual void SetTimeoutMs(int timeoutMs) = 0;
    virtual void SetPostData(const unsigned char* postData,unsigned int postLen) = 0;

    virtual const std::string& GetUrl() const = 0;
    virtual const std::map<std::string,std::string>& GetHeaders() const = 0;
    virtual int GetTimeoutMs() const = 0;
    virtual const unsigned char* GetPostData() const = 0;
    virtual unsigned int GetPostDataLen() const = 0;
};


class HttpRequestFactory
{
public:
    static HttpRequestInterface* CreateHttpRequest();
};




class HttpHelperImp;

class HttpHelper{
public:
    HttpHelper();
    ~HttpHelper();
    virtual void SetCallBackInterface(HttpCallBackInterface &callback);
    virtual void EasyPerform(const HttpRequestInterface& request); 
    virtual void MultiPerform(const HttpRequestInterface& request);
    virtual void Cancel();
protected:
    HttpHelperImp* instance_;
};

#endif
