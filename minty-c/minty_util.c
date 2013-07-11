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
 * Creates a new Linkedlist with a single node containing the passed element
 */
LinkedList *LinkedList_init(void *first_element) {
	LinkedList *ll = safe_alloc(sizeof(LinkedList));
	ll->child_node = NULL;
	ll->element = first_element;
	return ll;
}

/*
 * Retrieves an item from the LinkedList at the given index
 */
void *LinkedList_get(LinkedList *ll, int index) {
	// Assertion that the index is positive, and that it is in the list's range
	assert(index >= 0 && (!ll->child_node ? index == 0 : 1))

	// If the given index is zero, return the element at this node
	if(index == 0) return ll->element;

	// Otherwise make a recursive call to the child node with (index - 1)
	else return LinkedList_get(ll->child_node, index - 1);
}

/*
 * Gets the length of the LinkedList
 */
int LinkedList_length(LinkedList *ll) {
	// If this is the tail node, return 1
	if(!ll->child_node) return 1;

	// Otherwise, return the length of the sub-list + 1
	else return 1 + LinkedList_length(ll->child_node);
}

/*
 * Appends a new element to the end of the LinkedList
 */
void LinkedList_append(LinkedList *ll, void *element) {
	// If this is the tail node, create a new node as the child of this node,
	// and put the element in it
	if(!ll->child_node) {
		LinkedList *new_node = LinkedList_init(element);
		ll->child_node = new_node;
	}
	// Otherwise, ask the child of this node to append the new element
	else LinkedList_append(ll->child_node, element);
}

/*
 * Deallocates an entire LinkedList and all elements in it. May cause memory
 * leaks if contained elements are complex objects, as they will be freed
 * naively.
 */
void LinkedList_free(LinkedList *ll) {
	LinkedList_free(ll->child_node);
	free(ll->element)
	free(ll);
}