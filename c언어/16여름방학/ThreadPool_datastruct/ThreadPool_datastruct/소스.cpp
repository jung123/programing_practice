#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <windows.h>
#include <tchar.h>
#include <mysql.h>
#include <time.h>
#pragma comment(lib, "libmysql.lib")

#define DB_HOST "127.0.0.1"
#define DB_USER "root"
#define DB_PASSWD "password"
#define DB_NAME "data_struct"
#define CREATE_QUERY "create table result_data123(`station_name` varchar(10) not null primary key,`weekday_set_13` int(10) unsigned not null,`weekday_clear_13` int(10) unsigned not null,`weekday_total_13` int(10) unsigned not null,`weekend_set_13` int(10) unsigned not null,`weekend_clear_13` int(10) unsigned not null,`weekend_total_13` int(10) unsigned not null,`weekday_set_14` int(10) unsigned not null,`weekday_clear_14` int(10) unsigned not null,`weekday_total_14` int(10) unsigned not null,`weekend_set_14` int(10) unsigned not null,`weekend_clear_14` int(10) unsigned not null,`weekend_total_14` int(10) unsigned not null,`weekday_set_15` int(10) unsigned not null,`weekday_clear_15` int(10) unsigned not null,`weekday_total_15` int(10) unsigned not null,`weekend_set_15` int(10) unsigned not null,`weekend_clear_15` int(10) unsigned not null,`weekend_total_15` int(10) unsigned not null,`weekday_pm_set_13` int(10) unsigned not null,`weekday_pm_clear_13` int(10) unsigned not null,`weekend_pm_set_13` int(10) unsigned not null,`weekend_pm_clear_13` int(10) unsigned not null,`weekday_pm_set_14` int(10) unsigned not null,`weekday_pm_clear_14` int(10) unsigned not null,`weekend_pm_set_14` int(10) unsigned not null,`weekend_pm_clear_14` int(10) unsigned not null,`weekday_pm_set_15` int(10) unsigned not null,`weekday_pm_clear_15` int(10) unsigned not null,`weekend_pm_set_15` int(10) unsigned not null,`weekend_pm_clear_15` int(10) unsigned not null)ENGINE = InnoDB CHARSET=utf8 COLLATE utf8_general_ci; "

#define WORKING 1
#define NOT_WORK 0
#define WEEK_END 1
#define WEEK_DAYS 0
/*
	pm ���� �κ�
*/
#define YEAR 3
#define MONTH 12
#define DAY 31
#define PM_CHECK 2
#define PM_WARN 120
#define PM_SET 'O'
#define PM_CLEAR 'X'
// ����
char date_time[YEAR][MONTH][DAY];
CRITICAL_SECTION criticalSection_DateTime;
// �Լ�
int get_day(char *date);
int get_month(char *date);
void create_pm_list(char *table_name, int year);
void pm_check();
void InitDateTime();

/*
Thread Pool ���� �κ� <��>
*/
#define MAX_THREAD_STATION 10
#define MAX_WORK_STATION 20

#define MAX_WORK_FOR_STATION_THREAD 30
// work
typedef struct station_info {
	unsigned int weekday_set_15;
	unsigned int weekday_clear_15;
	unsigned int weekend_set_15;
	unsigned int weekend_clear_15;
	unsigned int weekday_set_14;
	unsigned int weekday_clear_14;
	unsigned int weekend_set_14;
	unsigned int weekend_clear_14;
	unsigned int weekday_set_13;
	unsigned int weekday_clear_13;
	unsigned int weekend_set_13;
	unsigned int weekend_clear_13;

	unsigned int weekday_pm_set_15;
	unsigned int weekday_pm_clear_15;
	unsigned int weekend_pm_set_15;
	unsigned int weekend_pm_clear_15;
	unsigned int weekday_pm_set_14;
	unsigned int weekday_pm_clear_14;
	unsigned int weekend_pm_set_14;
	unsigned int weekend_pm_clear_14;
	unsigned int weekday_pm_set_13;
	unsigned int weekday_pm_clear_13;
	unsigned int weekend_pm_set_13;
	unsigned int weekend_pm_clear_13;
} station_info_result;

// ***********************<��>�м��ϴ� �����忡�� �ʿ��� ���� 

typedef struct _Work_For_Month {
	char table_name[25];
	char station_name[25];
	BYTE month;
	DWORD year;
	station_info_result *result_station;
	CRITICAL_SECTION *lpCriticalSection;
} Work_For_Month;

typedef struct __WorkList_For_Month {						// �ϳ��� malloc �Ҵ��Ѵ� !!
	Work_For_Month workList[MAX_WORK_FOR_STATION_THREAD];
	BYTE byteCurrentWork;
	BYTE byteLastAddWork;
} WorkList_For_Month;
// ********************************************************


typedef void(*threadStationWork)(char *, int);		// routine���� ó���ϴ� work

typedef struct {
	HANDLE hThread;
	DWORD dwThreadId;
	BYTE working;
	int WorkingMonth;
	CRITICAL_SECTION critical_section_for_THREAD;
	WorkList_For_Month *lpWorkList_Month;
} threadInfoStation;
typedef struct _ThreadPoolForStation{
	threadInfoStation threadStationInfo[MAX_THREAD_STATION];
	HANDLE hEvent[MAX_THREAD_STATION];
	char *WorkListStation[MAX_WORK_STATION];

	DWORD dwWorkCuurent;		// init = 1
	DWORD dwWorkLastAdded;		// init = 0
	DWORD dwThreadIdx;

}threadPoolStation;
// ����
threadPoolStation threadPool_Station;
CRITICAL_SECTION criticalSection_Thread_station;
char *station_info[] = { "���￪(150)","��û(151)","����(152)","����3��(153)","����5��(154)","���빮(155)","�ż���(156)","���⵿(157)",
"û����(����)(158)","������(159)","��û(201)","�������Ա�(202)","������3��(203)","������4��(204)","���빮���繮ȭ����(205)","�Ŵ�(206)",
"��սʸ�(207)","�սʸ�(������û)(208)","�Ѿ��(209)","�Ҽ�(210)","����(211)","�Ǵ��Ա�(212)","����(213)","����(214)","��ǳ���(215)",
"���(216)","��õ(217)","���տ��(218)","�Ｚ(219)","����(220)","����(221)","����(222)","����(223)","����(224)","���(225)","���(226)",
"������(227)","������Ա�(228)","��õ(229)","�Ÿ�(230)","�Ŵ��(231)","���ε����д���(232)","�븲(233)","�ŵ���(234)","����(235)",
"��������û(236)","���(237)","����(238)","ȫ���Ա�(239)","����(240)","�̴�(241)","����(242)","������(243)","���(244)","�Ŵ�(245)",
"�ż���(246)","����õ(247)","��õ��û(248)","�����װŸ�(249)","���(250)",
"����(309)","���Ĺ�(310)","���ų�(311)","�ұ�(312)","���(313)","ȫ��(314)",
"������(315)","������(316)","�溹��(317)","�ȱ�(318)","����3��(319)",
"������3��(320)","�����Ա�(322)","���(323)","��ȣ(324)","����(325)",
"�б���(326)","�Ż�(327)","���(328)","����͹̳�(329)","����(330)",
"�����͹̳�(331)","����(332)","�ź�(333)","����(334)","��ġ(335)","�п���(336)",
"��û(337)","�Ͽ�(338)","����(339)","��������(340)","��������(341)","����(342)"
};

// �Լ�  makeThreadPool, addWork, GetWork, ClearThreadPool
unsigned int WINAPI ThreadProcForStation(LPVOID lpparam);		// <��>�� ó���ϴ� ��������  StartRoutine !
void MakeThreadPool_station(void);
void AddWork_Station(char *station_name);	// ���ڸ� ���� �� ������ Ǯ�� worklist�� �־�����...<Main>
char *GetWork_Station();
void ClearThreadPool_Station();
void anlyze_Station(char *station_name, int index);
void Check_End_Thread_Station();
int GetYear_Astation(char *table_name);
int GetMonth_Astation(char *table_name);
void Init_WorkList_For_Month(WorkList_For_Month *workList_month);
/*
Thread Pool ���� �κ� <��>
*/
#define MAX_THREAD_MONTH 40
#define MAX_WORK_MONTH 300

typedef void(*WORK_MONTH)(char *table_name, char *station_name, int month,DWORD year, station_info_result *lpResult, CRITICAL_SECTION *lpCriticalSection);	// ���� ó���ϴ� �����尡 
typedef struct _Month_ThreadInfo {
	HANDLE hThread;
	DWORD dwThreadId;
	BYTE working;
} MonthThread_info;
typedef struct _Month_ThreadPool {
	MonthThread_info arrThread[MAX_THREAD_MONTH];
	HANDLE hEventList[MAX_THREAD_MONTH];
	DWORD dwWorkList[MAX_WORK_MONTH];
	DWORD dwCrrentWork;
	DWORD dwLastaddWork;
	DWORD dwThreadIdx;
}MonthThreadPool;
// ����
CRITICAL_SECTION critical_section_month;
MonthThreadPool ThreadPool_Month;

