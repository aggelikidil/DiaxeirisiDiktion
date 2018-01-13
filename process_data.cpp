#include "process_data.h"

using namespace std;

const char* DAM = "Direct ODBC";

//Kritiria analysis

Article signal(void)
{
	wstring text = signal_power() + signal_detection();
	Article ar(L"signal.png", L"Signal - Σηματοδοσία", text);
	return ar;
}

Article ip(void)
{
	wstring text = ip_addresses();
	Article ar(L"ip.png", L"IP Addresses - Διευθύνσεις ΙΡ", text);
	return ar;
}

Article security(void)
{
	wstring text = security_function();
	Article ar(L"security.png", L"Security - Ασφάλεια δικτύου", text);
	return ar;
}

Article speed(void)
{
	wstring text = speed_function();
	Article ar(L"speed.png", L"Speed - Ταχύτητα", text);
	return ar;
}

wstring signal_power(void)
{
	HENV    hEnv;
    HDBC    hDbc;
    SQLRETURN  rc;
	wstring output;
	
	/* Number of rows in result set */
    SQLINTEGER      rowCount = 0;
	HSTMT           hStmt;

	/* Allocate an environment handle */
    rc = SQLAllocEnv(&hEnv);
	   
	/* Allocate a connection handle */
    rc = SQLAllocConnect(hEnv, &hDbc);
		
	/* Connect to the 'Fileblocker.accdb' database */
    rc = SQLConnect(hDbc, L"database",  SQL_NTS, NULL, 0, NULL, 0);

	if (SQL_SUCCEEDED(rc)) 
    {
        /* Prepare SQL query */
        rc = SQLAllocStmt(hDbc,&hStmt);
		string query = "SELECT SSID, BSSID, AVG(Signal) AS Average FROM XIRRUS WHERE Connected = 'True' GROUP BY SSID, BSSID;";
        rc = SQLPrepare(hStmt, (SQLWCHAR*)str2(query), SQL_NTS);
       
       	/* Execute the query and create a record set */
        rc = SQLExecute(hStmt); 
        
		output = L"<p>Συνδεδεμένα δίκτυα:</p><table><tr><th>Όνομα δικτύου</th><th>Διεύθυνση MAC</th><th>Μέση ισχύς λήψης</th></tr>";
		bool good_result = true;

		/* Loop through the rows in the result set */
        for (rc = SQLFetch(hStmt); rc == SQL_SUCCESS; rc=SQLFetch(hStmt)) 
		{
			unsigned char szData[600]; // Returned data storage
			SDWORD cbData; // Output length of data

			SQLGetData(hStmt, 1, SQL_C_CHAR, szData, sizeof(szData), &cbData);
			string ssid = (const char*)szData;
			SQLGetData(hStmt, 2, SQL_C_CHAR, szData, sizeof(szData), &cbData);
			string bssid = (const char*)szData;
			SQLGetData(hStmt, 3, SQL_C_CHAR, szData, sizeof(szData), &cbData);
			double average = atof((const char*)szData);
			string avg_str = d2s(average);

			wstring w_ssid, w_bssid, w_avg;
			StringToWString(w_ssid, ssid);
			StringToWString(w_bssid, bssid);
			StringToWString(w_avg, avg_str);
			if (average < -45)
				good_result = false;

			output += L"<tr><td>" + w_ssid + L"</td><td>" + w_bssid + L"</td><td>" + w_avg + L" dBm</td></tr>";
			rowCount++;
		}
		output += L"</table>";
		if (good_result)
			output += L"<p><h5>Αποτέλεσμα: δεν χρειάζεται βελτιστοποίηση</h5></p>";
		else
			output += L"<p><h4>Αποτέλεσμα: χαμηλή ισχύς λήψης, ελέγξτε την τοποθεσία της κεραίας</h4></p>";
		rc = SQLFreeStmt(hStmt, SQL_DROP);
        if (rowCount == 0)
		{
			string error = "Database is empty. Program will exit.";
			MessageBox(NULL, str2(error), L"Error in signal analysis occured", MB_ICONEXCLAMATION | MB_OK);
			exit(0);
		}
    }
    else
    {
        string error = "Couldn't connect to database.";
		MessageBox(NULL, str2(error), L"Problem occured", MB_ICONEXCLAMATION | MB_OK);
		exit(0);
    }

    /* Disconnect and free up allocated handles */
    rc = SQLFreeStmt(hStmt, SQL_DROP);
	SQLDisconnect(hDbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	SQLFreeHandle(SQL_HANDLE_ENV, hEnv);	
	return output;
}

wstring signal_detection(void)
{
	HENV    hEnv;
	HDBC    hDbc;
	SQLRETURN  rc;
	wstring output;

	/* Number of rows in result set */
	SQLINTEGER      rowCount = 0;
	HSTMT           hStmt;

	/* Allocate an environment handle */
	rc = SQLAllocEnv(&hEnv);

	/* Allocate a connection handle */
	rc = SQLAllocConnect(hEnv, &hDbc);

	/* Connect to the 'Fileblocker.accdb' database */
	rc = SQLConnect(hDbc, L"database", SQL_NTS, NULL, 0, NULL, 0);

	if (SQL_SUCCEEDED(rc))
	{
		/* Prepare SQL query */
		rc = SQLAllocStmt(hDbc, &hStmt);
		string query = "SELECT SSID, MAC_ADDRESS, MAX(AVERAGE_SIGNAL) AS ['Average Signal %'], SUM(DETECTION_COUNTER) AS ['Detected Count'], FREQUENCY \
						FROM WIRELESS \
						WHERE AVERAGE_SIGNAL >= 0.8 AND DETECTION_COUNTER > 10 \
						GROUP BY SSID, MAC_ADDRESS, FREQUENCY ORDER BY MAX(AVERAGE_SIGNAL) DESC;";
		rc = SQLPrepare(hStmt, (SQLWCHAR*)str2(query), SQL_NTS);

		/* Execute the query and create a record set */
		rc = SQLExecute(hStmt);

		output = L"<p>Εντοπισμένα δίκτυα:</p><table><tr><th>Όνομα δικτύου</th><th>Διεύθυνση MAC</th><th>Συχνότητα εμφάνισης</th><th>Πλήθος εμφανίσεων</th><th>Συχνότητα λειτουργίας</th></tr>";
		bool good_result = true;
		int home_frequency = 0;

		/* Loop through the rows in the result set */
		for (rc = SQLFetch(hStmt); rc == SQL_SUCCESS; rc = SQLFetch(hStmt))
		{
			unsigned char szData[600]; // Returned data storage
			SDWORD cbData; // Output length of data

			SQLGetData(hStmt, 1, SQL_C_CHAR, szData, sizeof(szData), &cbData);
			string ssid = (const char*)szData;
			SQLGetData(hStmt, 2, SQL_C_CHAR, szData, sizeof(szData), &cbData);
			string bssid = (const char*)szData;
			SQLGetData(hStmt, 3, SQL_C_CHAR, szData, sizeof(szData), &cbData);
			double shown = atof((const char*)szData);
			char temp[10]; sprintf(temp, "%.2lf", shown * 100);
			string shown_str = temp;
			SQLGetData(hStmt, 4, SQL_C_CHAR, szData, sizeof(szData), &cbData);
			int counter = atoi((const char*)szData);
			string counter_str = i2s(counter);
			SQLGetData(hStmt, 5, SQL_C_CHAR, szData, sizeof(szData), &cbData);
			int freq = atoi((const char*)szData);
			string freq_str = i2s(freq);
			
			wstring w_ssid, w_bssid, w_shown, w_counter, w_freq;
			StringToWString(w_ssid, ssid);
			StringToWString(w_bssid, bssid);
			StringToWString(w_shown, shown_str);
			StringToWString(w_counter, counter_str);
			StringToWString(w_freq, freq_str);
			
			if (rowCount == 1)
				home_frequency = freq;
			else
				if (home_frequency == freq)
					good_result = false;

			output += L"<tr><td>" + w_ssid + L"</td><td>" + w_bssid + L"</td><td>" + w_shown + L"%</td><td>" + w_counter + L"</td><td>" + w_freq + L" </td></tr>";
			rowCount++;
		}
		output += L"</table>";
		if (good_result)
			output += L"<p><h5>Αποτέλεσμα: δεν χρειάζεται βελτιστοποίηση</h5></p>";
		else
			output += L"<p><h4>Αποτέλεσμα: εντοπίστηκαν παρεμβολές, ελέγξτε τη συχνότητα λειτουργίας</h4></p>";
		rc = SQLFreeStmt(hStmt, SQL_DROP);
		if (rowCount == 0)
		{
			string error = "Database is empty. Program will exit.";
			MessageBox(NULL, str2(error), L"Error in signal analysis occured", MB_ICONEXCLAMATION | MB_OK);
			exit(0);
		}
	}
	else
	{
		string error = "Couldn't connect to database.";
		MessageBox(NULL, str2(error), L"Problem occured", MB_ICONEXCLAMATION | MB_OK);
		exit(0);
	}

	/* Disconnect and free up allocated handles */
	rc = SQLFreeStmt(hStmt, SQL_DROP);
	SQLDisconnect(hDbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
	return output;
}

wstring ip_addresses(void)
{
	HENV    hEnv;
	HDBC    hDbc;
	SQLRETURN  rc;
	wstring output;

	/* Number of rows in result set */
	SQLINTEGER      rowCount = 0;
	HSTMT           hStmt;

	/* Allocate an environment handle */
	rc = SQLAllocEnv(&hEnv);

	/* Allocate a connection handle */
	rc = SQLAllocConnect(hEnv, &hDbc);

	/* Connect to the 'Fileblocker.accdb' database */
	rc = SQLConnect(hDbc, L"database", SQL_NTS, NULL, 0, NULL, 0);

	if (SQL_SUCCEEDED(rc))
	{
		/* Prepare SQL query */
		rc = SQLAllocStmt(hDbc, &hStmt);
		string query = "SELECT IP, MAC_ADDRESS \
						FROM IPSCANNER \
						WHERE STATUS<>'Unknown' \
						GROUP BY IP, MAC_ADDRESS \
						ORDER BY IP; ";
		rc = SQLPrepare(hStmt, (SQLWCHAR*)str2(query), SQL_NTS);

		/* Execute the query and create a record set */
		rc = SQLExecute(hStmt);

		output = L"<p>Βρέθηκαν οι παρακάτω συσκευές:</p><table><tr><th>Διεύθυνση ΙΡ</th><th>Διεύθυνση MAC</th></tr>";
		bool good_result = true;
		
		/* Loop through the rows in the result set */
		for (rc = SQLFetch(hStmt); rc == SQL_SUCCESS; rc = SQLFetch(hStmt))
		{
			unsigned char szData[600]; // Returned data storage
			SDWORD cbData; // Output length of data

			SQLGetData(hStmt, 1, SQL_C_CHAR, szData, sizeof(szData), &cbData);
			string ip = (const char*)szData;
			SQLGetData(hStmt, 2, SQL_C_CHAR, szData, sizeof(szData), &cbData);
			string mac = (const char*)szData;
			
			wstring w_ip, w_mac;
			StringToWString(w_ip, ip);
			StringToWString(w_mac, mac);
			
			output += L"<tr><td>" + w_ip + L"</td><td>" + w_mac + L" </td></tr>";
			rowCount++;
		}
		wstring w_total;
		StringToWString(w_total, i2s((int)rowCount));
		output += L"</table> <p> Βρέθηκαν " + w_total + L" διαφορετικές ΙΡ διευθύνσεις </p>";
		if (rowCount > 200)
			good_result = false;
		if (good_result)
			output += L"<p><h5>Αποτέλεσμα: δεν χρειάζεται βελτιστοποίηση</h5></p>";
		else
			output += L"<p><h4>Αποτέλεσμα: το πλήθος των συσκευών στο δίκτυο είναι μεγάλο, αλλάξτε τη μάσκα υποδικτύου</h4></p>";
		rc = SQLFreeStmt(hStmt, SQL_DROP);
		if (rowCount == 0)
		{
			string error = "Database is empty. Program will exit.";
			MessageBox(NULL, str2(error), L"Error in signal analysis occured", MB_ICONEXCLAMATION | MB_OK);
			exit(0);
		}
	}
	else
	{
		string error = "Couldn't connect to database.";
		MessageBox(NULL, str2(error), L"Problem occured", MB_ICONEXCLAMATION | MB_OK);
		exit(0);
	}

	/* Disconnect and free up allocated handles */
	rc = SQLFreeStmt(hStmt, SQL_DROP);
	SQLDisconnect(hDbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
	return output;
}

wstring security_function(void)
{
	HENV    hEnv;
	HDBC    hDbc;
	SQLRETURN  rc;
	wstring output;

	/* Number of rows in result set */
	SQLINTEGER      rowCount = 0;
	HSTMT           hStmt;

	/* Allocate an environment handle */
	rc = SQLAllocEnv(&hEnv);

	/* Allocate a connection handle */
	rc = SQLAllocConnect(hEnv, &hDbc);

	/* Connect to the 'Fileblocker.accdb' database */
	rc = SQLConnect(hDbc, L"database", SQL_NTS, NULL, 0, NULL, 0);

	if (SQL_SUCCEEDED(rc))
	{
		/* Prepare SQL query */
		rc = SQLAllocStmt(hDbc, &hStmt);
		string query = "SELECT SSID, BSSID, ENCRYPTION, AUTH \
						FROM XIRRUS \
						WHERE Connected = 'True' \
						GROUP BY SSID, BSSID, ENCRYPTION, AUTH;";
		rc = SQLPrepare(hStmt, (SQLWCHAR*)str2(query), SQL_NTS);

		/* Execute the query and create a record set */
		rc = SQLExecute(hStmt);

		output = L"<p>Πληροφορίες ασφάλειας δικτύου:</p><table><tr><th>Είδος κρυπτογράφησης</th></tr>";
		bool good_result = true;

		/* Loop through the rows in the result set */
		for (rc = SQLFetch(hStmt); rc == SQL_SUCCESS; rc = SQLFetch(hStmt))
		{
			unsigned char szData[600]; // Returned data storage
			SDWORD cbData; // Output length of data

			SQLGetData(hStmt, 3, SQL_C_CHAR, szData, sizeof(szData), &cbData);
			string encryption = (const char*)szData;
			SQLGetData(hStmt, 4, SQL_C_CHAR, szData, sizeof(szData), &cbData);
			string auth = (const char*)szData;

			wstring w_enc, w_auth;
			StringToWString(w_enc, encryption);
			StringToWString(w_auth, auth);
			output += L"<tr><td>" + w_enc + L"</td></tr>";
			output += L"<tr><th>Πιστοποίηση</th></tr><tr><td>" + w_auth +L"</td></tr></table>";
			rowCount++;
			if (rowCount > 1)
				good_result = false;
			if (encryption == "None")
				good_result = false;
		}
		
		if (good_result)
			output += L"<p><h5>Αποτέλεσμα: δεν χρειάζεται βελτιστοποίηση</h5></p>";
		else
			output += L"<p><h4>Αποτέλεσμα: η ασφάλεια του δικτύου είναι ελλιπής</h4></p>";
		rc = SQLFreeStmt(hStmt, SQL_DROP);
		if (rowCount == 0)
		{
			string error = "Database is empty. Program will exit.";
			MessageBox(NULL, str2(error), L"Error in signal analysis occured", MB_ICONEXCLAMATION | MB_OK);
			exit(0);
		}
	}
	else
	{
		string error = "Couldn't connect to database.";
		MessageBox(NULL, str2(error), L"Problem occured", MB_ICONEXCLAMATION | MB_OK);
		exit(0);
	}

	/* Disconnect and free up allocated handles */
	rc = SQLFreeStmt(hStmt, SQL_DROP);
	SQLDisconnect(hDbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
	return output;
}

wstring speed_function(void)
{
	HENV    hEnv;
	HDBC    hDbc;
	SQLRETURN  rc;
	wstring output;

	/* Number of rows in result set */
	SQLINTEGER      rowCount = 0;
	HSTMT           hStmt;

	/* Allocate an environment handle */
	rc = SQLAllocEnv(&hEnv);

	/* Allocate a connection handle */
	rc = SQLAllocConnect(hEnv, &hDbc);

	/* Connect to the 'Fileblocker.accdb' database */
	rc = SQLConnect(hDbc, L"database", SQL_NTS, NULL, 0, NULL, 0);

	if (SQL_SUCCEEDED(rc))
	{
		/* Prepare SQL query */
		rc = SQLAllocStmt(hDbc, &hStmt);
		string query = "SELECT PHY_TYPES, MAX_SPEED \
						FROM WIRELESS \
						WHERE SSID = (SELECT DISTINCT SSID FROM XIRRUS WHERE Connected = 'True') \
						GROUP BY PHY_TYPES, MAX_SPEED;";
		rc = SQLPrepare(hStmt, (SQLWCHAR*)str2(query), SQL_NTS);

		/* Execute the query and create a record set */
		rc = SQLExecute(hStmt);

		output = L"<p>Πληροφορίες για την ταχύτητα του δικτύου:</p><table><tr><th>Πρωτόκολλο πρόσβασης</th></tr>";
		bool good_result = true;

		/* Loop through the rows in the result set */
		for (rc = SQLFetch(hStmt); rc == SQL_SUCCESS; rc = SQLFetch(hStmt))
		{
			unsigned char szData[600]; // Returned data storage
			SDWORD cbData; // Output length of data

			SQLGetData(hStmt, 1, SQL_C_CHAR, szData, sizeof(szData), &cbData);
			string prot = (const char*)szData;
			SQLGetData(hStmt, 2, SQL_C_CHAR, szData, sizeof(szData), &cbData);
			string speed = (const char*)szData;

			wstring w_prot, w_speed;
			StringToWString(w_prot, prot);
			StringToWString(w_speed, speed);
			output += L"<tr><td>" + w_prot + L"</td></tr>";
			output += L"<tr><th>Μέγιστη δυνατή ταχύτητα</th></tr><tr><td>" + w_speed + L"</td></tr></table>";
			rowCount++;
			if (prot.c_str()[6] == 'b') //check b/g/n
				good_result = false;
		}

		if (good_result)
			output += L"<p><h5>Αποτέλεσμα: δεν χρειάζεται βελτιστοποίηση</h5></p>";
		else
			output += L"<p><h4>Αποτέλεσμα: η ταχύτητα του δικτύου είναι μικρή</h4></p>";
		rc = SQLFreeStmt(hStmt, SQL_DROP);
		if (rowCount == 0)
		{
			string error = "Database is empty. Program will exit.";
			MessageBox(NULL, str2(error), L"Error in signal analysis occured", MB_ICONEXCLAMATION | MB_OK);
			exit(0);
		}
	}
	else
	{
		string error = "Couldn't connect to database.";
		MessageBox(NULL, str2(error), L"Problem occured", MB_ICONEXCLAMATION | MB_OK);
		exit(0);
	}

	/* Disconnect and free up allocated handles */
	rc = SQLFreeStmt(hStmt, SQL_DROP);
	SQLDisconnect(hDbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
	return output;
}
