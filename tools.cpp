#include "tools.h"

/*
Elegxos an ena checkbox einai checked 'H oxi. To checkbox exei id to resid
*/
bool GetCheckBox(HWND dlg, int resid)
{
	if (SendMessage(GetDlgItem(dlg, resid), BM_GETCHECK, 0, 0) == BST_CHECKED)
		return true;
	else
		return false;
}

/*
Anoigei to dialogiko parathuro epikoinwnias me to xristi, gia na epileksei
ta kritiria analysis
*/
BOOL CALLBACK DialogProcess(HWND hwnd, UINT message, WPARAM wp, LPARAM lp)
{
	static InfoGetter* getter = 0;
	switch (message)
	{
	case WM_INITDIALOG:
		getter = (InfoGetter*)lp;
		return TRUE;
	case WM_COMMAND:
	{
		int ctl = LOWORD(wp);
		int event = HIWORD(wp);
		if (ctl == IDCANCEL && event == BN_CLICKED)
		{
			getter->got = false;
			DestroyWindow(hwnd);
			return TRUE;
		}
		else if (ctl == IDOK && event == BN_CLICKED)
		{
			getter->signal = GetCheckBox(hwnd, ID_SIGNAL);
			getter->ip = GetCheckBox(hwnd, ID_ADDRESS);
			getter->security = GetCheckBox(hwnd, ID_SECURITY);
			getter->speed = GetCheckBox(hwnd, ID_SPEED);
			getter->got = true;
			DestroyWindow(hwnd);
			return TRUE;
		}
		return FALSE;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return TRUE;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return TRUE;
	}
	return FALSE;
}

/*
get input from user
*/
void InfoGetter::Get()
{
	HWND dlg = CreateDialogParam(GetModuleHandle(0), MAKEINTRESOURCE(IDD_INPUT), 0, DialogProcess, (LPARAM)this);
	MSG msg;

	while (GetMessage(&msg, 0, 0, 0))
	{
		if (!IsDialogMessage(dlg, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

/*
convert a string to LPCWSTR string (wchar)
*/
LPCWSTR str2(string text)
{
	wchar_t *wtext = new wchar_t[text.size() + 1];
	mbstowcs(wtext, text.c_str(), text.size() + 1); //Plus null
	LPCWSTR ptr = wtext;
	return ptr;
}

/*
convert a char* to LPCWSTR string (wchar)
*/
LPCWSTR str2(char* text)
{
	int len = strlen(text);
	wchar_t *wtext = new wchar_t[len + 1];
	mbstowcs(wtext, text, len + 1); //Plus null
	LPCWSTR ptr = wtext;
	return ptr;
}

/*
convert double to string
*/
string d2s(double num)
{
	char str[50];
	sprintf(str, "%.2lf", num);
	string s = str;
	return s;
}

/*
convert int to string
*/
string i2s(int num)
{
	char str[50];
	sprintf(str, "%d", num);
	string s = str;
	return s;
}

/*
convert string to wstring
*/
void StringToWString(std::wstring &ws, const std::string &s)
{
	std::wstring wsTmp(s.begin(), s.end());
	ws = wsTmp;
}

/*
check if input is okay (at least one checkbox is checked)
*/
bool check_input(InfoGetter& input)
{
	if (!input.ip && !input.security && !input.signal && !input.speed)
		return false;
	input.got = 1;
	return true;
}
