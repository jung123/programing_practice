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
	pm 관련 부분
*/
#define YEAR 3
#define MONTH 12
#define DAY 31
#define PM_CHECK 2
#define PM_WARN 120
#define PM_SET 'O'
#define PM_CLEAR 'X'
// 변수
char date_time[YEAR][MONTH][DAY];
CRITICAL_SECTION criticalSection_DateTime;
// 함수
int get_day(char *date);
int get_month(char *date);
void create_pm_list(char *table_name, int year);
void pm_check();
void InitDateTime();

/*
Thread Pool 관련 부분 <역>
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

// ***********************<월>분석하는 쓰레드에게 필요한 선언 

typedef struct _Work_For_Month {
	char table_name[25];
	char station_name[25];
	BYTE month;
	DWORD year;
	station_info_result *result_station;
	CRITICAL_SECTION *lpCriticalSection;
} Work_For_Month;

typedef struct __WorkList_For_Month {						// 하나를 malloc 할당한다 !!
	Work_For_Month workList[MAX_WORK_FOR_STATION_THREAD];
	BYTE byteCurrentWork;
	BYTE byteLastAddWork;
} WorkList_For_Month;
// ********************************************************


typedef void(*threadStationWork)(char *, int);		// routine에서 처리하는 work

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
// 변수
threadPoolStation threadPool_Station;
CRITICAL_SECTION criticalSection_Thread_station;
char *station_info[] = { "서울역(150)","시청(151)","종각(152)","종로3가(153)","종로5가(154)","동대문(155)","신설동(156)","제기동(157)",
"청량리(지하)(158)","동묘앞(159)","시청(201)","을지로입구(202)","을지로3가(203)","을지로4가(204)","동대문역사문화공원(205)","신당(206)",
"상왕십리(207)","왕십리(성동구청)(208)","한양대(209)","뚝섬(210)","성수(211)","건대입구(212)","구의(213)","강변(214)","잠실나루(215)",
"잠실(216)","신천(217)","종합운동장(218)","삼성(219)","선릉(220)","역삼(221)","강남(222)","교대(223)","서초(224)","방배(225)","사당(226)",
"낙성대(227)","서울대입구(228)","봉천(229)","신림(230)","신대방(231)","구로디지털단지(232)","대림(233)","신도림(234)","문래(235)",
"영등포구청(236)","당산(237)","합정(238)","홍대입구(239)","신촌(240)","이대(241)","아현(242)","충정로(243)","용답(244)","신답(245)",
"신설동(246)","도림천(247)","양천구청(248)","신정네거리(249)","용두(250)",
"지축(309)","구파발(310)","연신내(311)","불광(312)","녹번(313)","홍제(314)",
"무악재(315)","독립문(316)","경복궁(317)","안국(318)","종로3가(319)",
"을지로3가(320)","동대입구(322)","약수(323)","금호(324)","옥수(325)",
"압구정(326)","신사(327)","잠원(328)","고속터미널(329)","교대(330)",
"남부터미널(331)","양재(332)","매봉(333)","도곡(334)","대치(335)","학여울(336)",
"대청(337)","일원(338)","수서(339)","가락시장(340)","경찰병원(341)","오금(342)"
};

// 함수  makeThreadPool, addWork, GetWork, ClearThreadPool
unsigned int WINAPI ThreadProcForStation(LPVOID lpparam);		// <역>을 처리하는 쓰레드의  StartRoutine !
void MakeThreadPool_station(void);
void AddWork_Station(char *station_name);	// 인자를 받은 후 쓰레드 풀의 worklist에 넣어주자...<Main>
char *GetWork_Station();
void ClearThreadPool_Station();
void anlyze_Station(char *station_name, int index);
void Check_End_Thread_Station();
int GetYear_Astation(char *table_name);
int GetMonth_Astation(char *table_name);
void Init_WorkList_For_Month(WorkList_For_Month *workList_month);
/*
Thread Pool 관련 부분 <월>
*/
#define MAX_THREAD_MONTH 40
#define MAX_WORK_MONTH 300

