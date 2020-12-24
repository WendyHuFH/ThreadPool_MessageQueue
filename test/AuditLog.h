#pragma once

#ifndef _AUDIT_LOG_H
#define _AUDIT_LOG_H
#include <iostream>
#include <fstream>
#include <time.h>
#include <sstream>
#include <string>
#include <assert.h>
using namespace std;

class AuditLog
{
public:
	AuditLog()
	{
		tbuf = "";
		st = nullptr;
		ss.clear();
	}

	~AuditLog(void)
	{
	}
	string log_name;
	ofstream logfile;
	const string GetSysTime()
	{
		ss.clear(); ss.str("");
		st = GetSysTime_c();
		tbuf = "";
		tbuf = NumToString(st->tm_year + 1900) + NumToString(st->tm_mon + 1) + NumToString(st->tm_mday) + NumToString(st->tm_hour) + NumToString(st->tm_min) + NumToString(st->tm_sec) + ".txt";
		ss << tbuf;
		return ss.str();
	}
	struct tm* GetSysTime_c()
	{
		time(&now);
		return localtime(&now);
	}
	const string CharToString(const char *str)
	{
		ss.clear(); ss.str("");
		ss << str;
		return ss.str();	
	}
	const string NumToString(int num)
	{
		ss.clear(); ss.str("");
		ss << num;
		return ss.str();
	}
private:
	string tbuf;
	struct tm* st;
	time_t now;
	stringstream ss;
};

#endif