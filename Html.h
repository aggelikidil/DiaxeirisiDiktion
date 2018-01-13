#ifndef HTML_H
#define HTML_H

#include <iostream>
#include <string>
#include <fstream>
#include <locale>
#include <windows.h>
#include <codecvt>
#include "tools.h"

using namespace std;

class Article
{
private:
	wstring article_text;
public:
	Article(wstring image, wstring header, wstring text);
	wstring& getArticle() { return article_text; }
};

class Html
{
private:
	string real_filename;
	wstring not_analyzed;
	wstring signal, ip, security, speed;
	wstring first_part, end_part;
public:
	Html(string filename);
	void create_html_file();
	void set_signal(wstring& signal_article) { signal = signal_article; }
	void set_ip(wstring& ip_article) { ip = ip_article; }
	void set_security(wstring& security_article) { security = security_article; }
	void set_speed(wstring& speed_article) { speed = speed_article; }
};

#endif