int start_day[3][12] = { { 1,4,4,0,2,5,0,3,6,1,4,6 },{ 2,5,5,1,3,6,1,4,0,2,5,0 },{ 3,6,6,2,4,0,2,5,1,3,6,1 } };
// �Լ�
void MakeThreadPool_Month(void);
void AddWork_Month(int index, char *table_name, char *station_name, int month, int year, station_info_result *lpResult, CRITICAL_SECTION *lpCriticalSection, WorkList_For_Month *workList_month);		// <��>�м��ϴ� ������ �ڽ��� index

void GetWorkMonth(Work_For_Month *lpWork, DWORD index);		// � �����忡 �����ؾߴ���� �߿�
unsigned int WINAPI ThreadProc_Month(LPVOID lpparam);
void analyze_station_month(char *table_name, char *station_name, int month, DWORD year, station_info_result *lpResult, CRITICAL_SECTION *lpCriticalSection);
void ClearThreadPool_Month();
/*
	Connection Pool ���� �κ�
	dwConnectionIdx �� �� lpConnection�� ���� ��
	dwConnectionIdx�� ���ڷ� �ش� Ŀ�ؼ� ��ȯ���ִ� ������� ����
	���������� ��� Ŀ�ؼ� close ���ش� !!.
*/
#define CONNECTION_MAX 100

typedef struct _connection {
	MYSQL conn;
	MYSQL *connection;
}connection_info;

typedef struct _ConnectionPool {
	connection_info db_connect_pool[CONNECTION_MAX];
	BYTE working[CONNECTION_MAX];
}ConnectionPool;

// ����
CRITICAL_SECTION criticalSectionForDBconnector;
ConnectionPool connection_Pool;
// �Լ�
void MakeConnectionPool();
MYSQL *GetConnection(DWORD index);
DWORD GetConnectionIdx();
void ReturnConnection(DWORD index);
void ClearConnections();

/*
	�Լ�
*/
void error(char *message);
void print_message(char *message);
void create_table();
void Init_tempResult(station_info_result *temp);
int GetDay_Analyze(char *date, int year);
void InsertTempResult(station_info_result *result, station_info_result *temp_result);
void Excute_Query(station_info_result *result, char *station_name);
int GetWeekInfo(int year, int month, int day);
void Insert_pm_data(station_info_result *result);
void CheckResult(station_info_result * result, char *station_name);

int main() {
	clock_t start, end;
	float runtime = 0;
	start = clock();


	DWORD query_stat;
	MakeConnectionPool();							// Ŀ�ؼ� Ǯ ����
	MakeThreadPool_station();						// <��> �м��ϴ� ������ Ǯ ����
	create_table();								// ������� ���� �� �ִ� ���̺����

	pm_check();										// pm ���� ������ ����
	for (DWORD i = 0; i < YEAR; i++) {
		printf("%d��\n", i + 2013);
		for (DWORD j = 0; j < MONTH; j++) {
			for (DWORD z = 0; z < DAY; z++) {
				printf("%c ",date_time[i][j][z]);
			}
			printf("%d��\n", j+1);
		}
	}

	MakeThreadPool_Month();							// <��> �м��ϴ� ������ Ǯ ����
	for (DWORD i = 0; i < 92; i++) {				// <��> �м��ϴ� ������ Ǯ�� work �߰��Ѵ�. !
		AddWork_Station(station_info[i]);
	}

	printf("*************************************\n\n Add Work End !! \n\n***************************************\n\n");
	Check_End_Thread_Station();						// <��> �м��ϴ� ������ Ǯ�� ������� �� ó���ߴ��� �˻�

	DeleteCriticalSection(&criticalSection_DateTime);
	ClearThreadPool_Station();						// ������ Ǯ�� ������ Ŀ�� ������Ʈ �ݳ� <�̺�Ʈ ������Ʈ>
	ClearThreadPool_Month();
	ClearConnections();								// Ŀ�ؼ� ����

	end = clock();
	runtime = (float)(end - start) / CLOCKS_PER_SEC;
	printf(" \n\n ******************************************************************************************\n");
	printf("             ����ð�  =  %d�� %d��\n", (int)runtime/60,(int)runtime%60);
	return 0;
}
/*
	error(char *meesage)
	������¿� �Լ�
*/
void error(char *message) {
	fprintf_s(stderr, message);
	exit(1);
}
/*
	print_message(char *message)
	�޼��� ��¿� �Լ� !!
*/
void print_message(char *message) {
	fprintf_s(stdout, message);
	return;
}
/*
	void create_table();
	<��>�� �м��� ��������� ���� ���̺� �����ϴ� �Լ�
*/
void create_table() {
	MYSQL *connection = NULL;
	DWORD dwConnectionIdx, query_stat = 0;

	dwConnectionIdx = GetConnectionIdx();
	connection = GetConnection(dwConnectionIdx);

	// ���̺��� �����ϸ� ����� �ٽ� ����� !!

	query_stat = mysql_query(connection, "drop table if exists result_data123");
	if (query_stat != 0) {
		printf(" --- Mysql Query Error : %s < create table >\n\n", mysql_error(connection));
		exit(1);
	}
	
	query_stat = mysql_query(connection, CREATE_QUERY);
	if (query_stat != 0) {
		printf(" --- Mysql Query Error : %s < create table >\n\n", mysql_error(connection));
		exit(1);
	}
	ReturnConnection(dwConnectionIdx);

}
////------------------------------------------------------------------////
/*
	MakeConnectionPool
	connectionpool �ʱ�ȭ�ϴ� �Լ�
*/
void MakeConnectionPool() {
	DWORD i = 0;
	InitializeCriticalSection(&criticalSectionForDBconnector);		// CriticalSection �ʱ�ȭ !!
	EnterCriticalSection(&criticalSectionForDBconnector);
	for (i = 0; i < CONNECTION_MAX; i++) {
		mysql_init(&connection_Pool.db_connect_pool[i].conn);	// mysql ���� �� �ʱ�ȸ
		mysql_options(&connection_Pool.db_connect_pool[i].conn, MYSQL_SET_CHARSET_NAME, "euckr");		// �ѱ� ���� �� �ְ� ����
		mysql_options(&connection_Pool.db_connect_pool[i].conn, MYSQL_INIT_COMMAND, "SET NAMES euckr");

		connection_Pool.db_connect_pool[i].connection = mysql_real_connect(&connection_Pool.db_connect_pool[i].conn, DB_HOST, DB_USER, DB_PASSWD, DB_NAME, 3306, (char *)NULL, 0);
		if (connection_Pool.db_connect_pool[i].connection == NULL) error("Connection INIT ERROR !! <MakeConnectionPool>\n ");
		connection_Pool.working[i] = NOT_WORK;
	}
	LeaveCriticalSection(&criticalSectionForDBconnector);
}
/*
	MYSQL *GetConnection();
	Connection Pool���� �Ҵ���� ���� Connection�� index�� return���ش�.
	
*/

DWORD GetConnectionIdx() {
	DWORD i = 0;
	EnterCriticalSection(&criticalSectionForDBconnector);
	while (1) {
		i = 0;
		while (i < CONNECTION_MAX) {
			if (connection_Pool.working[i] == NOT_WORK) {
				connection_Pool.working[i] = WORKING;
				LeaveCriticalSection(&criticalSectionForDBconnector);
				return i;												// �ּҰ��� return !!
			}
			i++;
		}
		LeaveCriticalSection(&criticalSectionForDBconnector);
		print_message("Connection FULL!! \n");
		Sleep(2000);																// ������ Ǯ�� connection ��°����� block���¸� ǥ�� �� blocking�ȴ���
		EnterCriticalSection(&criticalSectionForDBconnector);						// ReturnConnection���� connection�� ��ٸ��� �������� �̺�Ʈ�� ��Ʈ���ִ� �������� ������ٰ�
																					// Ŀ�ؼ� ���� ������ ���� ����� �����尡 ó���ϴ� ���� �����غ� ��� 2������ Sleep���൵ 
																					// Connection�� ���� �� �ִٴ� ����� ���� !
	}
}
/*
	MYSQL *GetConnection(DWORD index);
	Connection Pool���� index�� ��ġ�� �ִ� connection�� return���ִ� �Լ�
*/
MYSQL *GetConnection(DWORD index) {
	MYSQL *db_conn;
	EnterCriticalSection(&criticalSectionForDBconnector);
	db_conn = connection_Pool.db_connect_pool[index].connection;
	LeaveCriticalSection(&criticalSectionForDBconnector);
	//print_message("\n\n%d Connection Acquired !! \n\n", index);
	return db_conn;
}

