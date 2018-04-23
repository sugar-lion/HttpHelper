#ifndef __CURL_HTTP_H__
#define __CURL_HTTP_H__

#include <curl/curl.h>
#include <vector>
#include "http_helper.h"
#include "buffer_helper.inc"
#include "mutex.h"
#include "thread.h"



class HttpRequestImp:public HttpRequestInterface
{
public:
    HttpRequestImp();
    ~HttpRequestImp();
    virtual void SetUrl(const std::string& url);
    virtual void AppendHeader(const std::string& key,const std::string& value);
    virtual void AppendHeaders(const std::map<std::string,std::string>& headers);
    virtual void SetTimeoutMs(int timeoutMs);
    virtual void SetPostData(const unsigned char* postData,unsigned int postLen);

    virtual const std::string& GetUrl()const ;
    virtual const std::map<std::string,std::string>& GetHeaders()const ;
    virtual int GetTimeoutMs()const ;
    virtual const unsigned char* GetPostData()const ;
    virtual unsigned int GetPostDataLen()const ;
private:
    std::string url_;
    std::map<std::string,std::string> headers_;
    int timeout_ms_;
    ByteBuffer post_data_;
};


class CurlHandle
{
public:
    CurlHandle(const HttpRequestInterface& request);
    ~CurlHandle();
    CURL* GetEasyHandle();
    bool IsAdded();
    void SetAddedTrue();
    const unsigned char* GetResponse();
    unsigned int GetResponseLen();
protected:
private:
    CURL* handle_;
    ByteBuffer response_;
    bool added_;
};

class HttpHelperImp
{
public:
    HttpHelperImp();
    ~HttpHelperImp();
    virtual void SetCallBackInterface(HttpCallBackInterface &callback);
    virtual void EasyPerform(const HttpRequestInterface& request); 
    virtual void MultiPerform(const HttpRequestInterface& request);
    virtual void Cancel();
protected:
    static int curlMultiPerform(CURLM * curl_m);
    static void* run_func(void* param);
    void runFunc();
    void addEasyToMulti(CURLM * curl_m);
    void readInfoFromMulti(CURLM * curl_m);
    void clearEasyFromMulti(CURLM * curl_m);
    CurlHandle* getCurlHandleFromHandleList(CURL *easy_handle);
private:
    std::vector<CurlHandle*> handle_list_;
    bgcc::Mutex handle_list_mutex_;
    bgcc::Thread *thread_;
    HttpCallBackInterface *callback_;
    bool cancel_;
};



#endif
