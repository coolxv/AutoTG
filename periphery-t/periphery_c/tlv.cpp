#include "pch.h"

#include <string.h>
#include "tlv.h"
 
namespace tlv
{

Tlv::Tlv(int type) : mType(type)
{
    Initialize(0, 0);
}

Tlv::Tlv(int type, bool value) : mType(type)
{
    Initialize(&value, sizeof(bool));
}

Tlv::Tlv(int type, char value) : mType(type)
{
    Initialize(&value, sizeof(char));
}

Tlv::Tlv(int type, short value) : mType(type)
{
    Initialize(&value, sizeof(short));
}

Tlv::Tlv(int type, int value) : mType(type)
{
    Initialize(&value, sizeof(int));
}

Tlv::Tlv(int type, long value) : mType(type)
{
    Initialize(&value, sizeof(long));
}

Tlv::Tlv(int type, long long value) : mType(type)
{
    Initialize(&value, sizeof(long long));
}

Tlv::Tlv(int type, float value) : mType(type)
{
    Initialize(&value, sizeof(float));
}

Tlv::Tlv(int type, double value) : mType(type)
{
    Initialize(&value, sizeof(double));
}

Tlv::Tlv(int type, char* value) : mType(type)
{
    Initialize(value, strlen(value)+1);
}

Tlv::Tlv(int type, std::string value) : mType(type)
{
    Initialize(value.c_str(),value.size() + 1);
}

Tlv::Tlv(int type, unsigned char *value, int length) : mType(type)
{
    Initialize(value, length);
}

Tlv::Tlv(int type, const Tlv& value) : mType(type)
{
    Initialize(value.GetValue(), value.GetLength());
}

Tlv::~Tlv()
{
    delete[] mValue;
}

void Tlv::Initialize(const void *value, int length)
{
    mLength = length;
    mValue = new unsigned char[length];
    memcpy(mValue, value, length);
}

int Tlv::GetType() const
{
    return mType;
}

int Tlv::GetLength() const
{
    return mLength;
}

unsigned char *Tlv::GetValue() const
{
    return mValue;
}

}
