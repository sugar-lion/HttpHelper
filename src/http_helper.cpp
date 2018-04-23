#include "http_helper.h"
#include "http_helper_imp.inc"

HttpRequestInterface* HttpRequestFactory::CreateHttpRequest()
{
    return new HttpRequestImp();
}


HttpHelper::HttpHelper()
{
    instance_ = new HttpHelperImp();
}

HttpHelper::~HttpHelper()
{
    if (NULL != instance_)
    {
        delete instance_;
        instance_ = NULL;
    }
}


void HttpHelper::SetCallBackInterface(HttpCallBackInterface &callback)
{
    instance_->SetCallBackInterface(callback);
}

void HttpHelper::EasyPerform(const HttpRequestInterface& request)
{
    instance_->EasyPerform(request);
}

void HttpHelper::MultiPerform(const HttpRequestInterface& request)
{
    instance_->MultiPerform(request);
}



