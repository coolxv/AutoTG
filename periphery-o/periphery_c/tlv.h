#pragma once


#include <string>

namespace tlv
{

class Tlv
{
public:
    Tlv(int type);
    Tlv(int type, bool value);
    Tlv(int type, char value);
    Tlv(int type, short value);
    Tlv(int type, int value);
    Tlv(int type, long value);
    Tlv(int type, long long value);
    Tlv(int type, float value);
    Tlv(int type, double value);
    Tlv(int type, char* value);
    Tlv(int type, std::string value);
    Tlv(int type, unsigned char *value, int length);    
    Tlv(int type, const Tlv& value);
    ~Tlv();

public:
    int GetType() const;
    int GetLength() const;
    unsigned char *GetValue() const;

private:
    Tlv(const Tlv& c);
    Tlv &operator=(const Tlv &c);
    void Initialize(const void *value, int length);

private:
    int mType;
    int mLength;
    unsigned char *mValue;
};

}


