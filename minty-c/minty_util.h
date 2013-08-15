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

char *str_concat(int count, ...);

char *str_concat_2(char *str1, char *str2);

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

LinkedList *LinkedList_init_with(void *element);

void *LinkedList_get(LinkedList *ll, int index);

int LinkedList_length(LinkedList *ll);

void LinkedList_append(LinkedList *ll, void *element);

void *LinkedList_pop(LinkedList *ll);

LinkedList *LinkedList_copy(LinkedList *ll);

void LinkedList_remove(LinkedList *ll, int index);

void LinkedList_free(LinkedList *ll);

/*
 * LLIterators provide more efficient iteration over LinkedLists. Since
 * LinkedLists are singly linked, LLIterators can only iterate over LinkedLists
 * in the forward direction (following the pointers).
 */
typedef struct LLIterator LLIterator;
struct LLIterator {
	LinkedList *list;
	LinkedListNode *current_node;
	int index;
};

LLIterator *LLIterator_init(LinkedList *list);

bool LLIterator_ended(LLIterator *iter);

void *LLIterator_get_current(LLIterator *iter);

int LLIterator_current_index(LLIterator *iter);

void LLIterator_advance(LLIterator *iter);

/*
 * Macro that can be used to apply a function to every element in a LinkedList,
 * like python/haskell's map function. It uses the efficient iterators defined
 * for the LinkedList type.
 *
 *     The 'list' parameter is the LinkedList whose elements the function will
 *     be applied to.
 *     The 'typecast' parameter is used to assign the appropriate type to the
 *     elements of the list. For this reason, LLMAP should only be used on lists
 *     where all the elements have the same type.
 *     The 'map_function' parameter is the name of the function that should be
 *     applied to each element. It must take a single parameter of the type
 *     given as the typecast parameter.
 */
#define LLMAP(list, typecast, map_function) \
	do { \
		LLIterator *iter = LLIterator_init(list); \
		while(!LLIterator_ended(iter)) { \
			map_function(((typecast)LLIterator_get_current(iter))); \
			LLIterator_advance(iter); \
		} \
		free(iter); \
	} while(0)

/*
 * Macro that can be used to apply a function to every argument in a list, as
 * with LLMAP(), but uses a function that takes a second parameter, which is
 * supplied as the fourth argument for this macro.
 */
#define LLMAP_PARAM(list, typecast, map_function, second_parameter) \
	do { \
		LLIterator *iter = LLIterator_init(list); \
		while(!LLIterator_ended(iter)) { \
			map_function( \
				((typecast)LLIterator_get_current(iter)), \
				second_parameter); \
			LLIterator_advance(iter); \
		} \
		free(iter); \
	} while(0)

#endif // MINTY_UTIL