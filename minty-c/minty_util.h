/*
 * Header file for minty_util.c
 * Contains forward declarations of some safe memory allocation functions and
 * some string operation functions.
 */

void *safe_alloc(int size);

char *safe_strdup(char *str);

int str_equal(char *str1, char *str2);

char *str_append(char *str, char c);

/*
 * Linked list used to store argument/statement lists & associated functions
 */
 typedef struct {
 	LinkedList *child_node;
 	void *element;
 } LinkedList;

 LinkedList *LinkedList_init(void *first_element);

 void *LinkedList_get(LinkedList *ll, int index);

 int LinkedList_length(LinkedList *ll);

 void LinkedList_append(LinkedList *ll, void *element);

 void LinkedList_free(LinkedList *ll);