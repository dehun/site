#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sqlite3.h>

#define CONFIG_DEFAULT "SELECT 1;"
#define CONFIG_SYNCHRONOUS_OFF "PRAGMA synchronous=OFF;"
#define CONFIG_JOURNAL_OFF "PRAGMA journal_mode = OFF;"
#define CONFIG_JOURNAL_MEMORY "PRAGMA journal_mode = memory;"
#define CONFIG_VACUM_OFF "PRAGMA auto_vacum = NONE;"
#define CONFIG_EXLUSIVE_LOCK "PRAGMA locking_mode = EXCLUSIVE;"


#define TEST_RUN_TIME 15      /* 1 minutes */
#define TEST_INSERT_PERCENTAGE 60
#define TEST_SELECT_PERCENTAGE 30
#define TEST_DELETE_PERCENTAGE 10


int callback(void *context, int argc, char **argv, char **columnNames) {
  return SQLITE_OK;                     /* do nothing */
}


#define CHECK_SQL_RESULT                        \
  if (ret != SQLITE_OK) {                       \
    return (unsigned long)-1;                   \
  }


#define DATABASE_FILE_NAME "write_test.sqlite"
#define BUFFER_SIZE 1024

unsigned long _test(time_t time_to_run, const char* config,
                         int group_transaction, time_t *time_spent) {
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

  if (group_transaction) {
    sqlite3_exec(db, "BEGIN", callback, 0, &error_message);
    CHECK_SQL_RESULT;
  }

  while (time(NULL) - start_time < time_to_run) { /* run for a time_to_run time */
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
  }


  if (group_transaction) {
    sqlite3_exec(db, "COMMIT", callback, 0, &error_message);
    CHECK_SQL_RESULT;
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
  print_test_results(ops, time_spent, "write_default_config");
  /* single_transaction config */
  ops = _test(TEST_RUN_TIME, CONFIG_DEFAULT, 1, &time_spent);
  print_test_results(ops, time_spent, "write_single_transaction");
  /* synchronous off */
  ops = _test(TEST_RUN_TIME, CONFIG_SYNCHRONOUS_OFF, 0, &time_spent);
  print_test_results(ops, time_spent, "write_synchronous_off_config");
  /* journal off */
  ops = _test(TEST_RUN_TIME, CONFIG_JOURNAL_OFF, 0, &time_spent);
  print_test_results(ops, time_spent, "write_journal_off_config");
  /* journal off */
  ops = _test(TEST_RUN_TIME, CONFIG_JOURNAL_MEMORY, 0, &time_spent);
  print_test_results(ops, time_spent, "write_journal_memory_config");
  /* vacuum off */
  ops = _test(TEST_RUN_TIME, CONFIG_VACUM_OFF, 0, &time_spent);
  print_test_results(ops, time_spent, "write_vacum_off_config");
  /* locking exclusive */
  ops = _test(TEST_RUN_TIME, CONFIG_EXLUSIVE_LOCK, 0, &time_spent);
  print_test_results(ops, time_spent, "write_exclusive_lock");
  /* syncornous_off + jounrnal_off */
  strcpy(config_buffer, CONFIG_SYNCHRONOUS_OFF);
  strcat(config_buffer, CONFIG_JOURNAL_OFF);
  ops = _test(TEST_RUN_TIME, config_buffer, 0, &time_spent);
  print_test_results(ops, time_spent, "write_synchronouf_off_and_journal_off");
  /* synchronous_off + jounrnal_off + vacum_off */
  strcpy(config_buffer, CONFIG_SYNCHRONOUS_OFF);
  strcat(config_buffer, CONFIG_JOURNAL_OFF);
  strcat(config_buffer, CONFIG_VACUM_OFF);
  ops = _test(TEST_RUN_TIME, config_buffer, 0, &time_spent);
  print_test_results(ops, time_spent,
                     "write_synchronouf_off_and_journal_off_and_vacuum_off");
  /* synchronous_off + jounrnal_off + vacum_off + exclusive_lock */
  strcpy(config_buffer, CONFIG_SYNCHRONOUS_OFF);
  strcat(config_buffer, CONFIG_JOURNAL_OFF);
  strcat(config_buffer, CONFIG_VACUM_OFF);
  strcat(config_buffer, CONFIG_EXLUSIVE_LOCK);
  ops = _test(TEST_RUN_TIME, config_buffer, 0, &time_spent);
  print_test_results(ops, time_spent,
                     "write_synchronouf_off_and_journal_off_and_vacuum_off_and_exclusive_lock");
  /* synchronous_off + jounrnal_off + vacum_off + exclusive_lock + single_transaction */
  strcpy(config_buffer, CONFIG_SYNCHRONOUS_OFF);
  strcat(config_buffer, CONFIG_JOURNAL_OFF);
  strcat(config_buffer, CONFIG_VACUM_OFF);
  strcat(config_buffer, CONFIG_EXLUSIVE_LOCK);
  ops = _test(TEST_RUN_TIME, config_buffer, 1, &time_spent);
  print_test_results(ops, time_spent,
                     "write_synchronouf_off_and_journal_off_and_vacuum_off_and_exclusive_lock_single_transaction");
}


int main(int argc, char **argv) {
  return run_a_test();
}