typedef void(*WORK_MONTH)(char *table_name, char *station_name, int month,DWORD year, station_info_result *lpResult, CRITICAL_SECTION *lpCriticalSection);	// 월을 처리하는 쓰레드가 
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
// 변수
CRITICAL_SECTION critical_section_month;
MonthThreadPool ThreadPool_Month;

int start_day[3][12] = { { 1,4,4,0,2,5,0,3,6,1,4,6 },{ 2,5,5,1,3,6,1,4,0,2,5,0 },{ 3,6,6,2,4,0,2,5,1,3,6,1 } };
// 함수
void MakeThreadPool_Month(void);
void AddWork_Month(int index, char *table_name, char *station_name, int month, int year, station_info_result *lpResult, CRITICAL_SECTION *lpCriticalSection, WorkList_For_Month *workList_month);		// <역>분석하는 쓰레드 자신의 index

void GetWorkMonth(Work_For_Month *lpWork, DWORD index);		// 어떤 쓰레드에 접근해야대는지 중요
unsigned int WINAPI ThreadProc_Month(LPVOID lpparam);
void analyze_station_month(char *table_name, char *station_name, int month, DWORD year, station_info_result *lpResult, CRITICAL_SECTION *lpCriticalSection);
void ClearThreadPool_Month();
/*
	Connection Pool 관련 부분
	dwConnectionIdx 를 얻어서 lpConnection을 얻은 후
	dwConnectionIdx을 인자로 해당 커넥션 반환해주는 방법으로 구현
	마지막으로 모든 커넥션 close 해준다 !!.
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

// 변수
CRITICAL_SECTION criticalSectionForDBconnector;
ConnectionPool connection_Pool;
// 함수
void MakeConnectionPool();
MYSQL *GetConnection(DWORD index);
DWORD GetConnectionIdx();
void ReturnConnection(DWORD index);
void ClearConnections();

/*
	함수
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
	MakeConnectionPool();							// 커넥션 풀 생성
	MakeThreadPool_station();						// <역> 분석하는 쓰레드 풀 생성
	create_table();								// 결과정보 담을 수 있는 테이블생성

	pm_check();										// pm 관련 데이터 생성
	for (DWORD i = 0; i < YEAR; i++) {
		printf("%d년\n", i + 2013);
		for (DWORD j = 0; j < MONTH; j++) {
			for (DWORD z = 0; z < DAY; z++) {
				printf("%c ",date_time[i][j][z]);
			}
			printf("%d월\n", j+1);
		}
	}

	MakeThreadPool_Month();							// <월> 분석하는 쓰레드 풀 생성
	for (DWORD i = 0; i < 92; i++) {				// <역> 분석하는 쓰레드 풀에 work 추가한다. !
		AddWork_Station(station_info[i]);
	}

	printf("*************************************\n\n Add Work End !! \n\n***************************************\n\n");
	Check_End_Thread_Station();						// <역> 분석하는 쓰레드 풀이 모든일을 다 처리했는지 검사

	DeleteCriticalSection(&criticalSection_DateTime);
	ClearThreadPool_Station();						// 쓰레드 풀에 생성된 커널 오브젝트 반납 <이벤트 오브젝트>
	ClearThreadPool_Month();
	ClearConnections();								// 커넥션 끊기

	end = clock();
	runtime = (float)(end - start) / CLOCKS_PER_SEC;
	printf(" \n\n ******************************************************************************************\n");
	printf("             실행시간  =  %d분 %d초\n", (int)runtime/60,(int)runtime%60);
	return 0;
}
/*
	error(char *meesage)
	에러출력용 함수
*/
void error(char *message) {
	fprintf_s(stderr, message);
	exit(1);
}
/*
	print_message(char *message)
	메세지 출력용 함수 !!
*/
void print_message(char *message) {
	fprintf_s(stdout, message);
	return;
}
/*
	void create_table();
	<역>을 분석한 결과정보를 담을 테이블 생성하는 함수
*/
void create_table() {
	MYSQL *connection = NULL;
	DWORD dwConnectionIdx, query_stat = 0;

	dwConnectionIdx = GetConnectionIdx();
	connection = GetConnection(dwConnectionIdx);

	// 테이블이 존재하면 지우고 다시 만든다 !!

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
	connectionpool 초기화하는 함수
*/
void MakeConnectionPool() {
	DWORD i = 0;
	InitializeCriticalSection(&criticalSectionForDBconnector);		// CriticalSection 초기화 !!
	EnterCriticalSection(&criticalSectionForDBconnector);
	for (i = 0; i < CONNECTION_MAX; i++) {
		mysql_init(&connection_Pool.db_connect_pool[i].conn);	// mysql 연동 전 초기회
		mysql_options(&connection_Pool.db_connect_pool[i].conn, MYSQL_SET_CHARSET_NAME, "euckr");		// 한글 나올 수 있게 세팅
		mysql_options(&connection_Pool.db_connect_pool[i].conn, MYSQL_INIT_COMMAND, "SET NAMES euckr");

		connection_Pool.db_connect_pool[i].connection = mysql_real_connect(&connection_Pool.db_connect_pool[i].conn, DB_HOST, DB_USER, DB_PASSWD, DB_NAME, 3306, (char *)NULL, 0);
		if (connection_Pool.db_connect_pool[i].connection == NULL) error("Connection INIT ERROR !! <MakeConnectionPool>\n ");
		connection_Pool.working[i] = NOT_WORK;
	}
	LeaveCriticalSection(&criticalSectionForDBconnector);
}
/*
	MYSQL *GetConnection();
	Connection Pool에서 할당되지 않은 Connection의 index를 return해준다.
	
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
				return i;												// 주소값을 return !!
			}
			i++;
		}
		LeaveCriticalSection(&criticalSectionForDBconnector);
		print_message("Connection FULL!! \n");
		Sleep(2000);																// 쓰레드 풀에 connection 얻는곳에서 block상태를 표현 후 blocking된다음
		EnterCriticalSection(&criticalSectionForDBconnector);						// ReturnConnection에서 connection을 기다리는 쓰레드의 이벤트를 세트해주는 형식으로 만들려다가
																					// 커넥션 수와 쓰레드 수의 관계와 쓰레드가 처리하는 일을 생각해본 결과 2초정도 Sleep해줘도 
																					// Connection을 얻을 수 있다는 결론을 내림 !
	}
}
/*
	MYSQL *GetConnection(DWORD index);
	Connection Pool에서 index에 위치해 있는 connection을 return해주는 함수
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
	Connection 반납 !!
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
	모든 connection을 Close한다.
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
	Date_time 전역변수 초기화하는 함수
*/

void InitDateTime() {
	InitializeCriticalSection(&criticalSection_DateTime);		// dateTime용 크리티컬섹션 초기화 !!
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
	- pm_check() :	 date_time[][][] 배열에 날짜별 pm 지수를 세팅하는 함수이다.
	create_pm_list 함수를 호출하게 된다.
*/

void pm_check() {
	MYSQL *connection;
	DWORD dwConnectionIdx = 0;
	MYSQL_RES *sql_table_result; // show tables 결과 
	MYSQL_ROW sql_table_row;	// table fetch 결과

	dwConnectionIdx = GetConnectionIdx();
	connection = GetConnection(dwConnectionIdx);

	int query_stat;
	int n = 0;

	char selectQuery[100] = "select * from ";	// 쿼리를 날린 후에 새로운 쿼리보내려면 버퍼초기화가 필요하다 !

												// table 가져옴
	query_stat = mysql_query(connection, "show tables");
	if (query_stat != 0) error("mysql query error : %s\n");

	sql_table_result = mysql_store_result(connection);
	while ((sql_table_row = mysql_fetch_row(sql_table_result)) != NULL) {	// table 정보 모두 받아온다 
																			// pm테이블만 가공 다른거는 그냥 넘김
		char *pos;
		char *pos1;
		if ((pos = strstr(sql_table_row[0], "pm")) != NULL) {
			// pm table 중 연도별로 따로 뺀다.!!
			if ((pos1 = strstr(sql_table_row[0], "13")) != NULL) {			// 13년도 pm table인경우
				create_pm_list(sql_table_row[0], 13);
			}
			else if ((pos1 = strstr(sql_table_row[0], "14")) != NULL) {		// 14년도 pm table인경우 
				create_pm_list(sql_table_row[0], 14);
			}
			else if ((pos1 = strstr(sql_table_row[0], "15")) != NULL) {		// 15년도 pm table인 경우
				create_pm_list(sql_table_row[0], 15);
			}
		}
	} // PM table에 쿼리를 다 날림 !!

	mysql_free_result(sql_table_result);	// 결과정보 프리
	ReturnConnection(dwConnectionIdx);
}
/*
	create_pm_list() : 분석할 테이블 이름과 연도정보를 바탕으로 해당년도의 일별 pm지수를
	date_time[][][]에 최종적으로 저장하는 함수
*/

void create_pm_list(char *table_name, int year) {
	MYSQL *connection;
	DWORD dwConnectionIdx;
	MYSQL_RES *select_res;
	MYSQL_ROW select_row;

	dwConnectionIdx = GetConnectionIdx();				// 여기서는 커넥션풀이 비지 않았을 확률이 높으므로 대기하는 코드를 안넣음
	connection = GetConnection(dwConnectionIdx);
	char select_query[50] = "select ";
	char *select_query00 = "select * from ";		// + table_name

	int query_stat;		// 쿼리문의 반환값 담는 변수와 strstr 반환값 담는 변수
	int j, pm_time, pm = 0;		// pm지수 가공시 필요한 변수들
	int month, day, pm_sum;				// 해당날짜의 month와 day를 받아와서 return하는 함수를 호출
										/*
										쿼리 조합 pm 테이블 조사
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
			여기서 select_row[1]의 날짜를 month day로 받아오는 함수필요
		*/
		month = get_month(select_row[1]);
		day = get_day(select_row[1]);
		pm = 0;
		for (j = 2; j < 27; j++) {		// 해당 날의 pm지수 측정
			if (select_row[j] != NULL) {		// 해당시간에 측정이 안된경우
				pm_time = atoi(select_row[j]);
				if (pm_time > PM_WARN) {
					pm++;
				}
			}
		}
		//
		if (pm >= PM_CHECK) {				// pm
			date_time[year][month][day] = PM_SET;		// 일별 pm 세팅 or clear 확인
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
	month 정보 가져오기 !!
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
	day 정보 가져오기 !!
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
	<역>을 분석하는 쓰레드의 main함수
	여기서 분석할 역명을 대기(work대기)
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
	<역>을 분석하는 쓰레드풀을 만든다. !!
	쓰래드 생성, Event(자동, non-signaled)
*/
void MakeThreadPool_station(void) {
	InitializeCriticalSection(&criticalSection_Thread_station);			// 쓰레드 풀 임계영역 설정

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
		InitializeCriticalSection(&threadPool_Station.threadStationInfo[i].critical_section_for_THREAD);	// Thread가 관리하는 critical Section INIT !
		threadPool_Station.dwThreadIdx++;
		threadPool_Station.threadStationInfo[i].WorkingMonth = 0; // 나중에 35가되면 자신이 주문한 일을 모두 끝낸것이다.
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
	WorkList를 환형으로 만든다. 꽉차있으면 waiting한다. !! <무엇으로 ???><어떤방식으로 대기할지도 생각해야한다 !>
	쓰레드 풀의 worklist에 전역변수 문자열의 주소를 넣어준다 !
*/
void AddWork_Station(char *station_name) {
	DWORD current, lastadd, i=0;
	EnterCriticalSection(&criticalSection_Thread_station);			// critical section Start !
	current = threadPool_Station.dwWorkCuurent;
	lastadd = threadPool_Station.dwWorkLastAdded;
	while (1) {		// worklist에 일을 넣어줄 때
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
	while(1){	// blocking 되어 있는 쓰레드를 깨워서 일을 하게하는 역할
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
	work리스트에 존재하는 처리할 역명의 주소를 return해주는 함수이다.
	return한 뒤에는 CurrentworkIndex를 올려줘야한다. ! 그리고 해당 work부분은 '\0'으로 초기화
*/
char *GetWork_Station() {	
	DWORD current = 0, lastadd = 0;
	char *station_name = NULL;
	EnterCriticalSection(&criticalSection_Thread_station);
	current = threadPool_Station.dwWorkCuurent;
	lastadd = threadPool_Station.dwWorkLastAdded;
	if ((current + 1 == lastadd) || ((current == MAX_WORK_STATION) && (lastadd == 0))) {	// worklist 비어있는경우
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
	쓰레드 풀의 모든 커널오브젝트를 반환한다 !
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
	<역>을 처리하는 쓰레드 풀의 쓰레드가 역을 처리하닥 <월>에게 자신의 일을 맡기는
	workList를 초기화하는 함수이다
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
	해당 역의 13 ~ 15년도 유동인구 조사 !!
	date_time에 접근시 읽기만 하기 때문에 세마포어로 만드는 것이 낫을듯 하다.
	이 함수 안에서 월별로 쓰레드<월>에게 work를 주는 것을 만들거나 다른 함수 하나를 호출하도록 만들자 !!
	호선에 상관없이 해당역을 찾아야한다 !!
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
	// 동적할당 (heap)영역 => 쓰레드들이 접근할 수 있도록
	result_station = (station_info_result *)malloc(sizeof(station_info_result));
	if (result_station == NULL) error(" --- result station Malloc Fail !! \n");

	workList_month = (WorkList_For_Month *)malloc(sizeof(WorkList_For_Month));
	if (workList_month == NULL) error(" --- WorkList For Month Malloc Fail !! \n");
	Init_WorkList_For_Month(workList_month);
	// 결과정보담는 구조체 초기화 ==> '0'으로 만들기
	Init_tempResult(result_station);	
	//
	EnterCriticalSection(&criticalSection_Thread_station);						// 해당 역의 결과정보를 담기 위해 동기화가 필요하기 때문에
	lpCriticalSection = &threadPool_Station.threadStationInfo[index].critical_section_for_THREAD;
	threadPool_Station.threadStationInfo[index].lpWorkList_Month = workList_month;	// <역>분석 쓰레드의 workList ==> 월분석 쓰레드가 접근
	LeaveCriticalSection(&criticalSection_Thread_station);
	// 13 ~15 년도 pm 세팅된날과 안된날들의 갯수를 결과 구조제에 저장 // 구조체에 접근하는 쓰레드는 이것하나이므로 크리티컬섹션 할 필요 없음
	Insert_pm_data(result_station);
	
	temp1 = sprintf_s(query_buffer1, sizeof(query_buffer1), "show tables");
	if (temp1 == -1) error("Error String <Analyze Station !!>22\n");
	/*
		connection 맺기 -> query -> store -> free - Close 이 순서를 어기면
		Error가 발생 쿼리를 보내고 위의 순서를 안지켜야 한다면 하나의 커넥션이 더 필요하다. !!
		!!! 여기서 고려해야할 점 : 저장 후 커넥션 계속사용(저장을 해야함), 저장하지 않고 다른 커넥션을 맺은 후 커넥션 유지(커넥션 낭비될 수 있음)
	*/
	dwConnectionIdx = GetConnectionIdx();		//show tables 용 connect
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
	
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {				// "show tables" 쿼리를 보낸 후 하나씩 꺼내는 루틴
		if ((temp = strstr(sql_row[0], "line")) == NULL) continue;			// 지하철 테이블이 아닌 경우
		
		
		query_buffer[0] = '\0';												// buffer 초기화
		temp1 = sprintf_s(query_buffer, sizeof(query_buffer), "select * from %s where 역명 = '%s' limit 1", sql_row[0], station_name);
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
			여기에다가 이제 해당 역을 분석하는 코드를 작성하면댄다.. !!
			<월> 쓰레드 풀 호출 !!
			여기에서 커넥션을 너무 많이 사용하는 것 같기도 하다 !!
			!!!! 여기에서 연도정보를 얻을 수 있다 !!!!!
		*/
		Sleep(1000);
		year = GetYear_Astation(sql_row[0]);								// 연도정보 추출
		// Add Work !!!! <월> 분석하는 쓰레드풀의 worklist에
		// month정보는 i =0 -> 12로 ADD해주자 if year == 2015 => table 이름에서 월정보 때자 !!
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
	/// <역> 분석하는 쓰레드가 insert한 모든 work가 다 처리되었는지 확인
	Sleep(5000);
	while (1) {
		EnterCriticalSection(&criticalSection_Thread_station);
		if (threadPool_Station.threadStationInfo[index].WorkingMonth == number) {
			threadPool_Station.threadStationInfo[index].WorkingMonth = 0;
			LeaveCriticalSection(&criticalSection_Thread_station);
			break;
		}
		LeaveCriticalSection(&criticalSection_Thread_station);
		Sleep(5000);														// 될 때까지 blocking
	}

	/////////////////// 그냥확인 !!

	CheckResult(result_station, station_name);

	///////////////////
	

	/// 이제 자신이 insert한 모든 work가 처리된 상태이다 !! ==> 구조체에 담긴 정보를 query로 보내야한다 !!
	Excute_Query(result_station, station_name);
	/// 자신이 선언해놨던것들 모두 정리해야한다 !!
	
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

	printf(" ** station name : %s 분석결과 !!\n");
	printf(" ********** 13년도 **********\n\n");
	printf(" --- 평일 --- PM_SET 일 평균 : %d,  PM_CLEAR 일 평균 : %d\n", result->weekday_set_13 / result->weekday_pm_set_13, result->weekday_clear_13 / result->weekday_pm_clear_13);
	printf(" --- 주말 --- PM_SET 일 평균 : %d,  PM_CLEAR 일 평균 : %d\n\n", result->weekend_set_13 / result->weekend_pm_set_13, result->weekend_clear_13 / result->weekend_pm_clear_13);
	printf(" ********** 14년도 **********\n\n");
	printf(" --- 평일 --- PM_SET 일 평균 : %d,  PM_CLEAR 일 평균 : %d\n", result->weekday_set_14 / result->weekday_pm_set_14, result->weekday_clear_14 / result->weekday_pm_clear_14);
	printf(" --- 주말 --- PM_SET 일 평균 : %d,  PM_CLEAR 일 평균 : %d\n\n", result->weekend_set_14 / result->weekend_pm_set_14, result->weekend_clear_14 / result->weekend_pm_clear_14);
	printf(" ********** 15년도 **********\n\n");
	printf(" --- 평일 --- PM_SET 일 평균 : %d,  PM_CLEAR 일 평균 : %d\n", result->weekday_set_15 / result->weekday_pm_set_15, result->weekday_clear_15 / result->weekday_pm_clear_15);
	printf(" --- 주말 --- PM_SET 일 평균 : %d,  PM_CLEAR 일 평균 : %d\n\n", result->weekend_set_15 / result->weekend_pm_set_15, result->weekend_clear_15 / result->weekend_pm_clear_15);

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
	table 이름에서 연도정보 추출 !!
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
	<역>을 분석하는 쓰레드 풀의 모든 일들을 처리했고 모든 쓰레드가 일을 하지 않은 상태인 것을 확인한 후
	프로그램을 종료한다. !!
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
		if ((current + 1 == lastadd) || ((current == MAX_WORK_STATION - 1) && (lastadd == 0))) {	// worklist 비어있는경우
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
	<월>을 분석하는 쓰레드풀을 생성하는 것
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
	<역>을 분석하는 쓰레드 풀의 workList에 일을 추가시키는 것
	동작 1 : 자신의 index를 workList에 추가
	동작 2 : 자신이 관리하는 worklist에 일을 추가
	위의 동작은 원자성이 보장되어야한다 !!
	그런뒤 쓰레드를 깨워준다 !!
*/
void AddWork_Month(int index, char *table_name, char *station_name, int month,int year, station_info_result *lpResult, CRITICAL_SECTION *lpCriticalSection,
					WorkList_For_Month *workList_month) {
	DWORD current, lastadd;
	BYTE currentStation, lastaddStation, i =0;
	while (1) {
		EnterCriticalSection(&critical_section_month);
		current = ThreadPool_Month.dwCrrentWork;
		lastadd = ThreadPool_Month.dwLastaddWork;
		if ((lastadd+1 == current) || ((current == 0) && (lastadd == MAX_WORK_MONTH-1))) {						// 꽉 찬경우
			LeaveCriticalSection(&critical_section_month);
			Sleep(1000);
			continue;
		}
		ThreadPool_Month.dwWorkList[ThreadPool_Month.dwLastaddWork++] = index;
		if (ThreadPool_Month.dwLastaddWork == MAX_WORK_MONTH) ThreadPool_Month.dwLastaddWork = 0; // 환형 !

		EnterCriticalSection(lpCriticalSection);
		currentStation = workList_month->byteCurrentWork;
 		lastaddStation = workList_month->byteLastAddWork;
		if ((currentStation == lastaddStation+1) || ((currentStation == 0) && (lastaddStation == MAX_WORK_FOR_STATION_THREAD-1))){
			// Thread가 관리하는 workList가 꽉 찬경우 중요하다 !!! 우선 위에서 ThreadPool에 추가한것을 다시 원상 복구해야한다.
			if (ThreadPool_Month.dwLastaddWork == 0) ThreadPool_Month.dwLastaddWork = MAX_WORK_MONTH - 1;
			else ThreadPool_Month.dwLastaddWork--;
			LeaveCriticalSection(&critical_section_month);
			//Sleep(1000);
			continue;
		}
		workList_month->workList[workList_month->byteLastAddWork].lpCriticalSection = lpCriticalSection;	// result_station에 접근할 때 사용
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
	//// work 추가 시킨뒤에 SetEvent를 통해서 쓰레드를 깨우는 코드 !! 깨우기 전에는 탈출할 수 없다 
	for (DWORD i = 0; i < MAX_WORK_MONTH; i++) SetEvent(ThreadPool_Month.hEventList[i]);
	LeaveCriticalSection(&critical_section_month);
}
/*
	GetWorkMonth_Idx
	<월>을 분석하는 쓰레드 풀의 worklist에 접근하여 지금 처리해야 하는 <역>쓰레드의 index를 받아온다
	그런뒤 
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
		if ( (current +1 == lastadd) || ( (current == MAX_WORK_MONTH-1) && (lastadd == 0) ) ) {		// 비어있는 경우
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

		// <역> Thread WorkList에 접근하기 위하여 worklist 주소와 해당 쓰레드의 criticalSection주소 받아옴
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
		// <역> Thread가 관리하는 WorkList에 진입
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
		if(byteCurrent +1 == MAX_WORK_FOR_STATION_THREAD) workList_month->byteCurrentWork = 0; // 환형 !! 
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
	// 일을 받기 전까지는 탈출할 수 없다 !!
	return ;
}
/*
	ThreadProc_Month
	<월>을 분석하는 쓰레드 풀에서 생성된 쓰레드가 처음으로 시작하는 main함수이다 !!
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
		// hEvent => Set 되면 무조건 work를 할당받게 된다 !!
		EnterCriticalSection(&critical_section_month);
		ThreadPool_Month.arrThread[(DWORD)lpparam].working = WORKING;
		LeaveCriticalSection(&critical_section_month);
		// work를 할당받기
		GetWorkMonth(&lpworkMonth, &index);
		// work를 할당받은 후 실행
		work(lpworkMonth.table_name,
			lpworkMonth.station_name,
			lpworkMonth.month,
			lpworkMonth.year,
			lpworkMonth.result_station,
			lpworkMonth.lpCriticalSection);
		// 실행이 끝난 뒤 다시 work를 할당 받기 위해 NOT_WORK상태가 된다 !!
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
	<월>을 분석하는 쓰레드 풀에서 일을 수행하는 함수이다.
	여기서는 2015년도 인 경우 month와 상관없이 query를 보내도 되지만
	2013, 2014연도 인 경우 month에 따라서 query를 새로 만들어야 한다 !!


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
	// 연도별 query문 만들기
	if (year == 2015) {
		sprintf_s(query, sizeof(query), "select 날짜, 합_계 from %s where 역명 = '%s'", table_name, station_name);
	}
	else {
		if (month < 10) sprintf_s(query, sizeof(query),
			"select 날짜, 합계 from %s where 역명='%s' and if(strcmp(left(날짜,2),'%d/')=0,'Y','N') = 'Y'",	table_name,station_name,month);
		else if((month > 9) && (month < 13)) sprintf_s(query, sizeof(query),
			"select 날짜, 합계 from %s where 역명='%s' and if(strcmp(left(날짜,3),'%d/')=0,'Y','N') = 'Y';",table_name, station_name, month);
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
		// 평일 Or 주말
		week_info = GetWeekInfo(year, month, day);
		// people 값을 얻는다. !!
		if ( (year == 2013) || (year == 2014) ) {		// 13, 14연도
			people = atoi(sql_row[1]);
		}
		else if (year == 2015) {						// 15 연도
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
		// 평일인지 주말인지에 따라 값을 넣는다 !! ==> 함수로 하면 좋겠지만 함수가 너무 많이 호출되어 성능이 떨어질 것 같다.

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
	// 값을 모두 넣었으면 이제 lpResult에 temp_result의 값을 넣어준다 +=해줘야한다.
	EnterCriticalSection(lpCriticalSection);
	InsertTempResult(lpResult, &temp_result);
	LeaveCriticalSection(lpCriticalSection);
	// Connection Return !!
	mysql_free_result(sql_res);
	ReturnConnection(dwConntionIdx);
}
/*
	Insert_pm_data
	프로그램 설명 : pm Set OR Clear 되는 날이 해당연도에 몇이이나 있는지 계산... 여기에다가 하지말고 다른데서 하는게 저 좋을것 같긴한데...
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
	역을 분석하는 쓰레드의 해당역에 대한 분석결과를 저장하는 구조체에 분석한 것을 저장
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
	날짜 정보에서 'day' 정보 추출하여 int형으로 return한다 !!
*/
int GetDay_Analyze(char *date, int year) {
	char temp[15], *context, *token;
	temp[0] = '\0';
	// date => temp 복사
	strncpy_s(temp, sizeof(temp), date, strlen(date));
	// 2013, 2014 연도 인 경우  <<< 1/1(일)/2013 >>>
	if (( year == 2013 ) || ( year == 2014 )) {
		token = strtok_s(temp, "/", &context);
		token = strtok_s(NULL, "/", &context);
		return atoi(token);
	}
	// 2015 연도 인 경우  <<< 2015-11-01 >>>
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
		printf("get_weekday_weekend() : 계산오류 !! \n");
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
	역을 분석하는 결과정보를 담을 구조체를 초기화하는 함수
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
	커넥션 풀이 생성한 모든 커널오브젝트 반환합니다. !!
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