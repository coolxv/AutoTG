#pragma once
#include <string>

std::string getPcName();

std::string  genCode();
std::string  genMachine();
std::string  getCodeFromMachine(std::string machine);
int32_t  setMachineToFile(const std::string &fileName);

int32_t  setRegCodeToFile(const std::string &fileName, const std::string &key, const std::string &machine, const std::string &srv, const std::string &port, const std::string &genDate, const std::string &validDate, int32_t version, int32_t type);
int32_t  getRegCodeFromFile(const std::string &fileName, const std::string &key, std::string &machine, std::string &srv, std::string &port, std::string &genDate, std::string &validDate, int32_t &version, int32_t &type);

void generateRSAKey(std::string strKey[2]);
void setRSAKeyToFile();


int file_encrypt2file(const char* inFileName, const char* outFileName);
int file_decrypt2buf(const char* inFileName, char** outbuf, int *len);

int file_MD5toFile(const char* inFileName, const char* outFileName);
int file_MD5toString(const char* inFileName, std::string &encodedStr);
