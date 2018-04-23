#include "http_helper_imp.inc"
#include "buffer_helper.inc"
#include "guard.h"



static size_t CurlWriter(void *buffer, size_t size, size_t count, void * stream)
{
    ByteBuffer *pStream = static_cast<ByteBuffer*>(stream);
    pStream->AppendBuffer((const unsigned char *)buffer, size * count);
    return size * count;
};

static int CurlDebug(CURL *pcurl, curl_infotype itype, char * pData, size_t size, void *)
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


void HttpRequestImp::SetUrl(const std::string& url)
{
    url_ = url;
}
void HttpRequestImp::AppendHeader(const std::string& key,const std::string& value)
{
    headers_.insert(std::make_pair<std::string,std::string>(key,value));
}
void HttpRequestImp::AppendHeaders(const std::map<std::string,std::string>& headers)
{
    headers_.insert(headers.begin(),headers.end());
}
void HttpRequestImp::SetTimeoutMs(int timeoutMs)
{
    timeout_ms_ = timeoutMs;
}
void HttpRequestImp::SetPostData(const unsigned char* postData,unsigned int postLen)
{
    post_data_.set_max_buffer_len(postLen);
    post_data_.AppendBuffer(postData,postLen);
}

const std::string& HttpRequestImp::GetUrl()const
{
    return url_;
}

const std::map<std::string,std::string>& HttpRequestImp::GetHeaders()const
{
    return headers_;
}

int HttpRequestImp::GetTimeoutMs()const
{
    return timeout_ms_;
}

const unsigned char* HttpRequestImp::GetPostData()const
{
    return post_data_.buffer();
}

unsigned int HttpRequestImp::GetPostDataLen()const
{
    return post_data_.buffer_len();
}


CurlHandle::CurlHandle(const HttpRequestInterface& request)
{
    handle_ = curl_easy_init();
    curl_easy_setopt(handle_, CURLOPT_URL, request.GetUrl().c_str());
    curl_easy_setopt(handle_, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(handle_, CURLOPT_FORBID_REUSE, 1);
    curl_easy_setopt(handle_, CURLOPT_TIMEOUT_MS, 10000);
    curl_easy_setopt(handle_, CURLOPT_POST, 1); //设置问非0表示本次操作为post  
    curl_easy_setopt(handle_, CURLOPT_POSTFIELDS, request.GetPostData());
    curl_easy_setopt(handle_, CURLOPT_POSTFIELDSIZE, request.GetPostDataLen());
    curl_easy_setopt(handle_, CURLOPT_VERBOSE, 1); //打印调试信息
    curl_easy_setopt(handle_, CURLOPT_DEBUGFUNCTION, CurlDebug);
    curl_easy_setopt(handle_, CURLOPT_WRITEFUNCTION, CurlWriter);
    curl_easy_setopt(handle_, CURLOPT_WRITEDATA, &response_);
}


CURL* CurlHandle::GetEasyHandle()
{
    return handle_;
}

bool CurlHandle::IsAdded()
{
    return added_;
}
void CurlHandle::SetAddedTrue()
{
    added_ = true;
}

const unsigned char* CurlHandle::GetResponse()
{
    return response_.buffer();
}
unsigned int CurlHandle::GetResponseLen()
{
    return response_.buffer_len();
}



HttpHelperImp::HttpHelperImp()
{
    thread_ = new bgcc::Thread(run_func,this);
    cancel_ = false;
}
HttpHelperImp::~HttpHelperImp()
{
}

void HttpHelperImp::SetCallBackInterface(HttpCallBackInterface &callback)
{
    callback_ = &callback;
}

void HttpHelperImp::EasyPerform(const HttpRequestInterface& request)
{
    CURL*curl_handle = curl_easy_init();
    ByteBuffer response_data;
    curl_easy_setopt(curl_handle, CURLOPT_URL, request.GetUrl().c_str());
    curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1); 
    curl_easy_setopt(curl_handle, CURLOPT_DEBUGFUNCTION, CurlDebug);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, CurlWriter);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &response_data);
    curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl_handle, CURLOPT_FORBID_REUSE, 1);
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT_MS, request.GetTimeoutMs());
      
    curl_easy_setopt(curl_handle, CURLOPT_POST, 1); 
    curl_slist *curl_headers_list;
    std::map<std::string,std::string>::const_iterator itr = request.GetHeaders().begin();
    while(itr != request.GetHeaders().end())
    {
        curl_headers_list = curl_slist_append(curl_headers_list, std::string(itr->first + ":" + itr->second).c_str());
    }
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, curl_headers_list);  
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, request.GetPostData());
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, request.GetPostDataLen());
    CURLcode curlCode = curl_easy_perform(curl_handle);
    if (curlCode == CURLE_OK)
    {
        callback_->onResponse(response_data.buffer(),response_data.buffer_len());
    }
    else if (curlCode == CURLE_OPERATION_TIMEDOUT)
    {
        callback_->onTimeOut();
    }
    else{
        callback_->onConnectFailed();
    }
    curl_easy_cleanup(curl_handle);

}
void HttpHelperImp::MultiPerform(const HttpRequestInterface& request)
{
    CurlHandle* curlHandle = new CurlHandle(request);
    bgcc::Guard<bgcc::Mutex> guard(&handle_list_mutex_);
    handle_list_.push_back(curlHandle);
    if (handle_list_.size() == 1)
    {
        thread_->start();
    }
}



