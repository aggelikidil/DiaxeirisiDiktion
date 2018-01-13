#ifndef _PROCESS_DATA_H_
#define _PROCESS_DATA_H_


#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <sqlext.h>
#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>
#include "Html.h"
#include "tools.h"

using namespace std;

class Article;

Article signal(void);
wstring signal_power(void);
wstring signal_detection(void);

Article ip(void);
wstring ip_addresses(void);

Article security(void);
wstring security_function(void);

Article speed(void);
wstring speed_function(void);

#endif
