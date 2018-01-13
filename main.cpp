#include <iostream>
#include <string>
#include <cstdlib>
#include "process_data.h"

using namespace std;

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
	InfoGetter input;
	string filename = "results.html";
	Html html_file(filename);

	/*
	O xristis epilegei ta kritiria pou thelei na ginei i aksiologisi tou diktuou. Oi epiloges einai:
	- Signal
	- IP Addresses
	- Security
	- Speed
	*/

	while(1)
	{
		input.Get();
		if (check_input(input) == false)
			MessageBox(NULL, L"Παρακαλώ επιλέξτε τουλάχιστον ένα κριτήριο ανάλυσης.", L"Λάθος είσοδος", MB_ICONEXCLAMATION | MB_OK);
		else
			break;
	}
	
	if (input.signal)
	{
		Article signal_article = signal();
		html_file.set_signal(signal_article.getArticle());
	}

	if (input.ip)
	{
		Article ip_article = ip();
		html_file.set_ip(ip_article.getArticle());
	}

	if (input.security)
	{
		Article security_article = security();
		html_file.set_security(security_article.getArticle());
	}
	
	if (input.speed)
	{
		Article speed_article = speed();
		html_file.set_speed(speed_article.getArticle());
	}	

	html_file.create_html_file();

	return 0;
}


