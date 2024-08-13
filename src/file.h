#ifndef __IEC_FILE_H__
#define __IEC_FILE_H__

#include <string>

class File
{
public:
    File();
    ~File();
public:
    void setName(const std::string &name);
    std::string getName() const;
    void setSize(const int64_t &size);
    int64_t getSize() const;
    void setLastModified(const time_t & lastModified);
    time_t getLastModified() const;
    void setLastModifiedUs(const suseconds_t &lastModifiedUs);
    suseconds_t getLastModifiedUs() const;
private:
    std::string _name;              //文件名
    int64_t _size;                  //文件大小
    time_t _lastModified;           //文件修改时间，秒的部分
    suseconds_t _lastModifiedUs;    //文件修改时间，微秒部分
};

#endif // !__IEC_FILE_H__