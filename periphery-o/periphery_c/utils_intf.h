#pragma once
#include <fflua.h>

class UtilsIntf
{
public:
	UtilsIntf();
	~UtilsIntf();
public:
	std::string md5(std::string fileName);
private:

};

void reg_utils_intf_lapi(ff::fflua_t *fflua);
