#include "http_helper_imp.inc"
#include "buffer_helper.inc"

size_t HttpHelperImp::CurlWriter(void *buffer, size_t size, size_t count, void * stream)
{
    ByteBuffer *pStream = static_cast<ByteBuffer*>(stream);
    pStream->AppendBuffer((const unsigned char *)buffer, size * count);
    return size * count;
};

int HttpHelperImp::CurlDebug(CURL *pcurl, curl_infotype itype, char * pData, size_t size, void *)
{  
    if(itype == CURLINFO_TEXT)  
    {  
        printf("[TEXT]:%s",pData);  
    }  
    else if(itype == CURLINFO_HEADER_IN)  
    {  
        printf("[HEADER_IN]:%s",pData);  
    }  
    else if(itype == CURLINFO_HEADER_OUT)  
    {  
        printf("[HEADER_OUT]:%s",pData);  
    }  
    else if(itype == CURLINFO_DATA_IN)  
    {  
        printf("[DATA_IN]:%s",pData);  
    }  
    else if(itype == CURLINFO_DATA_OUT)  
    {  
        printf("[DATA_OUT]:%s",pData);  
    }  
    return 0;  
}  


HttpHelperImp::HttpHelperImp()
{
    response_data_ = new ByteBuffer();
}

HttpHelperImp::~HttpHelperImp()
{
    if(NULL != response_data_)
    {
        delete response_data_;
        response_data_ = NULL;
    }
}

void HttpHelperImp::SetUrl(const std::string& url)
{
    url_ = url;
}
void HttpHelperImp::SetCallBackInterface(HttpCallBackInterface &callback)
{
    callback_ = &callback;
}

void HttpHelperImp::AddHeader(const std::string& key,const std::string& value)
{
    headers_.insert(std::make_pair<std::string,std::string>(key,value));
}

void HttpHelperImp::AddHeaders(const std::map<std::string,std::string>& headers)
{
    headers_.insert(headers.begin(),headers.end());
}

void HttpHelperImp::SetTimeOutMS(int timeoutMs)
{
    timeout_ms_ = timeoutMs;
}

void HttpHelperImp::Post(const unsigned char* postData,unsigned int postLen)
{
    response_data_->Reset();
    
    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, url_);
    struct curl_slist *headers = NULL;
    std::map<std::string,std::string>::iterator itr = headers_.begin();
    while(itr != headers_.end())
    {
        headers = curl_slist_append(headers, std::string(itr->first + ":" + itr->second).c_str());
    }
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);        
    curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(handle, CURLOPT_FORBID_REUSE, 1);
    curl_easy_setopt(handle, CURLOPT_TIMEOUT_MS, timeout_ms_);
    curl_easy_setopt(handle, CURLOPT_POST, 1); 
    curl_easy_setopt(handle, CURLOPT_VERBOSE, 1); 
    curl_easy_setopt(handle, CURLOPT_DEBUGFUNCTION, HttpHelperImp::CurlDebug);
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, postData);
    curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, postLen);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, HttpHelperImp::CurlWriter);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, response_data_);
    CURLcode curlCode = curl_easy_perform(handle);
    curl_easy_cleanup(handle);
    if (curlCode == CURLE_OK)
    {
        callback_->onResponse(response_data_->buffer(),response_data_->buffer_len());
    }
    else if (curlCode == CURLE_OPERATION_TIMEDOUT)
    {
        callback_->onTimeOut();
    }
    else{
        callback_->onConnectFailed();
    }
    return;
}