int HttpHelperImp::curlMultiPerform(CURLM * curl_m)
{
    int running_handles;
    while (CURLM_CALL_MULTI_PERFORM == curl_multi_perform(curl_m, &running_handles))
    {
        continue;
    }
    int ret = 0;
    struct timeval timeout_tv;
    fd_set  fd_read;
    fd_set  fd_write;
    fd_set  fd_except;
    int     max_fd = -1;

    FD_ZERO(&fd_read);
    FD_ZERO(&fd_write);
    FD_ZERO(&fd_except);

    timeout_tv.tv_sec = 1;
    timeout_tv.tv_usec = 0;

    curl_multi_fdset(curl_m, &fd_read, &fd_write, &fd_except, &max_fd);

    if (-1 == max_fd)
    {
        return -1;
    }

    int ret_code = ::select(max_fd + 1, &fd_read, &fd_write, &fd_except, &timeout_tv);
    switch(ret_code)
    {
    case -1:
        /* select error */
        ret = -1;
        break;
    case 0:
        /* select timeout */
    default:
        /* one or more of curl's file descriptors say there's data to read or write*/
        ret = 0;
        break;
    }

    return ret;
}

void HttpHelperImp::addEasyToMulti(CURLM * curl_m)
{
    bgcc::Guard<bgcc::Mutex> guard(&handle_list_mutex_);
    std::vector<CurlHandle*>::iterator itr = handle_list_.begin();
    while (itr != handle_list_.end())
    {
        if ((*itr)->IsAdded() == false)
        {
            curl_multi_add_handle(curl_m,(*itr)->GetEasyHandle());
            (*itr)->SetAddedTrue();
        }
        itr ++;
    }
}

void HttpHelperImp::clearEasyFromMulti(CURLM * curl_m)
{
    std::vector<CurlHandle*>::iterator itr = handle_list_.begin();
    while(itr != handle_list_.end())
    {
        curl_multi_remove_handle(curl_m, (*itr)->GetEasyHandle());
        curl_easy_cleanup((*itr)->GetEasyHandle());
        itr ++;
    }
}

CurlHandle* HttpHelperImp::getCurlHandleFromHandleList(CURL *easyHandle)
{
    bgcc::Guard<bgcc::Mutex> guard(&handle_list_mutex_);
    std::vector<CurlHandle*>::iterator itr = handle_list_.begin();
    while (itr != handle_list_.end())
    {
        if ((*itr)->GetEasyHandle() == easyHandle)
        {
            return *itr;
        }
        itr ++;
    }
    return NULL;
}

void HttpHelperImp::readInfoFromMulti(CURLM * curl_m)
{
    int msgs_left;
    CURLMsg*    msg = NULL;
    while ((msg =curl_multi_info_read(curl_m, &msgs_left))){
        if (CURLMSG_DONE == msg->msg)
        {
            CurlHandle* handle = getCurlHandleFromHandleList(msg->easy_handle);
            if (handle == NULL)
            {
                continue;
            }
            if (msg->data.result == CURLE_OK)
            {
                callback_->onResponse(handle->GetResponse(),handle->GetResponseLen());
            }
            else if (msg->data.result == CURLE_OPERATION_TIMEDOUT)
            {
                callback_->onTimeOut();
            }
            else {
                callback_->onConnectFailed();
            }
        }
    }
}

void HttpHelperImp::runFunc()
{
    CURLM * curl_m = curl_multi_init();
    addEasyToMulti(curl_m);
    while (!(cancel_))
    {
        addEasyToMulti(curl_m);
        curlMultiPerform(curl_m);
        readInfoFromMulti(curl_m);
    }
    clearEasyFromMulti(curl_m);
    curl_multi_cleanup(curl_m);
    return;
}


void HttpHelperImp::Cancel()
{
    cancel_ = true;
}



void* HttpHelperImp::run_func(void* param)
{
    HttpHelperImp *concurrenceHttp = (HttpHelperImp*)param;
    concurrenceHttp->runFunc();
    return NULL;
}