/*
	void ReturnConnection(DWORD );
	Connection �ݳ� !!
*/
void ReturnConnection(DWORD index) {
	EnterCriticalSection(&criticalSectionForDBconnector);
	connection_Pool.working[index] = NOT_WORK;
	connection_Pool.db_connect_pool[index].connection = &connection_Pool.db_connect_pool[index].conn;
	LeaveCriticalSection(&criticalSectionForDBconnector);
	//print_message("%d Connection Returned !! \n", index);
}
/*
	void ClearConnections();
	��� connection�� Close�Ѵ�.
*/
void ClearConnections() {
	DWORD i = 0;
	for (i = 0; i < CONNECTION_MAX; i++) {
		mysql_close(&connection_Pool.db_connect_pool[i].conn);
		mysql_close(connection_Pool.db_connect_pool[i].connection);
	}
	DeleteCriticalSection(&criticalSectionForDBconnector);
}
/*
	void InitDateTime()
	Date_time �������� �ʱ�ȭ�ϴ� �Լ�
*/

void InitDateTime() {
	InitializeCriticalSection(&criticalSection_DateTime);		// dateTime�� ũ��Ƽ�ü��� �ʱ�ȭ !!
	DWORD i, j, z;
	for (i = 0; i < YEAR; i++) {
		for (j = 0; j < MONTH;j++) {
			for (z = 0; z < DAY;z++) {
				date_time[i][j][z] = '\0';
			}
		}
	}

	return;
}
/*
	- pm_check() :	 date_time[][][] �迭�� ��¥�� pm ������ �����ϴ� �Լ��̴�.
	create_pm_list �Լ��� ȣ���ϰ� �ȴ�.
*/

void pm_check() {
	MYSQL *connection;
	DWORD dwConnectionIdx = 0;
	MYSQL_RES *sql_table_result; // show tables ��� 
	MYSQL_ROW sql_table_row;	// table fetch ���

	dwConnectionIdx = GetConnectionIdx();
	connection = GetConnection(dwConnectionIdx);

	int query_stat;
	int n = 0;

	char selectQuery[100] = "select * from ";	// ������ ���� �Ŀ� ���ο� ������������ �����ʱ�ȭ�� �ʿ��ϴ� !

												// table ������
	query_stat = mysql_query(connection, "show tables");
	if (query_stat != 0) error("mysql query error : %s\n");

	sql_table_result = mysql_store_result(connection);
	while ((sql_table_row = mysql_fetch_row(sql_table_result)) != NULL) {	// table ���� ��� �޾ƿ´� 
																			// pm���̺� ���� �ٸ��Ŵ� �׳� �ѱ�
		char *pos;
		char *pos1;
		if ((pos = strstr(sql_table_row[0], "pm")) != NULL) {
			// pm table �� �������� ���� ����.!!
			if ((pos1 = strstr(sql_table_row[0], "13")) != NULL) {			// 13�⵵ pm table�ΰ��
				create_pm_list(sql_table_row[0], 13);
			}
			else if ((pos1 = strstr(sql_table_row[0], "14")) != NULL) {		// 14�⵵ pm table�ΰ�� 
				create_pm_list(sql_table_row[0], 14);
			}
			else if ((pos1 = strstr(sql_table_row[0], "15")) != NULL) {		// 15�⵵ pm table�� ���
				create_pm_list(sql_table_row[0], 15);
			}
		}
	} // PM table�� ������ �� ���� !!

	mysql_free_result(sql_table_result);	// ������� ����
	ReturnConnection(dwConnectionIdx);
}
/*
	create_pm_list() : �м��� ���̺� �̸��� ���������� �������� �ش�⵵�� �Ϻ� pm������
	date_time[][][]�� ���������� �����ϴ� �Լ�
*/

void create_pm_list(char *table_name, int year) {
	MYSQL *connection;
	DWORD dwConnectionIdx;
	MYSQL_RES *select_res;
	MYSQL_ROW select_row;

	dwConnectionIdx = GetConnectionIdx();				// ���⼭�� Ŀ�ؼ�Ǯ�� ���� �ʾ��� Ȯ���� �����Ƿ� ����ϴ� �ڵ带 �ȳ���
	connection = GetConnection(dwConnectionIdx);
	char select_query[50] = "select ";
	char *select_query00 = "select * from ";		// + table_name

	int query_stat;		// �������� ��ȯ�� ��� ������ strstr ��ȯ�� ��� ����
	int j, pm_time, pm = 0;		// pm���� ������ �ʿ��� ������
	int month, day, pm_sum;				// �ش糯¥�� month�� day�� �޾ƿͼ� return�ϴ� �Լ��� ȣ��
										/*
										���� ���� pm ���̺� ����
										*/
	select_query[0] = '\0';
	strcat_s(select_query, sizeof(select_query), select_query00);
	strcat_s(select_query, sizeof(select_query), table_name);
	//

	query_stat = mysql_query(connection, select_query);
	if (query_stat != 0) error("create_list() : mysql_query(select) error !! \n");

	select_res = mysql_store_result(connection);
	year = year - 13;
	while ((select_row = mysql_fetch_row(select_res)) != NULL) {
		/*
			���⼭ select_row[1]�� ��¥�� month day�� �޾ƿ��� �Լ��ʿ�
		*/
		month = get_month(select_row[1]);
		day = get_day(select_row[1]);
		pm = 0;
		for (j = 2; j < 27; j++) {		// �ش� ���� pm���� ����
			if (select_row[j] != NULL) {		// �ش�ð��� ������ �ȵȰ��
				pm_time = atoi(select_row[j]);
				if (pm_time > PM_WARN) {
					pm++;
				}
			}
		}
		//
		if (pm >= PM_CHECK) {				// pm
			date_time[year][month][day] = PM_SET;		// �Ϻ� pm ���� or clear Ȯ��
		}
		else if (pm < PM_CHECK) {
			date_time[year][month][day] = PM_CLEAR;
		}
	}
	mysql_free_result(select_res);
	ReturnConnection(dwConnectionIdx);
	return;
}
/*
	get_month()
	month ���� �������� !!
*/

