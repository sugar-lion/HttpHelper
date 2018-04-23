#include "buffer_helper.inc"

ByteBuffer::ByteBuffer()
{
    buffer_ = NULL;
    buffer_len_ = 0;
    max_buffer_len_ = MAX_BUFFER_LEN_DEFAULT;
}


ByteBuffer::~ByteBuffer()
{
    if (buffer_ != NULL)
    {
        delete[] buffer_;
        buffer_ = NULL;
    }
    buffer_len_ = 0;
}

void ByteBuffer::Reset()
{
    if (buffer_ != NULL)
    {
        delete[] buffer_;
        buffer_ = NULL;
    }
    buffer_len_ = 0;
}


void ByteBuffer::AppendBuffer(const unsigned char* buffer,unsigned int size)
{
    if(NULL == buffer_)
    {
        buffer = new unsigned char[max_buffer_len_];
        buffer_len_ = 0;
    }
    if (buffer_len_ + size > max_buffer_len_)
    {
        return;
    }
    memcpy(buffer_ + buffer_len_,buffer,size);
}

const unsigned char* ByteBuffer::buffer()const
{
    return buffer_;
}

unsigned int ByteBuffer::buffer_len() const
{
    return buffer_len_;
}

void ByteBuffer::set_max_buffer_len(unsigned int max_len) 
{
    if(max_buffer_len_ < max_len)
    {
        Reset();
    }
    max_buffer_len_ = max_len;
}

