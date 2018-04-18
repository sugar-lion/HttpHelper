#ifndef __CURL_HTTP_H__
#define __CURL_HTTP_H__

#include <curl/curl.h>
#include <pthread.h>
#include "http_helper.h"

class ByteBuffer;
class HttpHelperImp
{
public:
    HttpHelperImp();
    ~HttpHelperImp();
    void SetUrl(const std::string& url);
    void SetCallBackInterface(HttpCallBackInterface &callback);
    void AddHeader(const std::string& key,const std::string& value);
    void AddHeaders(const std::map<std::string,std::string>& headers);
    void SetTimeOutMS(int timeoutMs);
    void Post(const unsigned char* postData,unsigned int postLen);

protected:
    static size_t CurlWriter(void *buffer, size_t size, size_t count, void * stream);
    static int CurlDebug(CURL *pcurl, curl_infotype itype, char * pData, size_t size, void *);
private:
    ByteBuffer* response_data_;
    std::string url_;
    std::map<std::string,std::string> headers_;
    int timeout_ms_;
    HttpCallBackInterface* callback_;
};


#endif