int get_month(char *date) {
	char temp[20] = "\0";
	char *token, *month, *context;
	
	strncpy_s(temp, sizeof(temp), date, strlen(date));
	token = strtok_s(temp, ".", &context);
	month = strtok_s(NULL, ".", &context);
	
	return atoi(month)-1;
}
/*
	get_day()
	day ���� �������� !!
*/
int get_day(char *date) {
	char temp[20] = "\0";
	char *token, *day, *context;

	strncpy_s(temp, sizeof(temp), date, strlen(date));
	token = strtok_s(temp, ".", &context);
	token = strtok_s(NULL, ".", &context);
	day = strtok_s(NULL, ".", &context);

	return atoi(day)-1;
}
////------------------------------------------------------------------////
/*
	unsigned int ThreadProcForStation(LPVOID lpparam);
	<��>�� �м��ϴ� �������� main�Լ�
	���⼭ �м��� ������ ���(work���)
*/
unsigned int WINAPI ThreadProcForStation(LPVOID lpparam) {
	DWORD *index = (DWORD *)lpparam;
	threadStationWork work = anlyze_Station;
	HANDLE hEvent;
	char *station_name = NULL;
	EnterCriticalSection(&criticalSection_Thread_station);
	hEvent = threadPool_Station.hEvent[(DWORD)lpparam];
	LeaveCriticalSection(&criticalSection_Thread_station);
	while (1) {
		station_name = GetWork_Station();
		if (station_name == NULL) {
			WaitForSingleObject(hEvent, INFINITE);
			continue;
		}
		
		EnterCriticalSection(&criticalSection_Thread_station);
		threadPool_Station.threadStationInfo[(DWORD)lpparam].working = WORKING;
		LeaveCriticalSection(&criticalSection_Thread_station);
		work(station_name, (DWORD)lpparam);
		EnterCriticalSection(&criticalSection_Thread_station);
		threadPool_Station.threadStationInfo[(DWORD)lpparam].working = NOT_WORK;
		LeaveCriticalSection(&criticalSection_Thread_station);
		station_name = NULL;
	}

	return 0;
}
/*
	void MakeThreadPool_station(void);
	<��>�� �м��ϴ� ������Ǯ�� �����. !!
	������ ����, Event(�ڵ�, non-signaled)
*/
void MakeThreadPool_station(void) {
	InitializeCriticalSection(&criticalSection_Thread_station);			// ������ Ǯ �Ӱ迵�� ����

	DWORD i = 0, dwThreadId = 0;
	HANDLE hThread;
	EnterCriticalSection(&criticalSection_Thread_station);
	for (i = 0; i < MAX_THREAD_STATION; i++) {
		hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProcForStation,(void *)threadPool_Station.dwThreadIdx, 0, (unsigned *)&dwThreadId);
		if (hThread == NULL) error("Station Thread Create Fail !! \n");
		threadPool_Station.threadStationInfo[i].hThread = hThread;
		threadPool_Station.threadStationInfo[i].dwThreadId = dwThreadId;
		threadPool_Station.hEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		threadPool_Station.threadStationInfo[i].working = NOT_WORK;
		InitializeCriticalSection(&threadPool_Station.threadStationInfo[i].critical_section_for_THREAD);	// Thread�� �����ϴ� critical Section INIT !
		threadPool_Station.dwThreadIdx++;
		threadPool_Station.threadStationInfo[i].WorkingMonth = 0; // ���߿� 35���Ǹ� �ڽ��� �ֹ��� ���� ��� �������̴�.
		threadPool_Station.threadStationInfo[i].lpWorkList_Month = NULL;
	}
	for (i = 0; i < MAX_WORK_STATION; i++) {
		threadPool_Station.WorkListStation[i] = NULL;
	}
	threadPool_Station.dwWorkCuurent = 0;
	threadPool_Station.dwWorkLastAdded = 1;
	LeaveCriticalSection(&criticalSection_Thread_station);

}
/*
	void AddWork_Station(char *station_name);
	WorkList�� ȯ������ �����. ���������� waiting�Ѵ�. !! <�������� ???><�������� ��������� �����ؾ��Ѵ� !>
	������ Ǯ�� worklist�� �������� ���ڿ��� �ּҸ� �־��ش� !
*/
void AddWork_Station(char *station_name) {
	DWORD current, lastadd, i=0;
	EnterCriticalSection(&criticalSection_Thread_station);			// critical section Start !
	current = threadPool_Station.dwWorkCuurent;
	lastadd = threadPool_Station.dwWorkLastAdded;
	while (1) {		// worklist�� ���� �־��� ��
		if ( (current -1 == lastadd) || ((current == 0) && (lastadd == MAX_WORK_STATION-1)) ) {
			//print_message("Station Work List is FULL \n");
			LeaveCriticalSection(&criticalSection_Thread_station);
			Sleep(5000);
			EnterCriticalSection(&criticalSection_Thread_station);
			current = threadPool_Station.dwWorkCuurent; 
			lastadd = threadPool_Station.dwWorkLastAdded;
		}
		else {
			threadPool_Station.WorkListStation[threadPool_Station.dwWorkLastAdded++] = station_name;
			if (threadPool_Station.dwWorkLastAdded == MAX_WORK_STATION) threadPool_Station.dwWorkLastAdded = 0;
			break;
		}
	}
	while(1){	// blocking �Ǿ� �ִ� �����带 ������ ���� �ϰ��ϴ� ����
		if (threadPool_Station.threadStationInfo[i].working == NOT_WORK) {
			SetEvent(threadPool_Station.hEvent[i]);
			//threadPool_Station.threadStationInfo[i].working = WORKING;//////////////////////////////////////////////////////
			break;
		}
		i++;
		if (i == MAX_WORK_STATION) {
			i = 0;
			LeaveCriticalSection(&criticalSection_Thread_station);
			Sleep(2000);
			EnterCriticalSection(&criticalSection_Thread_station);
		}
	}
	LeaveCriticalSection(&criticalSection_Thread_station);			// critical section End

}
/*
	char *GetWork_Station();
	work����Ʈ�� �����ϴ� ó���� ������ �ּҸ� return���ִ� �Լ��̴�.
	return�� �ڿ��� CurrentworkIndex�� �÷�����Ѵ�. ! �׸��� �ش� work�κ��� '\0'���� �ʱ�ȭ
*/
char *GetWork_Station() {	
	DWORD current = 0, lastadd = 0;
	char *station_name = NULL;
	EnterCriticalSection(&criticalSection_Thread_station);
	current = threadPool_Station.dwWorkCuurent;
	lastadd = threadPool_Station.dwWorkLastAdded;
	if ((current + 1 == lastadd) || ((current == MAX_WORK_STATION) && (lastadd == 0))) {	// worklist ����ִ°��
		LeaveCriticalSection(&criticalSection_Thread_station);
		return NULL;
	}
	if (threadPool_Station.dwWorkCuurent == MAX_WORK_STATION-1) {
		threadPool_Station.dwWorkCuurent = 0;
		station_name = threadPool_Station.WorkListStation[threadPool_Station.dwWorkCuurent];
	}
	else station_name = threadPool_Station.WorkListStation[++threadPool_Station.dwWorkCuurent];
	LeaveCriticalSection(&criticalSection_Thread_station);
	return station_name;
}
/*
	������ Ǯ�� ��� Ŀ�ο�����Ʈ�� ��ȯ�Ѵ� !
*/
void ClearThreadPool_Station() {
	EnterCriticalSection(&criticalSection_Thread_station);
	for (DWORD i = 0; i < MAX_THREAD_STATION; i++) {
		CloseHandle(threadPool_Station.hEvent[i]);
		DeleteCriticalSection(&threadPool_Station.threadStationInfo[i].critical_section_for_THREAD);
	}
	LeaveCriticalSection(&criticalSection_Thread_station);
	DeleteCriticalSection(&criticalSection_Thread_station);
}
/*
	void Init_WorkList_For_Month(WorkList_For_Month *workList_month);
	<��>�� ó���ϴ� ������ Ǯ�� �����尡 ���� ó���ϴ� <��>���� �ڽ��� ���� �ñ��
	workList�� �ʱ�ȭ�ϴ� �Լ��̴�
*/
void Init_WorkList_For_Month(WorkList_For_Month *workList_month) {
	workList_month->byteCurrentWork = 0;
	workList_month->byteLastAddWork = 1;
	for (BYTE i = 0; i < MAX_WORK_FOR_STATION_THREAD; i++) {
		workList_month->workList[i].lpCriticalSection = NULL;
		workList_month->workList[i].month = 0;
		workList_month->workList[i].result_station = NULL;
		workList_month->workList[i].station_name[0] = '\0';
		workList_month->workList[i].table_name[0] = '\0';
		workList_month->workList[i].year = 0;
	}

}
/*
	void anlyze_Station(char *station_name);
	�ش� ���� 13 ~ 15�⵵ �����α� ���� !!
	date_time�� ���ٽ� �б⸸ �ϱ� ������ ��������� ����� ���� ������ �ϴ�.
	�� �Լ� �ȿ��� ������ ������<��>���� work�� �ִ� ���� ����ų� �ٸ� �Լ� �ϳ��� ȣ���ϵ��� ������ !!
	ȣ���� ������� �ش翪�� ã�ƾ��Ѵ� !!
*/

