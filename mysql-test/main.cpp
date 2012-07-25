#include <stdio.h>
#include <mysql/mysql.h>
#include <stdlib.h>
#include <pthread.h>
#include <strings.h>

#include <iostream>
using namespace std;
int count;
char *host = "localhost";
char *user = "root";
char *passwd =  "admin";
char *db = "cgtree";

pthread_mutex_t lock;
void *thread(void *arg)
{
    while (1) {
        sleep(1);
        cout << count << endl;
    }
}
void *thread2(void *arg)
{
	MYSQL mysql;
	MYSQL_RES *result;
	MYSQL_ROW row;
	char sql[1000];
	//pthread_mutex_init(&lock, NULL);
	mysql_init(&mysql);
	if (mysql_real_connect(&mysql, host, user, passwd, db, 0, NULL, 0) == NULL) {
        cout << "mysql2 connect error" << endl;
        exit(1);
	}
	//while (1) {
        //bzero(sql, 1000);
        int num = rand() % 1000000;
        //cout << num << endl;
        //sprintf(sql, "%s%d", "select * from test where filename = ", num);
        sprintf(sql, "%s%d%s", "insert into test(filename) values(", num, ")");
        //cout << sql << endl;;
        if (mysql_query(&mysql, sql) != 0) {
            cout << "mysql query error" << endl;
            exit(1);
        }
/*
        if ((result = mysql_store_result(&mysql)) == NULL) {
            cout << "mysql store result error" << endl;
            exit(1);
        }*/
        /*
        while ((row = mysql_fetch_row(result))) {
            cout << row[0] << "   " << row[1] << endl;
        }*/
       // mysql_free_result(result);
		pthread_mutex_lock(&lock);
        count++;
		pthread_mutex_unlock(&lock);
	//}
}
int main()
{
	MYSQL mysql;
	MYSQL_RES *result;
	MYSQL_ROW row;
	pthread_t tid;
	char sql[1000];

    pthread_create(&tid, NULL, thread, NULL);
 	
	mysql_init(&mysql);
	if (mysql_real_connect(&mysql, host, user, passwd, db, 0, NULL, 0) == NULL) {
        cout << "mysql1 connect error" << endl;
        exit(1);
	}
//while (1) {
//	pthread_create(&tid, NULL, thread2, NULL);
//}
	while (1) {
        //bzero(sql, 1000);
        int num = rand() % 1000000;
        //cout << num << endl;
       // sprintf(sql, "%s%d", "select * from test where filename = ", num);
        sprintf(sql, "%s%d%s", "insert into test(filename) values(", num, ")");
        //cout << sql << endl;;
        if (mysql_query(&mysql, sql) != 0) {
            cout << "mysql query error" << endl;
            exit(1);
        }

       // if ((result = mysql_store_result(&mysql)) == NULL) {
       //     cout << "mysql store result error" << endl;
       //     exit(1);
       // }
        
       // while ((row = mysql_fetch_row(result))) {
       //     cout << row[0] << "   " << row[1] << endl;
       // }
       // mysql_free_result(result);
		//pthread_mutex_lock(&lock);
        count++;
	//	pthread_mutex_unlock(&lock);
	}
	mysql_close(&mysql);
	return 0;
}
