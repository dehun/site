# SQLite optimizations #
## Abstract ##
SQLite is a very simple database which is completly hosted inside of user application.
No additional services involved. It is very usefull when you want to use some sql but don't want to involve a monsters like mysql or postgresql. You can use slqite for a persistent data storage in your applications(especially if you want transactions).
## Issue ##
Our goal is to optimize sqlite for maximum performance and see what drawbacks we can get.
I will cover sqlite specific techniques here. They can be expanded on another databases also.
But I will not cover some generic things like index creation and so on.
## Methodology ##
We will use pragma setting of sqlite connections to set different modes and see how this will affect the performance of simple operations like insert, select, delete.
To measure performance I write a simple test application in C. You can find the full source code at the end of this article.
## Single transaction ##
[Transaction](http://en.wikipedia.org/wiki/Database_transaction) is atomic operation - either all actions in transaction are performed either none of them.
SQLite have implicit transactions - when you don't use transactions explicitly - sqlite will make a transaction per operation.

	INSERT INTO t (t1, t2) VALUES(1, 2)
	INSERT INTO t (t1, t2) VALUES(3, 4)

will be translated by sqlite into

    BEGIN
	INSERT INTO t (t1, t2) VALUES(1, 2)
	COMMIT
	BEGIN
	INSERT INTO t (t1, t2) VALUES(3, 4)
	COMMIT

So in case of batch of inserts we can optimize a tangible by using a single transaction for all inserts:

	BEGIN
	INSERT INTO t (t1, t2) VALUES(1, 2)
	INSERT INTO t (t1, t2) VALUES(3, 4)
	COMMIT

Lets examine the test run results:

	test: default_config
		total: 390267
		runtime: 1200
		ops per second: 325.222504
	test: 100_ops_in_transaction
		total: 4649100
		runtime: 1200
		ops per second: 3874.250000
	test: 1000_ops_in_transaction
		total: 10217000
		runtime: 1200
		ops per second: 8514.166992
	test: 10000_ops_in_transaction
		total: 11110943
		runtime: 1200
		ops per second: 9259.119141
	test: 10000000_ops_in_transaction
		total: 11278354
		runtime: 1200
		ops per second: 9398.627930

It shows that we can gain 30x performance improvement by batching operations in 1000 items transactions.
Further increase of operations done in single transactions give us no significant performance increase.

Also the very important notice is that disk IO is very intensive during default_config test. This is because after each insert the commit is occur and fsync with it.
By batching many operations into one we avoiding much of disk writes. Also this tells us why we got no performance increase after some number of operations in transaction - we just hit the upper bound of operation system buffer of file and increase of operations doesn't fits the buffer anymore.
## Journal ##
[Journal](http://en.wikipedia.org/wiki/Transaction_log) is a special set of data which is used to implement transactions mechanism.
SQLite supports different journal management approaches:

* no journal - journal is disabled
* delete - journal file is deleted each time new transaction is started
* truncate - journal file is truncated. Works faster on some platforms
* persist - journal file grows and grows to some limit
* memory - journal is stored in memory. One of the fastest ways
* WAL - another way of doing a journaling. Faster in most cases.

Journal is created in the same directory as a database.
For example we do

    cd ~/sqlite_test
    sqlite3 test.sqlite
    BEGIN;
    CREATE TABLE t (a, b ,c);

and in another terminal

    ls ~/sqlite_test

The results of ls command will be next:

* delete/truncate/persist:
>    test.sqlite  test.sqlite-journal
* memory:
>    test.sqlite
* WAL
>    test.sqlite  test.sqlite-shm  test.sqlite-wal


To turn a specific journal mode you should use next piece of code:

    PRAGMA journal_mode = DELETE | TRUNCATE | PERSIST | MEMORY | WAL | OFF


As for the performance:

	test: journal_off_config
		total: 2333511
		runtime: 1200
		ops per second: 1944.592529
	test: journal_delete_config
		total: 389034
		runtime: 1200
		ops per second: 324.195007
	test: journal_truncate_config
		total: 445043
		runtime: 1200
		ops per second: 370.869171
	test: journal_persist_config
		total: 597304
		runtime: 1200
		ops per second: 497.753326
	test: journal_memory_config
		total: 1898824
		runtime: 1200
		ops per second: 1582.353394
	test: journal_wal_config
		total: 4143846
		runtime: 1200
		ops per second: 3453.205078

We see that the most efficient option is either memory, off, or WAL.
The most preffered option is WAL because it have the most reliable implementation and have no fetures shrink impact like 'off'. With WAL we get a 10x performance boost.

## Synchronous OFF ##
By default sqlite use synchronous ON option. It means that it will be trying to keep file on disk to be synced during the changes as fast and reliable as possible.
This is done by calling fsync(fd). This function forces an OS to write all gathered cache for descriptor fd onto disk. In case of sqlite this leads to many small of chunks data written very often.
To avoid such behaviour we should change the synchronous to OFF

    PRAGMA synchronous = 0 | OFF | 1 | NORMAL | 2 | FULL;

Now the sqlite will use system OS buffers more efficiently - the OS will decide when to flush a buffers.
This will lead to most of operations to occure in memory which is much more faster.

The drawback of this solution is that we can get a corrupted sqlite database in case of system failure.
Quote from sqlite documentation:
> If the application running SQLite crashes, the data will be safe, but the database might become corrupted if the operating system crashes or the computer loses power before that data has been written to the disk surface. On the other hand, some operations are as much as 50 or more times faster with synchronous OFF.


Lets examine our performance gain with sycnhronous = OFF

	test: default_config
		total: 390267
		runtime: 1200
		ops per second: 325.222504

vs

    test: synchronous_off_config
		total: 12601839
		runtime: 1200
		ops per second: 10501.532227

We get a 30x performance gain by simple enabling this pragma. The tradeoff here is reliability.

## Exclusive Lock ##
    PRAGMA locking_mode = NORMAL | EXCLUSIVE;

The exclusive locking mode disables releasing of locks accuried for this database.
This can be used if we work with sqlite database from one thread and application.
Quote from documentation:

> There are three reasons to set the locking-mode to EXCLUSIVE.
>
>     The application wants to prevent other processes from accessing the database file.
>     The number of system calls for filesystem operations is reduced, possibly resulting in a small performance increase.
>     WAL databases can be accessed in EXCLUSIVE mode without the use of shared memory. (Additional information)

	test: exclusive_lock
		total: 980775
		runtime: 1200
		ops per second: 817.312500

We got here 2.5x performance gain. The drawback is not very painfull. So can be used for applications where realibility matters and only one application access sqlite database.

## Vacuum ##
Vacum is like a defragmentation of sqlite database. During the work you delete and add a new data.
But the file size can be much greater than the actually data size stored in database.
To avoid this we should use vacuum procedure.

I got no significant performance impact of vacuum. But I write this note as a note about failed attemt - the fail is also a result. So I tried to play with auto_vacuum settings.
auto_vacuum is feature developed to automaticaly compact the database during the work. If you want - you can disable the aut_vacuum feature and run VACUUM command explicitly.

    PRAGMA auto_vacuum = 0 | NONE | 1 | FULL | 2 | INCREMENTAL;

The test results:
	test: default_config
		total: 390267
		runtime: 1200
		ops per second: 325.222504

	test: vacuum_full_config
		total: 410505
		runtime: 1200
		ops per second: 342.087494

We see almost no performance impact. The default value of auto_vacuum is OFF.

## Conclusion ##
The main advantage we get using the methods where all operations are resolved in memory.
This can be done either by turning synchronous OFF or by making all operations in a single transaction.

## Full test results ##
	test: default_config
		total: 390267
		runtime: 1200
		ops per second: 325.222504
	test: 100_ops_in_transaction
		total: 4649100
		runtime: 1200
		ops per second: 3874.250000
	test: 1000_ops_in_transaction
		total: 10217000
		runtime: 1200
		ops per second: 8514.166992
	test: 10000_ops_in_transaction
		total: 11110943
		runtime: 1200
		ops per second: 9259.119141
	test: 10000000_ops_in_transaction
		total: 11278354
		runtime: 1200
		ops per second: 9398.627930
	test: synchronous_off_config
		total: 12601839
		runtime: 1200
		ops per second: 10501.532227
	test: journal_off_config
		total: 2333511
		runtime: 1200
		ops per second: 1944.592529
	test: journal_delete_config
		total: 389034
		runtime: 1200
		ops per second: 324.195007
	test: journal_truncate_config
		total: 445043
		runtime: 1200
		ops per second: 370.869171
	test: journal_persist_config
		total: 597304
		runtime: 1200
		ops per second: 497.753326
	test: journal_memory_config
		total: 1898824
		runtime: 1200
		ops per second: 1582.353394
	test: journal_wal_config
		total: 4143846
		runtime: 1200
		ops per second: 3453.205078
	test: journal_wal_config_autocheckpoint_off
		total: 1787602
		runtime: 1201
		ops per second: 1488.427979
	test: journal_wal_config_autocheckpoint_10000
		total: 3963780
		runtime: 1200
		ops per second: 3303.149902
	test: vacum_off_config
		total: 410505
		runtime: 1200
		ops per second: 342.087494
	test: exclusive_lock
		total: 980775
		runtime: 1200
		ops per second: 817.312500
	test: synchronouf_off_and_journal_off
		total: 22066307
		runtime: 1200
		ops per second: 18388.589844
	test: synchronouf_off_and_journal_off_and_vacuum_off
		total: 21721941
		runtime: 1200
		ops per second: 18101.617188
	test: synchronouf_off_and_journal_off_and_vacuum_off_and_exclusive_lock
		total: 26863458
		runtime: 1200
		ops per second: 22386.214844
	test: synchronouf_off_and_journal_off_and_vacuum_off_and_exclusive_lock_1000_ops_in_transaction
		total: 29370021
		runtime: 1200
		ops per second: 24475.017578
	test: synchronouf_off_and_journal_wal_and_vacuum_off_and_exclusive_lock_1000_ops_in_transaction
		total: 29514000
		runtime: 1201
		ops per second: 24574.521484

## Test source code ##
	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include <time.h>
	#include <sqlite3.h>

	#define CONFIG_DEFAULT "SELECT 1;"
	#define CONFIG_SYNCHRONOUS_OFF "PRAGMA synchronous=OFF;"
	#define CONFIG_JOURNAL_OFF "PRAGMA journal_mode = OFF;"
	#define CONFIG_JOURNAL_DELETE "PRAGMA journal_mode = DELETE;"
	#define CONFIG_JOURNAL_TRUNCATE "PRAGMA journal_mode = TRUNCATE;"
	#define CONFIG_JOURNAL_PERSIST "PRAGMA journal_mode = PERSIST;"
	#define CONFIG_JOURNAL_MEMORY "PRAGMA journal_mode = MEMORY;"
	#define CONFIG_JOURNAL_WAL "PRAGMA journal_mode = WAL;"
	#define CONFIG_JOURNAL_WAL_AUTOCHECKPOINT_OFF "PRAGMA journal_mode = WAL; PRAGMA wal_autocheckpoint = 0;"
	#define CONFIG_JOURNAL_WAL_AUTOCHECKPOINT_10000 "PRAGMA journal_mode = WAL; PRAGMA wal_autocheckpoint = 10000;"
	#define CONFIG_VACUUM_FULL "PRAGMA auto_vacum = FULL;"
	#define CONFIG_EXLUSIVE_LOCK "PRAGMA locking_mode = EXCLUSIVE;"


	#define TEST_RUN_TIME 10	  /* 20 minutes */
	#define TEST_INSERT_PERCENTAGE 60
	#define TEST_SELECT_PERCENTAGE 30
	#define TEST_DELETE_PERCENTAGE 10


	int callback(void *context, int argc, char **argv, char **columnNames) {
	  return SQLITE_OK;						/* do nothing */
	}


	#define CHECK_SQL_RESULT						\
	  if (ret != SQLITE_OK) {						\
		return (unsigned long)-1;					\
	  }


	#define DATABASE_FILE_NAME "write_test.sqlite"
	#define BUFFER_SIZE 1024

	unsigned long _test(time_t time_to_run, const char* config,
							 int ops_in_transaction, time_t *time_spent) {
	  time_t start_time;
	  char *error_message = NULL;
	  sqlite3* db;
	  char buff[BUFFER_SIZE];
	  int ret;

	  /* set start time */
	  start_time = time(NULL);
	  /* create database */
	  remove(DATABASE_FILE_NAME);
	  ret = sqlite3_open(DATABASE_FILE_NAME, &db);
	  CHECK_SQL_RESULT;
	  /* feed the config */
	  ret = sqlite3_exec(db, config, callback, 0, &error_message);
	  CHECK_SQL_RESULT;
	  /* create table */
	  ret = sqlite3_exec(db,
						 "CREATE TABLE t(id INTEGER PRIMARY KEY AUTOINCREMENT, t1, t2 , t3)",
						 callback, 0, &error_message);
	  CHECK_SQL_RESULT;
	  /* make inserts with random values during the time_to_run period of time*/
	  unsigned long inserts = 0;
	  unsigned long selects = 0;
	  unsigned long deletes = 0;

	  unsigned int already_done_ops_in_transaction = 0;

	  while (time(NULL) - start_time < time_to_run) { /* run for a time_to_run time */
		/* transactions logic */
		if (ops_in_transaction && (already_done_ops_in_transaction == 0)) {
		  sqlite3_exec(db, "BEGIN", callback, 0, &error_message);
		  CHECK_SQL_RESULT;
		}

		/* perform operations - insert/delete/select */
		int drop = rand() % 100;
		if (drop <= TEST_INSERT_PERCENTAGE) {
		  /* perform insert */
		  snprintf(buff, BUFFER_SIZE, "INSERT INTO t (t1, t2 ,t3) VALUES(%d, %d, %d);",
				   rand()%500, rand()%500, rand()%500);
		  sqlite3_exec(db, buff, callback, 0, &error_message);
		  CHECK_SQL_RESULT;
		  ++inserts;
		} else if ((drop > TEST_INSERT_PERCENTAGE) && (drop <= TEST_INSERT_PERCENTAGE + TEST_SELECT_PERCENTAGE)) {
		  /* perform select */
		  snprintf(buff, BUFFER_SIZE, "SELECT FROM t WHERE t%d = %d", rand()%3 + 1, rand()%500);
		  sqlite3_exec(db, buff, callback, 0, &error_message);
		  CHECK_SQL_RESULT;
		  ++selects;
		} else if ((drop > TEST_INSERT_PERCENTAGE + TEST_SELECT_PERCENTAGE)) {
		  /* perofrm delete */
		  snprintf(buff, BUFFER_SIZE, "DELETE FROM t WHERE t%d = %d",
				   1 + rand()%3, rand() % 500);
		  sqlite3_exec(db, buff, callback, 0, &error_message);
		  CHECK_SQL_RESULT;
		  ++deletes;
		} else {
		  printf("[e] invalid drop %d\n", drop);
		  return -1;
		}

		/* transactions logic */
		++already_done_ops_in_transaction;
		if (ops_in_transaction && (already_done_ops_in_transaction == ops_in_transaction)) {
		  sqlite3_exec(db, "COMMIT", callback, 0, &error_message);
		  CHECK_SQL_RESULT;
		  already_done_ops_in_transaction = 0;
		}
	  }

	  if (ops_in_transaction && (already_done_ops_in_transaction == ops_in_transaction)) {
		sqlite3_exec(db, "COMMIT", callback, 0, &error_message);
		CHECK_SQL_RESULT;
		already_done_ops_in_transaction = 0;
	  }



	  /* clean up */
	  ret = sqlite3_close(db);
	  CHECK_SQL_RESULT;

	  *time_spent = time(NULL) - start_time;


	  /* printf("inserts: %d\nselects: %d\n deletes: %d", inserts, selects, deletes); */
	  return inserts + selects + deletes;
	}


	void print_test_results(unsigned long ops, time_t time, const char *test_name) {
	  printf("test: %s\n", test_name);
	  printf("\ttotal: %u\n", ops);
	  printf("\truntime: %u\n", time);
	  printf("\tops per second: %f\n", (float)ops/time);
	}


	int run_a_test() {
	  unsigned long ops;
	  char config_buffer[10240];
	  time_t time_spent;
	  /* default config */
	  ops = _test(TEST_RUN_TIME, CONFIG_DEFAULT, 0, &time_spent);
	  print_test_results(ops, time_spent, "default_config");
	  /* single_transaction 100 ops in trans */
	  ops = _test(TEST_RUN_TIME, CONFIG_DEFAULT, 100, &time_spent);
	  print_test_results(ops, time_spent, "100_ops_in_transaction");
	  /* single_transaction 1000 ops in trans */
	  ops = _test(TEST_RUN_TIME, CONFIG_DEFAULT, 1000, &time_spent);
	  print_test_results(ops, time_spent, "1000_ops_in_transaction");
	  /* single_transaction 100000 ops in trans */
	  ops = _test(TEST_RUN_TIME, CONFIG_DEFAULT, 10000, &time_spent);
	  print_test_results(ops, time_spent, "10000_ops_in_transaction");
	  /* single_transaction 10000000 ops in trans */
	  ops = _test(TEST_RUN_TIME, CONFIG_DEFAULT, 10000000, &time_spent);
	  print_test_results(ops, time_spent, "10000000_ops_in_transaction");
	  /* synchronous off */
	  ops = _test(TEST_RUN_TIME, CONFIG_SYNCHRONOUS_OFF, 0, &time_spent);
	  print_test_results(ops, time_spent, "synchronous_off_config");
	  /* journal off */
	  ops = _test(TEST_RUN_TIME, CONFIG_JOURNAL_OFF, 0, &time_spent);
	  print_test_results(ops, time_spent, "journal_off_config");
	  /* journal delete */
	  ops = _test(TEST_RUN_TIME, CONFIG_JOURNAL_DELETE, 0, &time_spent);
	  print_test_results(ops, time_spent, "journal_delete_config");
	  /* journal truncate */
	  ops = _test(TEST_RUN_TIME, CONFIG_JOURNAL_TRUNCATE, 0, &time_spent);
	  print_test_results(ops, time_spent, "journal_truncate_config");
	  /* journal persist */
	  ops = _test(TEST_RUN_TIME, CONFIG_JOURNAL_PERSIST, 0, &time_spent);
	  print_test_results(ops, time_spent, "journal_persist_config");
	  /* journal memory */
	  ops = _test(TEST_RUN_TIME, CONFIG_JOURNAL_MEMORY, 0, &time_spent);
	  print_test_results(ops, time_spent, "journal_memory_config");
	  /* journal wal */
	  ops = _test(TEST_RUN_TIME, CONFIG_JOURNAL_WAL, 0, &time_spent);
	  print_test_results(ops, time_spent, "journal_wal_config");
	  /* journal wal autocheckpoint off */
	  ops = _test(TEST_RUN_TIME, CONFIG_JOURNAL_WAL_AUTOCHECKPOINT_OFF, 0, &time_spent);
	  print_test_results(ops, time_spent, "journal_wal_config_autocheckpoint_off");
	  /* journal wal autocheckpoint 10000 */
	  ops = _test(TEST_RUN_TIME, CONFIG_JOURNAL_WAL_AUTOCHECKPOINT_10000, 0, &time_spent);
	  print_test_results(ops, time_spent, "journal_wal_config_autocheckpoint_10000");
	  /* vacuum off */
	  ops = _test(TEST_RUN_TIME, CONFIG_VACUUM_FULL, 0, &time_spent);
	  print_test_results(ops, time_spent, "vacuum_full_config");
	  /* locking exclusive */
	  ops = _test(TEST_RUN_TIME, CONFIG_EXLUSIVE_LOCK, 0, &time_spent);
	  print_test_results(ops, time_spent, "exclusive_lock");
	  /* syncornous_off + jounrnal_off */
	  strcpy(config_buffer, CONFIG_SYNCHRONOUS_OFF);
	  strcat(config_buffer, CONFIG_JOURNAL_OFF);
	  ops = _test(TEST_RUN_TIME, config_buffer, 0, &time_spent);
	  print_test_results(ops, time_spent, "synchronouf_off_and_journal_off");
	  /* synchronous_off + jounrnal_off + exclusive_lock */
	  strcpy(config_buffer, CONFIG_SYNCHRONOUS_OFF);
	  strcat(config_buffer, CONFIG_JOURNAL_OFF);
	  strcat(config_buffer, CONFIG_EXLUSIVE_LOCK);
	  ops = _test(TEST_RUN_TIME, config_buffer, 0, &time_spent);
	  print_test_results(ops, time_spent,
						 "synchronouf_off_and_journal_off_and_exclusive_lock");
	  /* synchronous_off + jounrnal_off + vacum_off + exclusive_lock + 1000 ops in transaction */
	  strcpy(config_buffer, CONFIG_SYNCHRONOUS_OFF);
	  strcat(config_buffer, CONFIG_JOURNAL_OFF);
	  strcat(config_buffer, CONFIG_EXLUSIVE_LOCK);
	  ops = _test(TEST_RUN_TIME, config_buffer, 1000, &time_spent);
	  print_test_results(ops, time_spent,
						 "synchronouf_off_and_journal_off_and_vacuum_off_and_exclusive_lock_1000_ops_in_transaction");
	}


	int main(int argc, char **argv) {
	  return run_a_test();
	}