void anlyze_Station(char *station_name, int index) {
	MYSQL *connection = NULL, *connection1 = NULL;
	MYSQL_RES *sql_result = NULL, *sql_result1 = NULL;
	MYSQL_ROW sql_row;
	DWORD dwConnectionIdx, dwConnectionIdx1, dwQuery_stat = 0, number = 0;
	station_info_result *result_station = NULL;
	CRITICAL_SECTION *lpCriticalSection = NULL;
	WorkList_For_Month *workList_month = NULL;

	int temp1, num_rows = 0, year = 0, month = 0;
	char query_buffer1[15], *temp, query_buffer[110];
	query_buffer[0] = query_buffer1[0] = '\0';
	// �����Ҵ� (heap)���� => ��������� ������ �� �ֵ���
	result_station = (station_info_result *)malloc(sizeof(station_info_result));
	if (result_station == NULL) error(" --- result station Malloc Fail !! \n");

	workList_month = (WorkList_For_Month *)malloc(sizeof(WorkList_For_Month));
	if (workList_month == NULL) error(" --- WorkList For Month Malloc Fail !! \n");
	Init_WorkList_For_Month(workList_month);
	// ���������� ����ü �ʱ�ȭ ==> '0'���� �����
	Init_tempResult(result_station);	
	//
	EnterCriticalSection(&criticalSection_Thread_station);						// �ش� ���� ��������� ��� ���� ����ȭ�� �ʿ��ϱ� ������
	lpCriticalSection = &threadPool_Station.threadStationInfo[index].critical_section_for_THREAD;
	threadPool_Station.threadStationInfo[index].lpWorkList_Month = workList_month;	// <��>�м� �������� workList ==> ���м� �����尡 ����
	LeaveCriticalSection(&criticalSection_Thread_station);
	// 13 ~15 �⵵ pm ���õȳ��� �ȵȳ����� ������ ��� �������� ���� // ����ü�� �����ϴ� ������� �̰��ϳ��̹Ƿ� ũ��Ƽ�ü��� �� �ʿ� ����
	Insert_pm_data(result_station);
	
	temp1 = sprintf_s(query_buffer1, sizeof(query_buffer1), "show tables");
	if (temp1 == -1) error("Error String <Analyze Station !!>22\n");
	/*
		connection �α� -> query -> store -> free - Close �� ������ ����
		Error�� �߻� ������ ������ ���� ������ �����Ѿ� �Ѵٸ� �ϳ��� Ŀ�ؼ��� �� �ʿ��ϴ�. !!
		!!! ���⼭ ����ؾ��� �� : ���� �� Ŀ�ؼ� ��ӻ��(������ �ؾ���), �������� �ʰ� �ٸ� Ŀ�ؼ��� ���� �� Ŀ�ؼ� ����(Ŀ�ؼ� ����� �� ����)
	*/
	dwConnectionIdx = GetConnectionIdx();		//show tables �� connect
	connection = GetConnection(dwConnectionIdx);
	dwConnectionIdx1 = GetConnectionIdx();		//
	connection1 = GetConnection(dwConnectionIdx1);
	
	dwQuery_stat = mysql_query(connection, query_buffer1);
	if (dwQuery_stat != 0) {
		printf(" --- Query Error !! <Analyze Station> --- %d ldx Error : %s\n", dwConnectionIdx, mysql_error(connection));
		exit(1);
	}
	sql_result = mysql_store_result(connection);
	// printing !!
	printf(" *** %s Station Analyzed !! *** \n\n", station_name);
	
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {				// "show tables" ������ ���� �� �ϳ��� ������ ��ƾ
		if ((temp = strstr(sql_row[0], "line")) == NULL) continue;			// ����ö ���̺��� �ƴ� ���
		
		
		query_buffer[0] = '\0';												// buffer �ʱ�ȭ
		temp1 = sprintf_s(query_buffer, sizeof(query_buffer), "select * from %s where ���� = '%s' limit 1", sql_row[0], station_name);
		if (temp1 == -1) error(" --- Sprintf Error !! < Analyze Station >11\n");
		
		dwQuery_stat = mysql_query(connection1, query_buffer);				// query !!
		if (dwQuery_stat != 0) {											// Error !!
			printf(" --- Query Error !! <Analyze Station> --- %d ldx Error : %s\n", dwConnectionIdx, mysql_error(connection1));
			exit(1);
		}
		sql_result1 = mysql_store_result(connection1);						// result store !!
		num_rows = mysql_num_rows(sql_result1);								// num Of Result !!
		if (num_rows == 0) {												// result is Empty !!
			mysql_free_result(sql_result1);
			continue;
		}

		/*
			���⿡�ٰ� ���� �ش� ���� �м��ϴ� �ڵ带 �ۼ��ϸ���.. !!
			<��> ������ Ǯ ȣ�� !!
			���⿡�� Ŀ�ؼ��� �ʹ� ���� ����ϴ� �� ���⵵ �ϴ� !!
			!!!! ���⿡�� ���������� ���� �� �ִ� !!!!!
		*/
		Sleep(1000);
		year = GetYear_Astation(sql_row[0]);								// �������� ����
		// Add Work !!!! <��> �м��ϴ� ������Ǯ�� worklist��
		// month������ i =0 -> 12�� ADD������ if year == 2015 => table �̸����� ������ ���� !!
		if (year == 2015) {
			month = GetMonth_Astation(sql_row[0]);
			AddWork_Month(index, sql_row[0], station_name, month, year, result_station, lpCriticalSection, workList_month);
			number++;
		}else{
			for (DWORD i = 0; i < MONTH; i++) {
				AddWork_Month(index, sql_row[0], station_name,i, year, result_station, lpCriticalSection, workList_month);
				number++;
			}
		}
		mysql_free_result(sql_result1);										// sql_result1 _ connection1 _ Free !!
		
	}
	/// <��> �м��ϴ� �����尡 insert�� ��� work�� �� ó���Ǿ����� Ȯ��
	Sleep(5000);
	while (1) {
		EnterCriticalSection(&criticalSection_Thread_station);
		if (threadPool_Station.threadStationInfo[index].WorkingMonth == number) {
			threadPool_Station.threadStationInfo[index].WorkingMonth = 0;
			LeaveCriticalSection(&criticalSection_Thread_station);
			break;
		}
		LeaveCriticalSection(&criticalSection_Thread_station);
		Sleep(5000);														// �� ������ blocking
	}

	/////////////////// �׳�Ȯ�� !!

	CheckResult(result_station, station_name);

	///////////////////
	

	/// ���� �ڽ��� insert�� ��� work�� ó���� �����̴� !! ==> ����ü�� ��� ������ query�� �������Ѵ� !!
	Excute_Query(result_station, station_name);
	/// �ڽ��� �����س����͵� ��� �����ؾ��Ѵ� !!
	
	connection = NULL;
	mysql_free_result(sql_result);											// sql_result _ connection _ Free !!
	free(workList_month);
	free(result_station);
	
	ReturnConnection(dwConnectionIdx1);
	ReturnConnection(dwConnectionIdx);
	
	return;
}

/*
	
*/
void CheckResult(station_info_result * result, char *station_name) {

	printf(" ** station name : %s �м���� !!\n");
	printf(" ********** 13�⵵ **********\n\n");
	printf(" --- ���� --- PM_SET �� ��� : %d,  PM_CLEAR �� ��� : %d\n", result->weekday_set_13 / result->weekday_pm_set_13, result->weekday_clear_13 / result->weekday_pm_clear_13);
	printf(" --- �ָ� --- PM_SET �� ��� : %d,  PM_CLEAR �� ��� : %d\n\n", result->weekend_set_13 / result->weekend_pm_set_13, result->weekend_clear_13 / result->weekend_pm_clear_13);
	printf(" ********** 14�⵵ **********\n\n");
	printf(" --- ���� --- PM_SET �� ��� : %d,  PM_CLEAR �� ��� : %d\n", result->weekday_set_14 / result->weekday_pm_set_14, result->weekday_clear_14 / result->weekday_pm_clear_14);
	printf(" --- �ָ� --- PM_SET �� ��� : %d,  PM_CLEAR �� ��� : %d\n\n", result->weekend_set_14 / result->weekend_pm_set_14, result->weekend_clear_14 / result->weekend_pm_clear_14);
	printf(" ********** 15�⵵ **********\n\n");
	printf(" --- ���� --- PM_SET �� ��� : %d,  PM_CLEAR �� ��� : %d\n", result->weekday_set_15 / result->weekday_pm_set_15, result->weekday_clear_15 / result->weekday_pm_clear_15);
	printf(" --- �ָ� --- PM_SET �� ��� : %d,  PM_CLEAR �� ��� : %d\n\n", result->weekend_set_15 / result->weekend_pm_set_15, result->weekend_clear_15 / result->weekend_pm_clear_15);

	return;
}
/*
	void Excute_Query(station_info_result *result)
*/
void Excute_Query(station_info_result *result, char *station_name) {
	DWORD dwConnectionIdx;
	MYSQL *connection = NULL;
	char insert_query[400];
	insert_query[0] = '0';
	int query_stat;

	unsigned int total_weekday_13;
	unsigned int total_weekday_14;
	unsigned int total_weekday_15;
	unsigned int total_weekend_13;
	unsigned int total_weekend_14;
	unsigned int total_weekend_15;

	total_weekday_13 = (result->weekday_clear_13) + (result->weekday_set_13);
	total_weekday_14 = (result->weekday_clear_14) + (result->weekday_set_14);
	total_weekday_15 = (result->weekday_clear_15) + (result->weekday_set_15);
	total_weekend_13 = (result->weekend_clear_13) + (result->weekend_set_13);
	total_weekend_14 = (result->weekend_clear_14) + (result->weekend_set_14);
	total_weekend_15 = (result->weekend_clear_15) + (result->weekend_set_15);

	sprintf_s(insert_query, sizeof(insert_query), "insert into result_data123 values('%s','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld','%ld')",
		station_name,
		result->weekday_set_13,
		result->weekday_clear_13,
		total_weekday_13,
		result->weekend_set_13,
		result->weekend_clear_13,
		total_weekend_13,
		result->weekday_set_14,
		result->weekday_clear_14,
		total_weekday_14,
		result->weekend_set_14,
		result->weekend_clear_14,
		total_weekend_14,
		result->weekday_set_15,
		result->weekday_clear_15,
		total_weekday_15,
		result->weekend_set_15,
		result->weekend_clear_15,
		total_weekend_15,
		result->weekday_pm_set_13,
		result->weekday_pm_clear_13,
		result->weekend_pm_set_13,
		result->weekend_pm_clear_13,
		result->weekday_pm_set_14,
		result->weekday_pm_clear_14,
		result->weekend_pm_set_14,
		result->weekend_pm_clear_14,
		result->weekday_pm_set_15,
		result->weekday_pm_clear_15,
		result->weekend_pm_set_15,
		result->weekend_pm_clear_15);

	dwConnectionIdx = GetConnectionIdx();
	connection = GetConnection(dwConnectionIdx);

	query_stat = mysql_query(connection, insert_query);
	if (query_stat != 0) {
		printf("insert_data() : insert query error!!!  Error : %s \n",mysql_error(connection));
		exit(1);
	}
	ReturnConnection(dwConnectionIdx);
	return;
}

