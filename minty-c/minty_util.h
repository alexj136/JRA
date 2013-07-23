/*
 * Header file for minty_util.c
 * Contains forward declarations of some safe memory allocation functions and
 * some string operation functions.
 */

#ifndef MINTY_UTIL
#define MINTY_UTIL

typedef enum {
	false,
	true
} bool;

void *safe_alloc(int size);

char *safe_strdup(char *str);

bool str_equal(char *str1, char *str2);

char *str_concat(char *str1, char *str2);

char *str_append(char *str, char c);

typedef struct LinkedListNode LinkedListNode;
struct LinkedListNode {
	struct LinkedListNode *child_node;
	void *element;
};

/*
 * Linked list used to store argument/statement lists & associated functions
 */
typedef struct LinkedList LinkedList;
struct LinkedList{
	LinkedListNode *head_node;
};

LinkedList *LinkedList_init();

void *LinkedList_get(LinkedList *ll, int index);

int LinkedList_length(LinkedList *ll);

void LinkedList_append(LinkedList *ll, void *element);

void *LinkedList_pop(LinkedList *ll);

LinkedList *LinkedList_copy(LinkedList *ll);

void LinkedList_free(LinkedList *ll);

#endif // MINTY_UTIL