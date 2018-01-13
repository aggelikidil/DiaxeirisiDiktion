#include "Html.h"

Article::Article(wstring image, wstring header, wstring text)
{
	article_text = L"<article> <aside> <img src=\"images/" + image + L"\" width = \"100\" / >";
	article_text += L"</aside><section>";
	article_text += L"<h2>" + header + L"</h2>";
	article_text += text;
	article_text += L"</section></article>";
}

//Html implementation
Html::Html(string filename)
{
	real_filename = filename;
	not_analyzed = L"";
	signal = not_analyzed;
	ip = not_analyzed;
	security = not_analyzed;
	speed = not_analyzed;
	first_part = L"<!DOCTYPE html><html><head><meta charset=\"utf-8\"/><style type=\"text/css\">@import url('css/style.css');</style></head>\
		<body><header>Διαχείριση Δικτύων - Σελίδα αποτελεσμάτων</header><nav>Ονόματα ομάδας:<br/>Αγγελική Δηλάκη, Κωνσταντίνα Χατζή</nav>\
		<div id=\"container\"><div id=\"new_section\"><center><h2>Αποτελέσματα προγράμματος βελτιστοποίησης δικτύου</h2>\
		<img src=\"images/wifi.png\" width=\"150\"/></center>\
		<p>Στην συνέχεια παρουσιάζονται τα αποτελέσματα του προγράμματος βελτιστοποίησης δικτύου.<br/></p><br/></div>";
	end_part = L"</div><footer>www.di.uoa.gr © UoA</footer></body></html>";
}

void Html::create_html_file()
{
	//create and open html file as UTF-8 encode
	wofstream file;
	file.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::generate_header>));
	file.open(real_filename.c_str());
	if (file.bad() == true)
	{
		string error = "Could not create the HTML file. Program will now exit.";
		MessageBox(NULL, str2(error), L"Problem occured", MB_ICONEXCLAMATION | MB_OK);
		exit(0);
	}

	//write first part of html file
	file << first_part;

	//write results to html file
	file << signal << endl;
	file << ip << endl;
	file << security << endl;
	file << speed << endl;

	//write footer part of html file
	file << end_part;

	//close html file
	file.close();

	//open html file
	system(real_filename.c_str());
}