/*
	int GetMonth_Astation(char *table_name)
	table �̸����� �������� ���� !!
*/
int GetYear_Astation(char *table_name) {
	char *year = NULL, *context;
	char temp_name[25];
	temp_name[0] = '\0';
	strncpy_s(temp_name, sizeof(temp_name), table_name, strlen(table_name));
	year = strtok_s(temp_name, "_", &context);
	return atoi(year);
}
/*
	int GetMonth_Astation(char *table_name);
*/
int GetMonth_Astation(char *table_name) {
	char *month = NULL, *context;
	char temp_name[25];
	temp_name[0] = '\0';
	strncpy_s(temp_name, sizeof(temp_name), table_name, strlen(table_name));
	month = strtok_s(temp_name, "_", &context);
	month = strtok_s(NULL, "_", &context);
	month = strtok_s(NULL, "_", &context);
	month = strtok_s(NULL, "_", &context);
	return atoi(context);
}

/*
	void Check_End_Thread_Station()
	<��>�� �м��ϴ� ������ Ǯ�� ��� �ϵ��� ó���߰� ��� �����尡 ���� ���� ���� ������ ���� Ȯ���� ��
	���α׷��� �����Ѵ�. !!
*/
void Check_End_Thread_Station() {
	DWORD stop = 0, current, lastadd;
	while (1) {
		Sleep(5000);
		stop = 0;
		EnterCriticalSection(&criticalSection_Thread_station);
		for (DWORD i = 0; i < MAX_THREAD_STATION; i++) {
			if (threadPool_Station.threadStationInfo[i].working == WORKING) stop++;
		}
		if (stop > 0) {
			LeaveCriticalSection(&criticalSection_Thread_station);
			continue;
		}
		current = threadPool_Station.dwWorkCuurent;
		lastadd = threadPool_Station.dwWorkLastAdded;
		if ((current + 1 == lastadd) || ((current == MAX_WORK_STATION - 1) && (lastadd == 0))) {	// worklist ����ִ°��
			LeaveCriticalSection(&criticalSection_Thread_station);
			break;
		}
		LeaveCriticalSection(&criticalSection_Thread_station);
	}
	printf("*************************************\n\n Program End !! \n\n *********************************\n\n");
	return;
}

