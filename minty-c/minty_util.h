/*
 * Header file for minty_util.c
 * Contains forward declarations of some safe memory allocation functions and
 * some string operation functions.
 */

void *safe_alloc(int size);

char *safe_strdup(char *str);

int str_equal(char *str1, char *str2);

char *str_append(char *str, char c);