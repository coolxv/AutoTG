#pragma once
#include <windows.h>

struct ClientInfo
{
	std::string srv;
	std::string port;
	std::string user;
	std::string pwd;
	std::string uuid;
	std::string ver;
	std::string price;
};



int client_login(const std::string &srv, const std::string &port, const std::string &user, const std::string &pwd, const std::string &uuid, const std::string &ver, const std::string  &price);
int client_logout(const std::string &srv, const std::string &port, const std::string &user, const std::string &pwd, const std::string &uuid, const std::string &ver);

DWORD WINAPI client_thr(LPVOID lpParameter);

