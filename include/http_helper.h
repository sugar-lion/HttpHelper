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
    //virtual void onError(int errorCode,const char* errDesc) = 0;
};


class HttpHelperImp;
class HttpHelper
{
public:
    HttpHelper();
    virtual ~HttpHelper();
    virtual void SetUrl(const std::string& url);
    virtual void SetCallBackInterface(HttpCallBackInterface &callback);
    virtual void AddHeader(const std::string& key,const std::string& value);
    virtual void AddHeaders(const std::map<std::string,std::string>& headers);
    virtual void SetTimeOutMS(int timeoutMs);
    virtual void Post(const unsigned char* postData,unsigned int postLen);
protected:
    HttpHelperImp* instance_;
};


#endif
