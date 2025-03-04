#include <string.h>
#include <stdlib.h>
#include "harness/unity.h"
#include "../src/lab.h"

void setUp(void) {
}

void tearDown(void) {
}

void test_cmd_parse2(void)
{
    /* Test parsing "foo -v" */
    char *stng = (char*)malloc(sizeof(char)*7);
    strcpy(stng, "foo -v");
    char **actual = cmd_parse(stng);

    /* Expected output: "foo", "-v", NULL */
    char **expected = (char**) malloc(sizeof(char*) * 3);
    expected[0] = (char*)malloc(sizeof(char)*4);
    expected[1] = (char*)malloc(sizeof(char)*3);
    expected[2] = NULL;
    strcpy(expected[0], "foo");
    strcpy(expected[1], "-v");

    TEST_ASSERT_EQUAL_STRING(expected[0], actual[0]);
    TEST_ASSERT_EQUAL_STRING(expected[1], actual[1]);
    TEST_ASSERT_FALSE(actual[2]);

    free(expected[0]);
    free(expected[1]);
    free(expected);
    free(stng);
    cmd_free(actual);
}

void test_cmd_parse(void)
{
    char **rval = cmd_parse("ls -a -l");
    TEST_ASSERT_TRUE(rval);
    TEST_ASSERT_EQUAL_STRING("ls", rval[0]);
    TEST_ASSERT_EQUAL_STRING("-a", rval[1]);
    TEST_ASSERT_EQUAL_STRING("-l", rval[2]);
    TEST_ASSERT_EQUAL_STRING(NULL, rval[3]);
    TEST_ASSERT_FALSE(rval[3]);
    cmd_free(rval);
}

void test_trim_white_no_whitespace(void)
{
    char *line = (char*) calloc(10, sizeof(char));
    strncpy(line, "ls -a", 10);
    char *rval = trim_white(line);
    TEST_ASSERT_EQUAL_STRING("ls -a", rval);
    free(line);
}

void test_trim_white_start_whitespace(void)
{
    char *line = (char*) calloc(10, sizeof(char));
    strncpy(line, "  ls -a", 10);
    char *rval = trim_white(line);
    TEST_ASSERT_EQUAL_STRING("ls -a", rval);
    free(line);
}

void test_trim_white_end_whitespace(void)
{
    char *line = (char*) calloc(10, sizeof(char));
    strncpy(line, "ls -a  ", 10);
    char *rval = trim_white(line);
    TEST_ASSERT_EQUAL_STRING("ls -a", rval);
    free(line);
}

void test_trim_white_both_whitespace_single(void)
{
    char *line = (char*) calloc(10, sizeof(char));
    strncpy(line, " ls -a ", 10);
    char *rval = trim_white(line);
    TEST_ASSERT_EQUAL_STRING("ls -a", rval);
    free(line);
}

void test_trim_white_both_whitespace_double(void)
{
    char *line = (char*) calloc(10, sizeof(char));
    strncpy(line, "  ls -a  ", 10);
    char *rval = trim_white(line);
    TEST_ASSERT_EQUAL_STRING("ls -a", rval);
    free(line);
}

void test_trim_white_all_whitespace(void)
{
    char *line = (char*) calloc(10, sizeof(char));
    strncpy(line, "  ", 10);
    char *rval = trim_white(line);
    TEST_ASSERT_EQUAL_STRING("", rval);
    free(line);
}

void test_trim_white_mostly_whitespace(void)
{
    char *line = (char*) calloc(10, sizeof(char));
    strncpy(line, "    a    ", 10);
    char *rval = trim_white(line);
    TEST_ASSERT_EQUAL_STRING("a", rval);
    free(line);
}

void test_get_prompt_default(void)
{
    char *prompt = get_prompt("MY_PROMPT");
    TEST_ASSERT_EQUAL_STRING("shell>", prompt);
    free(prompt);
}

void test_get_prompt_custom(void)
{
    const char* prmpt = "MY_PROMPT";
    if(setenv(prmpt, "foo>", 1)) {
         TEST_FAIL();
    }
    char *prompt = get_prompt(prmpt);
    TEST_ASSERT_EQUAL_STRING("foo>", prompt);
    free(prompt);
    unsetenv(prmpt);
}

void test_ch_dir_home(void)
{
    char *line = (char*) calloc(10, sizeof(char));
    strncpy(line, "cd", 10);
    char **cmd = cmd_parse(line);
    char *expected = getenv("HOME");
    change_dir(cmd);
    char *actual = getcwd(NULL, 0);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(line);
    free(actual);
    cmd_free(cmd);
}

void test_ch_dir_root(void)
{
    char *line = (char*) calloc(10, sizeof(char));
    strncpy(line, "cd /", 10);
    char **cmd = cmd_parse(line);
    change_dir(cmd);
    char *actual = getcwd(NULL, 0);
    TEST_ASSERT_EQUAL_STRING("/", actual);
    free(line);
    free(actual);
    cmd_free(cmd);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_cmd_parse);
  RUN_TEST(test_cmd_parse2);
  RUN_TEST(test_trim_white_no_whitespace);
  RUN_TEST(test_trim_white_start_whitespace);
  RUN_TEST(test_trim_white_end_whitespace);
  RUN_TEST(test_trim_white_both_whitespace_single);
  RUN_TEST(test_trim_white_both_whitespace_double);
  RUN_TEST(test_trim_white_all_whitespace);
  RUN_TEST(test_trim_white_mostly_whitespace);
  RUN_TEST(test_get_prompt_default);
  RUN_TEST(test_get_prompt_custom);
  RUN_TEST(test_ch_dir_home);
  RUN_TEST(test_ch_dir_root);
  return UNITY_END();
}
