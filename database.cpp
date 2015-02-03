#include "stdafx.h"
#include "sqlite/sqlite3.h"




sqlite3 *db;
__int64 maxHandNo = 0;

static int callbackMaxHand(void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;
	for(i=0; i<argc; i++){
	//	Log("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}	
	maxHandNo = _atoi64(argv[0]);

	return 0;	 
}

static int callbackNewHand(void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;

	char filename[MAX_PATH];
	

	for(i=0; i<argc; i++) {
		//Log("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");

		if (strcmp(azColName[i],"handNo")==0) {
			__int64 handNo = _atoi64(argv[i]);
			if (handNo > maxHandNo)
				maxHandNo = handNo;
		}
		if (strcmp(azColName[i],"cards")==0) {
			if (strcmp("undefined",argv[i])==0)
				break;
		}
		if (strcmp(azColName[i],"tableName")==0) {
			time_t t = time(0);   // get time now
			struct tm * now = localtime( & t );
			sprintf_s(filename,"C:\\Users\\Max\\AppData\\Roaming\\PlanetWin365 Pro\\HandHistoryNG\\converted\\%4d-%02d-%02d-%s.txt",
				now->tm_year+1900, now->tm_mon+1, now->tm_mday, argv[i]);				
		}
		if (strcmp(azColName[i],"rawLines")==0) {
			int len = strlen(argv[i]);
			FILE *f;

			f = fopen(filename,"ab");
			if (f) {

				char* psHeader = "PokerStars";
				fwrite(psHeader,10,1,f);
				fwrite(argv[i]+10,len-10,1,f);
				fprintf(f,"\n\n");
				fclose(f);
			}
		}
	}	

	return 0;	 
}


void InitDB()
{
	char *zErrMsg = 0;
	int rc;

	rc = sqlite3_open("C:\\Users\\Max\\AppData\\Roaming\\PlanetWin365 Pro\\HandHistoryNG\\hhdb_DontYouDare.sqlite", &db);
	if( rc )
	{
		Log("Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_exec(db, "select max(handNo) from hand", callbackMaxHand, 0, &zErrMsg);
	if( rc!=SQLITE_OK ) {
		Log("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	
}


void OnNewHand()
{
	char *zErrMsg = 0;
	char buf[256];
	sprintf_s(buf,"select * from hand where handNo>%I64d",maxHandNo);
	int rc = sqlite3_exec(db, buf , callbackNewHand, 0, &zErrMsg);
	if( rc!=SQLITE_OK ) {
		Log("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
}

void CloseDB()
{
	sqlite3_close(db);	
}