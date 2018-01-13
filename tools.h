#ifndef TOOLS_H
#define TOOLS_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <windows.h>
#include "resource.h"

using namespace std;

struct InfoGetter
{
	bool signal, ip, security, speed;
	bool got;
	InfoGetter() : got(false) {}
	void Get();
};

bool check_input(InfoGetter&);
bool GetCheckbox(HWND dlg, int resid);
BOOL CALLBACK DialogProcess(HWND hwnd, UINT message, WPARAM wp, LPARAM lp);

LPCWSTR str2(string text);
LPCWSTR str2(char*);
string d2s(double num);
string i2s(int num);
void StringToWString(std::wstring &ws, const std::string &s);

#endif
