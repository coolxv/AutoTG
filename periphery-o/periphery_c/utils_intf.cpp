#include "pch.h"
#include <windows.h>
#include "utils_intf.h"
#include "reg.h"
using namespace std;
using namespace ff;


static const char *reg_utilsFunc =
"function dumpTable(tb,str)\n"
"    if type(tb) ~= \"table\" then return end\n"
"    if nil == str then str = \"\" end\n"
"    print(\"\\n\")\n"
"    for k,v in pairs(tb)\n"
"    do\n"
"        if type(v) ==\"table\"\n"
"        then\n"
"            dumpTable(v,str)\n"
"        else\n"
"            print(str,k,v)\n"
"        end\n"
"    end\n"
"    print(\"\\n\")\n"
"end\n";





UtilsIntf::UtilsIntf()
{

}
UtilsIntf::~UtilsIntf()
{

}


string UtilsIntf::md5(string fileName)
{
	string md5str;
	file_MD5toString(fileName.c_str(), md5str);
	return md5str;

}

void winSleep(unsigned int milliSeconds)
{
	Sleep(milliSeconds);
	return;
}

void reg_utils_intf_lapi(fflua_t *fflua)
{
	fflua->run_string(reg_utilsFunc);
	fflua_register_t<>(fflua->get_lua_state()).def(&winSleep, "winSleep");                //! 注册静态函数  
	//! 注册基类函数, ctor() 为构造函数的类型  
	fflua_register_t<UtilsIntf, ctor()>(fflua->get_lua_state(), "UtilsIntf")
		.def(&UtilsIntf::md5, "md5");
}