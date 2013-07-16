#include <malloc.h>
#include <assert.h>
#include <string.h>
#include "minty_util.h"

/*
 * Safely allocates heap memory by asserting that the allocation was successful
 */
void *safe_alloc(int size) {
	void *ptr = malloc(size);
	assert(ptr);
	return ptr;
}

/*
 * Safely calls strdup by asserting that the string allocation was successful
 */
char *safe_strdup(char *str) {
	char *alloc_str = strdup(str);
	assert(alloc_str);
	return alloc_str;
}

/* 
 * Function that decides if two strings are equal. Returns 1 (true) if they are
 * the same, or 0 (false) if they differ.
 */
int str_equal(char *str1, char *str2) {
	// If they have different lengths, we can say immidiately that they differ
	if(strlen(str1) != strlen(str2)) return 0;
	// If they are she same length, we must use strncmp to compare them. strncmp
	// returns 0 for identical strings, and other ints for different ones, so we
	// negate the result.
	else return !strncmp(str1, str2, strlen(str1));
}

/* 
 * Takes a pointer to a char array and returns a pointer to a value-equal
 * string (but for the appended character) on the heap. Will cause a memory leak
 * if the string at the returned pointer is not freed when appropriate.
 * May cause an error if appending exceeds the size of the given array.
 */
char *str_append(char *str, char c) {
	// Allocate the appropriate amount of memory for the new string - this is
	// the size of the old string, inlucing the null terminator, plus one.
	char *new_str = safe_alloc((strlen(str) + 2) * sizeof(char));
	
	// Copy the caracters from the old string into the new one, but not the null
	// terminator
	int i;
	for(i = 0; i < strlen(str); i++) new_str[i] = str[i];

	// Write the new char after the contents of the old string
	new_str[strlen(str)] = c;

	// Write the null terminator
	new_str[strlen(str) + 1] = '\0';

	// Free the old string from memory
	free(str);

	// Return the pointer to the new string
	return new_str;
}

/*
 * The following functions are for use with LinkedList structs
 */

/*
 * Creates a new LinkedlistNode
 */
static LinkedListNode *LinkedListNode_init(void *element) {
	LinkedListNode *lln = safe_alloc(sizeof(LinkedListNode));
	lln->child_node = NULL;
	lln->element = element;
	return lln;
}

/*
 * Creates a new Linkedlist
 */
LinkedList *LinkedList_init() {
	LinkedList *ll = safe_alloc(sizeof(LinkedList));
	ll->head_node = NULL;
	return ll;
}

/*
 * Used by LinkedList_get to retrieve an item from the LinkedList at the given
 * index by recursing to the required node
 */
static void *LinkedListNode_get(LinkedListNode *lln, int index) {
	// Assertion that the index is positive, and that it is in the list's range
	assert(index >= 0 && (!lln->child_node ? index == 0 : 1));

	// If the given index is zero, return the element at this node
	if(index == 0) return lln->element;

	// Otherwise make a recursive call to the child node with (index - 1)
	else return LinkedListNode_get(lln->child_node, index - 1);
}

/*
 * Externally accessible interface to LinkedListNode_get
 */
void *LinkedList_get(LinkedList *ll, int index) {
	return LinkedListNode_get(ll->head_node, index);
}

/*
 * Used by LinkedList_length to get the length of the LinkedList using recursion
 */
static int LinkedListNode_length(LinkedListNode *lln) {
	// If this is the tail node (so if the child node is null), return 1
	if(!lln->child_node) return 1;

	// Otherwise, return the length of the sub-list + 1
	else return 1 + LinkedListNode_length(lln->child_node);
}

/*
 * Gets the length of the LinkedList
 */
int LinkedList_length(LinkedList *ll) {
	// If there are no nodes, the length is 0
	if(!ll->head_node) return 0;

	// Otherwise, return the result of the recursive LinkedListNode_length
	// function
	else return LinkedListNode_length(ll->head_node);
}

/*
 * Function used by LinkedList_append to append a new element to the end of the
 * LinkedList using recursion
 */
static void LinkedListNode_append(LinkedListNode *lln, void *element) {
	// If this is the tail node, create a new node as the child of this node,
	// and put the element in it
	if(!lln->child_node) {
		LinkedListNode *new_node = LinkedListNode_init(element);
		lln->child_node = new_node;
	}
	// Otherwise, ask the child of this node to append the new element
	else LinkedListNode_append(lln->child_node, element);
}

/*
 * Appends a new element to the end of the LinkedList
 */
void LinkedList_append(LinkedList *ll, void *element) {
	// If the LinkedList is empty, create a new node with the given element as
	// the head node
	if(!ll->head_node) {
		LinkedList *new_node = LinkedListNode_init(element);
		ll->head_node = new_node;
	}
	// Otherwise, use LinkedListNode_append to add it after the last existing
	// node
	else LinkedListNode_append(ll->head_node, element);
}

/*
 * Returns the element at the top/front of the list, and removes it from the
 * list
 */
void *LinkedList_pop(LinkedList *ll) {
	// If there are no elements left, return NULL
	if(!ll->head_node) return NULL;

	// Get a reference to the old head node so we can free it once we're done
	// with it
	LinkedListNode *old_head = ll->head_node;

	// Retrieve the top element which will be returned
	void *top_element = old_head->element;

	// Set the node that came after the old head as the new head
	ll->head_node = old_head->child_node;

	// Free the old head
	free(old_head);

	return top_element;
}

/*
 * Recursive function used by LinkedList_free to recursively free LinkedList
 * nodes
 */
static void LinkedListNode_free(LinkedListNode *lln) {
	if(lln->child_node) LinkedListNode_free(lln->child_node);
	free(lln);
}

/*
 * Deallocates an entire LinkedList (but not the elements in it). Will cause
 * memory leaks if contained elements are objects, as they will not be freed
 */
void LinkedList_free(LinkedList *ll) {
	LinkedListNode_free(ll->head_node);
	free(ll);
}