////------------------------------------------------------------------////
/*
	MakeThreadPool_Month(void)
	<��>�� �м��ϴ� ������Ǯ�� �����ϴ� ��
*/
void MakeThreadPool_Month(void) {

	InitializeCriticalSection(&critical_section_month);

	EnterCriticalSection(&critical_section_month);
	ThreadPool_Month.dwCrrentWork = 0;
	ThreadPool_Month.dwLastaddWork = 1;
	for (DWORD i = 0; i < MAX_THREAD_MONTH; i++) {
		ThreadPool_Month.hEventList[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (ThreadPool_Month.hEventList[i] == NULL) error(" --- Make ThreadPool Error EventObject !! <MakeThreadPool_Month>");
		ThreadPool_Month.arrThread[i].hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc_Month, (void *)ThreadPool_Month.dwThreadIdx, 0, (unsigned *)&ThreadPool_Month.arrThread[i].dwThreadId);
		if (ThreadPool_Month.arrThread[i].hThread == NULL) error(" --- Make Thread Pool Error ThreadObject !! <MakeThreadPool_Month>");
		ThreadPool_Month.arrThread[i].working = NOT_WORK;
	}
	for (DWORD i = 0; i < MAX_WORK_MONTH; i++) {
		ThreadPool_Month.dwWorkList[i] = 0;
	}
	LeaveCriticalSection(&critical_section_month);

	return;
}
/*
	AddWork_Month
	<��>�� �м��ϴ� ������ Ǯ�� workList�� ���� �߰���Ű�� ��
	���� 1 : �ڽ��� index�� workList�� �߰�
	���� 2 : �ڽ��� �����ϴ� worklist�� ���� �߰�
	���� ������ ���ڼ��� ����Ǿ���Ѵ� !!
	�׷��� �����带 �����ش� !!
*/
void AddWork_Month(int index, char *table_name, char *station_name, int month,int year, station_info_result *lpResult, CRITICAL_SECTION *lpCriticalSection,
					WorkList_For_Month *workList_month) {
	DWORD current, lastadd;
	BYTE currentStation, lastaddStation, i =0;
	while (1) {
		EnterCriticalSection(&critical_section_month);
		current = ThreadPool_Month.dwCrrentWork;
		lastadd = ThreadPool_Month.dwLastaddWork;
		if ((lastadd+1 == current) || ((current == 0) && (lastadd == MAX_WORK_MONTH-1))) {						// �� �����
			LeaveCriticalSection(&critical_section_month);
			Sleep(1000);
			continue;
		}
		ThreadPool_Month.dwWorkList[ThreadPool_Month.dwLastaddWork++] = index;
		if (ThreadPool_Month.dwLastaddWork == MAX_WORK_MONTH) ThreadPool_Month.dwLastaddWork = 0; // ȯ�� !

		EnterCriticalSection(lpCriticalSection);
		currentStation = workList_month->byteCurrentWork;
 		lastaddStation = workList_month->byteLastAddWork;
		if ((currentStation == lastaddStation+1) || ((currentStation == 0) && (lastaddStation == MAX_WORK_FOR_STATION_THREAD-1))){
			// Thread�� �����ϴ� workList�� �� ����� �߿��ϴ� !!! �켱 ������ ThreadPool�� �߰��Ѱ��� �ٽ� ���� �����ؾ��Ѵ�.
			if (ThreadPool_Month.dwLastaddWork == 0) ThreadPool_Month.dwLastaddWork = MAX_WORK_MONTH - 1;
			else ThreadPool_Month.dwLastaddWork--;
			LeaveCriticalSection(&critical_section_month);
			//Sleep(1000);
			continue;
		}
		workList_month->workList[workList_month->byteLastAddWork].lpCriticalSection = lpCriticalSection;	// result_station�� ������ �� ���
		workList_month->workList[workList_month->byteLastAddWork].month = (BYTE) month;
		workList_month->workList[workList_month->byteLastAddWork].year = year;
		workList_month->workList[workList_month->byteLastAddWork].result_station = lpResult;
		strncpy_s(workList_month->workList[workList_month->byteLastAddWork].station_name, sizeof(workList_month->workList[workList_month->byteLastAddWork].station_name),
			station_name, strlen(station_name));
		strncpy_s(workList_month->workList[workList_month->byteLastAddWork].table_name, sizeof(workList_month->workList[workList_month->byteLastAddWork].table_name),
			table_name, strlen(table_name));
		workList_month->byteLastAddWork++;
		if (workList_month->byteLastAddWork == MAX_WORK_FOR_STATION_THREAD)workList_month->byteLastAddWork = 0;

		LeaveCriticalSection(lpCriticalSection);
	//	LeaveCriticalSection(&critical_section_month);
		break;
	}
	//// work �߰� ��Ų�ڿ� SetEvent�� ���ؼ� �����带 ����� �ڵ� !! ����� ������ Ż���� �� ���� 
	for (DWORD i = 0; i < MAX_WORK_MONTH; i++) SetEvent(ThreadPool_Month.hEventList[i]);
	LeaveCriticalSection(&critical_section_month);
}
/*
	GetWorkMonth_Idx
	<��>�� �м��ϴ� ������ Ǯ�� worklist�� �����Ͽ� ���� ó���ؾ� �ϴ� <��>�������� index�� �޾ƿ´�
	�׷��� 
*/
void GetWorkMonth(Work_For_Month *lpWork, DWORD *index1) {
	DWORD index = 0, current, lastadd;
	BYTE byteCurrent, byteLastadd;
	WorkList_For_Month *workList_month;
	CRITICAL_SECTION *lpCriticalSection = NULL;
	while (1) {
		EnterCriticalSection(&critical_section_month);
		current = ThreadPool_Month.dwCrrentWork;
		lastadd = ThreadPool_Month.dwLastaddWork;
		if ( (current +1 == lastadd) || ( (current == MAX_WORK_MONTH-1) && (lastadd == 0) ) ) {		// ����ִ� ���
			LeaveCriticalSection(&critical_section_month);
			continue;
		}
		if ((ThreadPool_Month.dwCrrentWork + 1) == MAX_WORK_MONTH) {
			ThreadPool_Month.dwCrrentWork = 0;
			index = ThreadPool_Month.dwWorkList[ThreadPool_Month.dwCrrentWork];
		} 
		else {
			ThreadPool_Month.dwCrrentWork++;
			index = ThreadPool_Month.dwWorkList[ThreadPool_Month.dwCrrentWork];
		}

		// <��> Thread WorkList�� �����ϱ� ���Ͽ� worklist �ּҿ� �ش� �������� criticalSection�ּ� �޾ƿ�
		EnterCriticalSection(&criticalSection_Thread_station);
		workList_month = threadPool_Station.threadStationInfo[index].lpWorkList_Month;
		if (workList_month == NULL) {
			if (ThreadPool_Month.dwCrrentWork == 0) ThreadPool_Month.dwCrrentWork = MAX_WORK_MONTH - 1;
			else ThreadPool_Month.dwCrrentWork--;
			LeaveCriticalSection(&criticalSection_Thread_station);
			LeaveCriticalSection(&critical_section_month);
			continue;
		}
		lpCriticalSection = &(threadPool_Station.threadStationInfo[index].critical_section_for_THREAD);
		LeaveCriticalSection(&criticalSection_Thread_station);
		// <��> Thread�� �����ϴ� WorkList�� ����
		EnterCriticalSection(lpCriticalSection);
		byteCurrent = workList_month->byteCurrentWork;
		byteLastadd = workList_month->byteLastAddWork;
		// workList is Empty !!!
		if ((byteCurrent+1 == byteLastadd) || ((byteCurrent+1 == MAX_WORK_FOR_STATION_THREAD) && (byteLastadd == 0))) {	
			if (ThreadPool_Month.dwCrrentWork == 0) ThreadPool_Month.dwCrrentWork = MAX_WORK_MONTH - 1;
			else ThreadPool_Month.dwCrrentWork--;
			LeaveCriticalSection(lpCriticalSection);
			LeaveCriticalSection(&critical_section_month);
			continue;
		}
		// workList is not Empty !!
		if(byteCurrent +1 == MAX_WORK_FOR_STATION_THREAD) workList_month->byteCurrentWork = 0; // ȯ�� !! 
		else workList_month->byteCurrentWork++;
		byteCurrent = workList_month->byteCurrentWork;

		lpWork->lpCriticalSection = lpCriticalSection;
		lpWork->month = workList_month->workList[byteCurrent].month;
		lpWork->result_station = workList_month->workList[byteCurrent].result_station;
		strncpy_s(lpWork->station_name, sizeof(lpWork->station_name), workList_month->workList[byteCurrent].station_name, strlen(workList_month->workList[byteCurrent].station_name));
		strncpy_s(lpWork->table_name, sizeof(lpWork->table_name), workList_month->workList[byteCurrent].table_name, strlen(workList_month->workList[byteCurrent].table_name));
		lpWork->year = workList_month->workList[byteCurrent].year;
	
		LeaveCriticalSection(lpCriticalSection);
		LeaveCriticalSection(&critical_section_month);
		*index1 = index;
		return ;
	}
	// ���� �ޱ� �������� Ż���� �� ���� !!
	return ;
}
/*
	ThreadProc_Month
	<��>�� �м��ϴ� ������ Ǯ���� ������ �����尡 ó������ �����ϴ� main�Լ��̴� !!
*/
unsigned int WINAPI ThreadProc_Month(LPVOID lpparam) {
	Work_For_Month lpworkMonth;
	DWORD result = 0, index;
	HANDLE hEvent;
	WORK_MONTH work = analyze_station_month;

	EnterCriticalSection(&critical_section_month);
	hEvent = ThreadPool_Month.hEventList[(DWORD)lpparam];
	LeaveCriticalSection(&critical_section_month);
	
	while (1) {
		WaitForSingleObject(hEvent, INFINITE);
		// hEvent => Set �Ǹ� ������ work�� �Ҵ�ް� �ȴ� !!
		EnterCriticalSection(&critical_section_month);
		ThreadPool_Month.arrThread[(DWORD)lpparam].working = WORKING;
		LeaveCriticalSection(&critical_section_month);
		// work�� �Ҵ�ޱ�
		GetWorkMonth(&lpworkMonth, &index);
		// work�� �Ҵ���� �� ����
		work(lpworkMonth.table_name,
			lpworkMonth.station_name,
			lpworkMonth.month,
			lpworkMonth.year,
			lpworkMonth.result_station,
			lpworkMonth.lpCriticalSection);
		// ������ ���� �� �ٽ� work�� �Ҵ� �ޱ� ���� NOT_WORK���°� �ȴ� !!
		EnterCriticalSection(&criticalSection_Thread_station);
		threadPool_Station.threadStationInfo[index].WorkingMonth++;
		LeaveCriticalSection(&criticalSection_Thread_station);
		EnterCriticalSection(&critical_section_month);
		ThreadPool_Month.arrThread[(DWORD)lpparam].working = NOT_WORK;
		LeaveCriticalSection(&critical_section_month);
	}
}
/*
	analyze_station_month
	<��>�� �м��ϴ� ������ Ǯ���� ���� �����ϴ� �Լ��̴�.
	���⼭�� 2015�⵵ �� ��� month�� ������� query�� ������ ������
	2013, 2014���� �� ��� month�� ���� query�� ���� ������ �Ѵ� !!


*/
void analyze_station_month(char *table_name, char *station_name, int month, DWORD year, station_info_result *lpResult, CRITICAL_SECTION *lpCriticalSection) {
	MYSQL *connection = NULL;
	MYSQL_RES *sql_res = NULL;
	MYSQL_ROW sql_row;
	DWORD dwConntionIdx, query_stat = 0;
	station_info_result temp_result;
	int day = 0, week_info, people = 0, year1;
	char query[150], *pos, temp[10], *a, *b;
	month++;

	Init_tempResult(&temp_result);
	query[0] = '\0';
	// Connection GET !!
	dwConntionIdx = GetConnectionIdx();
	connection = GetConnection(dwConntionIdx);
	// ������ query�� �����
	if (year == 2015) {
		sprintf_s(query, sizeof(query), "select ��¥, ��_�� from %s where ���� = '%s'", table_name, station_name);
	}
	else {
		if (month < 10) sprintf_s(query, sizeof(query),
			"select ��¥, �հ� from %s where ����='%s' and if(strcmp(left(��¥,2),'%d/')=0,'Y','N') = 'Y'",	table_name,station_name,month);
		else if((month > 9) && (month < 13)) sprintf_s(query, sizeof(query),
			"select ��¥, �հ� from %s where ����='%s' and if(strcmp(left(��¥,3),'%d/')=0,'Y','N') = 'Y';",table_name, station_name, month);
	}
	//
	query_stat = mysql_query(connection, query);
	if (query_stat != 0) {
		printf(" --- Mysql Query Error : %s < analyze station month >\n", mysql_error(connection));
		exit(1);
	}
	sql_res = mysql_store_result(connection);
	//
	while ((sql_row = mysql_fetch_row(sql_res)) != NULL) {
		people = 0;
		// Acquire day 
		day = GetDay_Analyze(sql_row[0], year);
		// ���� Or �ָ�
		week_info = GetWeekInfo(year, month, day);
		// people ���� ��´�. !!
		if ( (year == 2013) || (year == 2014) ) {		// 13, 14����
			people = atoi(sql_row[1]);
		}
		else if (year == 2015) {						// 15 ����
			if ((pos = strstr(sql_row[1], ",")) == NULL) {
				people = atoi(sql_row[1]);
			}
			else {
				temp[0] = '\0';
				strncpy_s(temp, sizeof(temp), sql_row[1], strlen(sql_row[1]));
				a = strtok_s(temp, ",", &b);
				people = (atoi(a) * 1000) + atoi(b);
			}
		}
		// �������� �ָ������� ���� ���� �ִ´� !! ==> �Լ��� �ϸ� �������� �Լ��� �ʹ� ���� ȣ��Ǿ� ������ ������ �� ����.

		year1 = year;
		month--;
		day--;
		year = year - 2013;
		
		switch (year1) {
			case 2013: {
				if (week_info == WEEK_DAYS) {
					if (date_time[year][month][day] == PM_SET) {
						temp_result.weekday_set_13 += people;
					}
					else if (date_time[year][month][day] == PM_CLEAR) {
						temp_result.weekday_clear_13 += people;
					}
				}
				else if (week_info == WEEK_END) {
					if (date_time[year][month][day] == PM_SET) {
						temp_result.weekend_set_13 += people;
					}
					else if (date_time[year][month][day] == PM_CLEAR) {
						temp_result.weekend_clear_13 += people;
					}
				}
				break;
			}
			case 2014: {
				if (week_info == WEEK_DAYS) {
					if (date_time[year][month][day] == PM_SET) {
						temp_result.weekday_set_14 += people;
					}
					else if (date_time[year][month][day] == PM_CLEAR) {
						temp_result.weekday_clear_14 += people;
					}
				}
				else if (week_info == WEEK_END) {
					if (date_time[year][month][day] == PM_SET) {
						temp_result.weekend_set_14 += people;
					}
					else if (date_time[year][month][day] == PM_CLEAR) {
						temp_result.weekend_clear_14 += people;
					}
				}
				break;
			}
			case 2015: {
				if (week_info == WEEK_DAYS) {
					if (date_time[year][month][day] == PM_SET) {
						temp_result.weekday_set_15 += people;
					}
					else if (date_time[year][month][day] == PM_CLEAR) {
						temp_result.weekday_clear_15 += people;
					}
				}
				else if (week_info == WEEK_END) {
					if (date_time[year][month][day] == PM_SET) {
						temp_result.weekend_set_15 += people;
					}
					else if (date_time[year][month][day] == PM_CLEAR) {
						temp_result.weekend_clear_15 += people;
					}
				}
				break;
			}
		}

		year = year + 2013;
		day++;
		month++;
	}
	// ���� ��� �־����� ���� lpResult�� temp_result�� ���� �־��ش� +=������Ѵ�.
	EnterCriticalSection(lpCriticalSection);
	InsertTempResult(lpResult, &temp_result);
	LeaveCriticalSection(lpCriticalSection);
	// Connection Return !!
	mysql_free_result(sql_res);
	ReturnConnection(dwConntionIdx);
}
/*
	Insert_pm_data
	���α׷� ���� : pm Set OR Clear �Ǵ� ���� �ش翬���� �����̳� �ִ��� ���... ���⿡�ٰ� �������� �ٸ����� �ϴ°� �� ������ �����ѵ�...
*/
void Insert_pm_data(station_info_result *result) {
	int year1, year, month, day, week_info;

	for (year = 0; year < YEAR;year++) {
		year1 = year + 2013;
		for (month = 0; month < MONTH;month++) {
			for (day = 0; day < DAY;day++) {

				week_info = GetWeekInfo(year, month, day);

				switch (year1) {
				case 2013: {
					if (week_info == WEEK_DAYS) {
						if (date_time[year][month][day] == PM_SET) {
							result->weekday_pm_set_13 += 1;
						}
						else if (date_time[year][month][day] == PM_CLEAR) {
							result->weekday_pm_clear_13 += 1;
						}
					}
					else if (week_info == WEEK_END) {
						if (date_time[year][month][day] == PM_SET) {
							result->weekend_pm_set_13 += 1;
						}
						else if (date_time[year][month][day] == PM_CLEAR) {
							result->weekend_pm_clear_13 += 1;
						}
					}
					break;
				}
				case 2014: {
					if (week_info == WEEK_DAYS) {
						if (date_time[year][month][day] == PM_SET) {
							result->weekday_pm_set_14 += 1;
						}
						else if (date_time[year][month][day] == PM_CLEAR) {
							result->weekday_pm_clear_14 += 1;
						}
					}
					else if (week_info == WEEK_END) {
						if (date_time[year][month][day] == PM_SET) {
							result->weekend_pm_set_14 += 1;
						}
						else if (date_time[year][month][day] == PM_CLEAR) {
							result->weekend_pm_clear_14 += 1;
						}
					}
					break;
				}
				case 2015: {
					if (week_info == WEEK_DAYS) {
						if (date_time[year][month][day] == PM_SET) {
							result->weekday_pm_set_15 += 1;
						}
						else if (date_time[year][month][day] == PM_CLEAR) {
							result->weekday_pm_clear_15 += 1;
						}
					}
					else if (week_info == WEEK_END) {
						if (date_time[year][month][day] == PM_SET) {
							result->weekend_pm_set_15 += 1;
						}
						else if (date_time[year][month][day] == PM_CLEAR) {
							result->weekend_pm_clear_15 += 1;
						}
					}
					break;
				}
				}

			}
		}
	}

	return;
}

