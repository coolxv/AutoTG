#include "pch.h"
#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <zmq.hpp>
#include <msgpack.hpp>
#include <message.h>
#include "reg.h"
#include "client.h"

using namespace std;


template <typename T>
bool send_data(zmq::socket_t& socket, const string& tag, const T& data)
{
	msgpack::sbuffer packed;
	msgpack::pack(&packed, data);
	//tag
	zmq::message_t tag_msg(tag.size());
	std::memcpy(tag_msg.data(), tag.data(), tag.size());
	bool ret1 = socket.send(tag_msg, ZMQ_SNDMORE);
	//data
	zmq::message_t body_msg(packed.size());
	std::memcpy(body_msg.data(), packed.data(), packed.size());
	bool ret2 = socket.send(body_msg);

	return ret1 && ret2;

}

static bool recv_tag(zmq::socket_t& socket, string& tag)
{
	zmq::message_t tag_msg;
	bool ret = socket.recv(&tag_msg);
	//tag
	if (ret)
	{
		tag.assign(static_cast<const char*>(tag_msg.data()), tag_msg.size());
	}
	return ret;

}


template <typename T>
static bool recv_data(zmq::socket_t& socket, T& data)
{
	zmq::message_t body_msg;
	bool ret = socket.recv(&body_msg);
	//data
	if (ret)
	{
		msgpack::unpacked unpacked_body = msgpack::unpack(static_cast<const char*>(body_msg.data()), body_msg.size());
		msgpack::object deserialized = unpacked_body.get();
		deserialized.convert(data);
	}
	return ret;
}


string get_host_mac()
{
	bool ret = false;
	string mac;
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	PIP_ADAPTER_INFO pAdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL)
		return mac;
	// Make an initial call to GetAdaptersInfo to get the necessary size into the ulOutBufLen variable
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
		if (pAdapterInfo == NULL)
			return mac;
	}

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR)
	{
		for (PIP_ADAPTER_INFO pAdapter = pAdapterInfo; pAdapter != NULL; pAdapter = pAdapter->Next)
		{
			// 确保是以太网
			if (pAdapter->Type != MIB_IF_TYPE_ETHERNET)
				continue;
			// 确保MAC地址的长度为 00-00-00-00-00-00
			if (pAdapter->AddressLength != 6)
				continue;
			char acMAC[32];
			sprintf_s(acMAC, "%02X-%02X-%02X-%02X-%02X-%02X",
				int(pAdapter->Address[0]),
				int(pAdapter->Address[1]),
				int(pAdapter->Address[2]),
				int(pAdapter->Address[3]),
				int(pAdapter->Address[4]),
				int(pAdapter->Address[5]));
			mac += acMAC;
			break;
		}
	}

	free(pAdapterInfo);
	return mac;
}


