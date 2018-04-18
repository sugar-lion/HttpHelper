#include "http_helper.h"
#include "http_helper_imp.inc"

HttpHelper::HttpHelper()
{
    instance_ = new HttpHelperImp();
}

HttpHelper::~HttpHelper()
{
    if(NULL != instance_)
    {
        delete instance_;
        instance_ = NULL;
    }
}

void HttpHelper::SetUrl(const std::string& url)
{
    instance_->SetUrl(url);
}

void HttpHelper::SetCallBackInterface(HttpCallBackInterface &callback)
{
    instance_->SetCallBackInterface(callback);
}

void HttpHelper::AddHeader(const std::string& key,const std::string& value)
{
    instance_->AddHeader(key, value);
}

void HttpHelper::AddHeaders(const std::map<std::string,std::string>& headers)
{
    instance_->AddHeaders(headers);
}

void HttpHelper::SetTimeOutMS(int timeoutMs)
{
    instance_->SetTimeOutMS(timeoutMs);
}

void HttpHelper::Post(const unsigned char* postData,unsigned int postLen)
{
    instance_->Post(postData, postLen);
}


