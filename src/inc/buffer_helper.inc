#ifndef __BUFFER_HELPER_H__
#define __BUFFER_HELPER_H__
#include <string.h>

class ByteBuffer
{
public:
    ByteBuffer();
    ~ByteBuffer();
    void AppendBuffer(const unsigned char* buffer,unsigned int size);
    void Reset();
    const unsigned char* buffer() const;
    unsigned int buffer_len() const;
    void set_max_buffer_len(unsigned int max_len) ;
private:
    unsigned char *buffer_;
    int buffer_len_;
    int max_buffer_len_;
    static const int MAX_BUFFER_LEN_DEFAULT = 1024*1024;
};


#endif