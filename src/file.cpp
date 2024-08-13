#include "file.h"

File::File()
{
}

File::~File()
{
}

void File::setName(const std::string &name)
{
    if(_name != name)
    {
        _name = name;
    }
}

std::string File::getName() const
{
    return _name;
}

void File::setSize(const int64_t &size)
{
    if(_size != size)
    {
        _size = size;
    }
}

int64_t File::getSize() const
{
    return _size;
}

void File::setLastModified(const time_t &lastModified)
{
    if(_lastModified != lastModified)
    {
        _lastModified = lastModified;
    }
}

time_t File::getLastModified() const
{
    return _lastModified;
}

void File::setLastModifiedUs(const suseconds_t &lastModifiedUs)
{
    if(_lastModifiedUs != lastModifiedUs)
    {
        _lastModifiedUs = lastModifiedUs;
    }
}

suseconds_t File::getLastModifiedUs() const
{
    return _lastModifiedUs;
}