string get_host_ip()
{

	//-----------------------------------------
	// Declare and initialize variables
	DWORD dwRetval;
	int nRet = 0;
	int iResult;
	WSADATA wsaData;
	struct addrinfo *result = NULL;
	struct addrinfo *ptr = NULL;
	struct addrinfo hints;
	string ip;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		return ip;
	}
	//--------------------------------
	// Setup the hints address info structure
	// which is passed to the getaddrinfo() function
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;


	//--------------------------------
	// Call getaddrinfo(). If the call succeeds,
	// the result variable will hold a linked list
	// of addrinfo structures containing response
	// information
	char hostname[256] = { 0 };
	dwRetval = getaddrinfo(hostname, NULL, &hints, &result);
	if (dwRetval != 0) {
		WSACleanup();
		return ip;
	}

	char IPdotdec[20];
	// Retrieve each address and print out the hex bytes
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		struct sockaddr_in  *sockaddr_ipv4;
		sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;
		inet_ntop(AF_INET, &sockaddr_ipv4->sin_addr, IPdotdec, 16);
		if (1 == strcmp(IPdotdec, "127.0.0.1"))
		{
			continue;
		}
		ip = IPdotdec;
		break;
	}

	freeaddrinfo(result);
	WSACleanup();
	return ip;

}
string get_host_ip_2()
{

	//-----------------------------------------
	// Declare and initialize variables
	WSADATA wsaData;
	int iResult;
	SOCKET sockfd;
	DWORD dwRetval;
	int nTry = 0;
	int i = 1;
	int nRet;
	unsigned long ulTime = 0;

	struct addrinfo *result = NULL;
	struct addrinfo *ptr = NULL;
	struct addrinfo hints;


	DWORD ipbufferlength = 46;


	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		//printf("WSAStartup failed: %d\n", iResult);
		return "";
	}

	//--------------------------------
	// Setup the hints address info structure
	// which is passed to the getaddrinfo() function
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;


	//--------------------------------
	// Call getaddrinfo(). If the call succeeds,
	// the result variable will hold a linked list
	// of addrinfo structures containing response
	// information

	dwRetval = getaddrinfo("time.nist.gov", "37", &hints, &result);
	if (dwRetval != 0) {
		//printf("getaddrinfo failed with error: %d\n", dwRetval);
		WSACleanup();
		return "";
	}


	// Retrieve each address and print out the hex bytes
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{

		sockfd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (INVALID_SOCKET == sockfd)
			continue;

		int TimeOut = 3000;//设置接收超时6秒
		setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&TimeOut, sizeof(TimeOut));

		do
		{
			if (5 == nTry++)
			{
				break;
			}

			nRet = connect(sockfd, ptr->ai_addr, ptr->ai_addrlen);
			if (SOCKET_ERROR == nRet)
			{
				continue;
			}

			nRet = recv(sockfd, (char *)&ulTime, sizeof(ulTime), 0);
			if ((SOCKET_ERROR != nRet) && (0 != nRet))
			{
				// getsockname
				struct sockaddr_in myaddr;
				int myaddr_len = sizeof(myaddr);
				char bind_ip[20] = { 0 };


				getsockname(sockfd, (struct sockaddr *)&myaddr, &myaddr_len);
				inet_ntop(AF_INET, &myaddr.sin_addr, bind_ip, sizeof(bind_ip));

				closesocket(sockfd);
				freeaddrinfo(result);
				WSACleanup();

				//SYSTEMTIME st = LongTime2StTime(ulTimehl);
				return bind_ip;
			}

		} while (1);

		closesocket(sockfd);
	}


	freeaddrinfo(result);
	WSACleanup();
	return "";



}

static bool client_login_req(const string &addr, const string &port, const string &user, const string &pwd, const string &uuid, const string &ver, const string  &price)
{
	//  Prepare our context and socket
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REQ);
	int sendtimeout = 5000;
	socket.setsockopt(ZMQ_SNDTIMEO, &sendtimeout, sizeof(sendtimeout));
	int recvtimeout = 5000;
	socket.setsockopt(ZMQ_RCVTIMEO, &recvtimeout, sizeof(recvtimeout));
	int lingertime = 0;
	socket.setsockopt(ZMQ_LINGER, &lingertime, sizeof(lingertime));

	string path = "tcp://" + addr + ":" + port;
	socket.connect(path.c_str());
	//send
	const std::string tag_req = "login";
	login_req_pk login_req;
	login_req.user = user;
	login_req.pwd = pwd;
	login_req.uuid = uuid;
	login_req.mn = getPcName();
	login_req.mc = genMachine();
	login_req.pri_ip = get_host_ip_2();
	//login_req.pub_ip = "";
	login_req.mac = get_host_mac();
	login_req.ver = ver;
	login_req.time = std::time(0);
	login_req.price = price;
	send_data(socket, tag_req, login_req);
	//recv
	string tag_rsp;
	recv_tag(socket, tag_rsp);
	login_rsp_pk login_rsp;
	bool ret = recv_data(socket, login_rsp);
	if (ret)
	{
		if (1 == login_rsp.err_code)
		{
			cout << login_rsp.err_msg << endl;
			ret = true;
		}
		else
		{
			cout << login_rsp.err_msg << endl;
			ret = false;
		}

	}

	return ret;
}


