#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sqlite3.h>

#define CONFIG_DEFAULT "SELECT 1;"
#define CONFIG_SYNCHRONOUS_OFF "PRAGMA synchronous=OFF;"
#define CONFIG_JOURNAL_OFF "PRAGMA journal_mode = OFF;"
#define CONFIG_VACUM_OFF "PRAGMA auto_vacum = NONE;"
#define CONFIG_EXLUSIVE_LOCK "PRAGMA locking_mode = EXCLUSIVE;"


#define TEST_RUN_TIME 60      /* 1 minutes */


int callback(void *context, int argc, char **argv, char **columnNames) {
  return SQLITE_OK;                     /* do nothing */
}


#define CHECK_SQL_RESULT                        \
  if (ret != SQLITE_OK) {                       \
    return (unsigned long)-1;                   \
  }


#define DATABASE_FILE_NAME "write_test.sqlite"
#define BUFFER_SIZE 1024

unsigned long write_test(time_t time_to_run, const char* config) {
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
  while (time(NULL) - start_time < time_to_run) { /* run for a time_to_run time */
    snprintf(buff, BUFFER_SIZE, "INSERT INTO t (t1, t2 ,t3) VALUES(%d, %d, %d);",
             rand(), rand(), rand());
    sqlite3_exec(db, buff, callback, 0, &error_message);
    CHECK_SQL_RESULT;
    ++inserts;
  }
  /* clean up */
  ret = sqlite3_close(db);
  CHECK_SQL_RESULT;

  return inserts;
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
  /* default config */
  ops = write_test(TEST_RUN_TIME, CONFIG_DEFAULT);
  print_test_results(ops, TEST_RUN_TIME, "write_default_config");
  /* synchronous off */
  ops = write_test(TEST_RUN_TIME, CONFIG_SYNCHRONOUS_OFF);
  print_test_results(ops, TEST_RUN_TIME, "write_synchronous_off_config");
  /* journal off */
  ops = write_test(TEST_RUN_TIME, CONFIG_JOURNAL_OFF);
  print_test_results(ops, TEST_RUN_TIME, "write_journal_off_config");
  /* vacuum off */
  ops = write_test(TEST_RUN_TIME, CONFIG_VACUM_OFF);
  print_test_results(ops, TEST_RUN_TIME, "write_vacum_off_config");
  /* locking exclusive */
  ops = write_test(TEST_RUN_TIME, CONFIG_EXLUSIVE_LOCK);
  print_test_results(ops, TEST_RUN_TIME, "write_exclusive_lock");
  /* syncornous_off + jounrnal_off */
  strcpy(config_buffer, CONFIG_SYNCHRONOUS_OFF);
  strcat(config_buffer, CONFIG_JOURNAL_OFF);
  ops = write_test(TEST_RUN_TIME, config_buffer);
  print_test_results(ops, TEST_RUN_TIME, "write_synchronouf_off_and_journal_off");
  /* synchronous_off + jounrnal_off + vacum_off */
  strcpy(config_buffer, CONFIG_SYNCHRONOUS_OFF);
  strcat(config_buffer, CONFIG_JOURNAL_OFF);
  strcat(config_buffer, CONFIG_VACUM_OFF);
  ops = write_test(TEST_RUN_TIME, config_buffer);
  print_test_results(ops, TEST_RUN_TIME,
                     "write_synchronouf_off_and_journal_off_and_vacuum_off");
  /* synchronous_off + jounrnal_off + vacum_off + exclusive_lock*/
  strcpy(config_buffer, CONFIG_SYNCHRONOUS_OFF);
  strcat(config_buffer, CONFIG_JOURNAL_OFF);
  strcat(config_buffer, CONFIG_VACUM_OFF);
  strcat(config_buffer, CONFIG_EXLUSIVE_LOCK);
  ops = write_test(TEST_RUN_TIME, config_buffer);
  print_test_results(ops, TEST_RUN_TIME,
                     "write_synchronouf_off_and_journal_off_and_vacuum_off_and_exclusive_lock");
}


int main(int argc, char **argv) {
  return run_a_test();
}