/*
	void InsertTempResult(station_info_result *result, station_info_result *temp_result) 
	���� �м��ϴ� �������� �ش翪�� ���� �м������ �����ϴ� ����ü�� �м��� ���� ����
*/
void InsertTempResult(station_info_result *result, station_info_result *temp_result) {
	
	result->weekday_clear_13 += temp_result->weekday_clear_13;
	result->weekday_pm_clear_13 += temp_result->weekday_pm_clear_13;
	result->weekday_pm_set_13 += temp_result->weekday_pm_set_13;
	result->weekday_set_13 += temp_result->weekday_set_13;
	result->weekend_clear_13 += temp_result->weekend_clear_13;
	result->weekend_pm_clear_13 += temp_result->weekend_pm_clear_13;
	result->weekend_pm_set_13 += temp_result->weekend_pm_set_13;
	result->weekend_set_13 += temp_result->weekend_set_13;

	result->weekday_clear_14 += temp_result->weekday_clear_14;
	result->weekday_pm_clear_14 += temp_result->weekday_pm_clear_14;
	result->weekday_pm_set_14 += temp_result->weekday_pm_set_14;
	result->weekday_set_14 += temp_result->weekday_set_14;
	result->weekend_clear_14 += temp_result->weekend_clear_14;
	result->weekend_pm_clear_14 += temp_result->weekend_pm_clear_14;
	result->weekend_pm_set_14 += temp_result->weekend_pm_set_14;
	result->weekend_set_14 += temp_result->weekend_set_14;

	result->weekday_clear_15 += temp_result->weekday_clear_15;
	result->weekday_pm_clear_15 += temp_result->weekday_pm_clear_15;
	result->weekday_pm_set_15 += temp_result->weekday_pm_set_15;
	result->weekday_set_15 += temp_result->weekday_set_15;
	result->weekend_clear_15 += temp_result->weekend_clear_15;
	result->weekend_pm_clear_15 += temp_result->weekend_pm_clear_15;
	result->weekend_pm_set_15 += temp_result->weekend_pm_set_15;
	result->weekend_set_15 += temp_result->weekend_set_15;

}
/*
	int GetDay_Analyze(char *date, int year);
	��¥ �������� 'day' ���� �����Ͽ� int������ return�Ѵ� !!
*/
int GetDay_Analyze(char *date, int year) {
	char temp[15], *context, *token;
	temp[0] = '\0';
	// date => temp ����
	strncpy_s(temp, sizeof(temp), date, strlen(date));
	// 2013, 2014 ���� �� ���  <<< 1/1(��)/2013 >>>
	if (( year == 2013 ) || ( year == 2014 )) {
		token = strtok_s(temp, "/", &context);
		token = strtok_s(NULL, "/", &context);
		return atoi(token);
	}
	// 2015 ���� �� ���  <<< 2015-11-01 >>>
	if ( year == 2015 ) {
		token = strtok_s(temp, "-", &context);
		token = strtok_s(NULL, "-", &context);
		return atoi(context);
	}


}
/*
	int GetWeekInfo(int year, int month, int day);
*/
int GetWeekInfo(int year, int month, int day) {
	year = year - 2013;
	month--;
	
	int result_day;

	result_day = ((day % 7) + start_day[year][month]);
	if (result_day > 7) result_day = result_day % 7;
	if (result_day == 0) result_day += 7;
	if (result_day > 7 && result_day < 1) {
		printf("get_weekday_weekend() : ������ !! \n");
		exit(1);
	}
	if (result_day > 5) {
		return WEEK_END;
	}
	else {
		return WEEK_DAYS;
	}
}
/*
	void Init_tempResult(station_info_result *temp)
	���� �м��ϴ� ��������� ���� ����ü�� �ʱ�ȭ�ϴ� �Լ�
*/
void Init_tempResult(station_info_result *temp) {

	temp->weekday_set_15 = 0;
	temp->weekday_clear_15 = 0;
	temp->weekend_set_15 = 0;
	temp->weekend_clear_15 = 0;
	temp->weekday_set_14 = 0;
	temp->weekday_clear_14 = 0;
	temp->weekend_set_14 = 0;
	temp->weekend_clear_14 = 0;
	temp->weekday_set_13 = 0;
	temp->weekday_clear_13 = 0;
	temp->weekend_set_13 = 0;
	temp->weekend_clear_13 = 0;

	temp->weekday_pm_set_15 = 0;
	temp->weekday_pm_clear_15 = 0;
	temp->weekend_pm_set_15 = 0;
	temp->weekend_pm_clear_15 = 0;
	temp->weekday_pm_set_14 = 0;
	temp->weekday_pm_clear_14 = 0;
	temp->weekend_pm_set_14 = 0;
	temp->weekend_pm_clear_14 = 0;
	temp->weekday_pm_set_13 = 0;
	temp->weekday_pm_clear_13 = 0;
	temp->weekend_pm_set_13 = 0;
	temp->weekend_pm_clear_13 = 0;
	return;
}


/*
	void ClearThreadPool_Month();
	Ŀ�ؼ� Ǯ�� ������ ��� Ŀ�ο�����Ʈ ��ȯ�մϴ�. !!
*/
void ClearThreadPool_Month() {
	for (DWORD i = 0; i < MAX_THREAD_MONTH; i++) {
		CloseHandle(ThreadPool_Month.hEventList[i]);
	}
	DeleteCriticalSection(&critical_section_month);
	return;
}

////------------------------------------------------------------------////

////------------------------------------------------------------------////

////------------------------------------------------------------------////