static bool client_logout_req(const string &addr, const string &port, const string &user, const string &pwd, const string &uuid, const string &ver)
{
	//  Prepare our context and socket
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REQ);
	int sendtimeout = 5000;
	socket.setsockopt(ZMQ_SNDTIMEO, &sendtimeout, sizeof(sendtimeout));
	int recvtimeout = 5000;
	socket.setsockopt(ZMQ_RCVTIMEO, &recvtimeout, sizeof(recvtimeout));
	int lingertime = 0;
	socket.setsockopt(ZMQ_LINGER, &lingertime, sizeof(lingertime));

	string path = "tcp://" + addr + ":" + port;
	socket.connect(path.c_str());
	//send
	const std::string tag_req = "logout";
	logout_req_pk logout_req;
	logout_req.user = user;
	logout_req.pwd = pwd;
	logout_req.uuid = uuid;
	logout_req.mn = getPcName();
	logout_req.mc = genMachine();
	logout_req.time = std::time(0);;
	send_data(socket, tag_req, logout_req);
	//recv
	string tag_rsp;
	recv_tag(socket, tag_rsp);
	logout_rsp_pk logout_rsp;
	bool ret = recv_data(socket, logout_rsp);
	if (ret)
	{
		if (1 == logout_rsp.err_code)
		{
			cout << logout_rsp.err_msg << endl;
			ret = true;
		}
		else
		{
			cout << logout_rsp.err_msg << endl;
			ret = false;
		}

	}

	return ret;
}

int client_login(const string &srv, const string &port, const string &user, const string &pwd, const string &uuid, const string &ver, const string  &price)
{

	//-----------------------------------------
	// Declare and initialize variables
	DWORD dwRetval;
	int nRet = 0;
	int iResult;
	WSADATA wsaData;
	struct addrinfo *result = NULL;
	struct addrinfo *ptr = NULL;
	struct addrinfo hints;
	

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		return 0;
	}
	//--------------------------------
	// Setup the hints address info structure
	// which is passed to the getaddrinfo() function
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;


	//--------------------------------
	// Call getaddrinfo(). If the call succeeds,
	// the result variable will hold a linked list
	// of addrinfo structures containing response
	// information
	if (srv.empty() || port.empty())
	{
		WSACleanup();
		return 0;
	}
	else
	{
		dwRetval = getaddrinfo(srv.c_str(), port.c_str(), &hints, &result);
	}
	if (dwRetval != 0) {
		WSACleanup();
		return 0;
	}


	// Retrieve each address and print out the hex bytes
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		struct sockaddr_in  *sockaddr_ipv4;
		sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;
		char IPdotdec[20];
		inet_ntop(AF_INET, &sockaddr_ipv4->sin_addr, IPdotdec, 16);
		bool ret = client_login_req(IPdotdec, port, user, pwd, uuid, ver, price);
		if (ret)
		{
			nRet = 1;
			break;
		}
	}

	freeaddrinfo(result);
	WSACleanup();
	return nRet;

}

int client_logout(const string &srv, const string &port, const string &user, const string &pwd, const string &uuid, const string &ver)
{

	//-----------------------------------------
	// Declare and initialize variables
	DWORD dwRetval;
	int nRet = 0;
	int iResult;
	WSADATA wsaData;
	struct addrinfo *result = NULL;
	struct addrinfo *ptr = NULL;
	struct addrinfo hints;


	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		return 0;
	}
	//--------------------------------
	// Setup the hints address info structure
	// which is passed to the getaddrinfo() function
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;


	//--------------------------------
	// Call getaddrinfo(). If the call succeeds,
	// the result variable will hold a linked list
	// of addrinfo structures containing response
	// information
	if (srv.empty() || port.empty())
	{
		WSACleanup();
		return 0;
	}
	else
	{
		dwRetval = getaddrinfo(srv.c_str(), port.c_str(), &hints, &result);
	}
	if (dwRetval != 0) {
		WSACleanup();
		return 0;
	}


	// Retrieve each address and print out the hex bytes
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		struct sockaddr_in  *sockaddr_ipv4;
		sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;
		char IPdotdec[20];
		inet_ntop(AF_INET, &sockaddr_ipv4->sin_addr, IPdotdec, 16);
		bool ret = client_logout_req(IPdotdec, port, user, pwd, uuid, ver);
		if (ret)
		{
			nRet = 1;
			break;
		}
	}

	freeaddrinfo(result);
	WSACleanup();
	return nRet;

}


 DWORD WINAPI client_thr(LPVOID lpParameter)
{
	 ClientInfo *client_info = (ClientInfo * )lpParameter;
	 int ret_login = 1;
	 while (1 == ret_login)
	 {
		 Sleep(600000);
		 ret_login = client_login(client_info->srv, client_info->port, client_info->user, client_info->pwd, client_info->uuid, client_info->ver, client_info->price);
	 }
	 return ret_login;
}

