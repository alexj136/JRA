#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
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
bool str_equal(char *str1, char *str2) {
	// If they have different lengths, we can say immediately that they differ
	if(strlen(str1) != strlen(str2)) return 0;
	// If they are she same length, we must use strncmp to compare them. strncmp
	// returns 0 for identical strings, and other ints for different ones, so we
	// negate the result.
	else return !strncmp(str1, str2, strlen(str1));
}

/*
 * Varargs function used to concatenate any number of strings - a new string is
 * allocated and a pointer to it is returned. Works on arbitrarily long strings
 * and the buffer produced has no wasted space. The 'count' parameter is the
 * number of strings being passed. If count is 0, a NULL pointer is returned. If
 * count is greater than the number of given arguments, this function's
 * behaviour is undefined.
 */
char *str_concat(int count, ...) {

	// If count is zero, there is nothing to do, return NULL as specified.
	if(count == 0) return NULL;

	// Prepare varargs for usage
	va_list args;
	va_start(args, count);

	// Copy the first argument into str. If we don't use a copy, the for loop
	// would free the passed string, which would be very, very bad.
	char *str = safe_strdup(va_arg(args, char *));

	// If count is one, copy the string using safe_strdup and return a pointer
	// to it (we must also call va_end to clean up the varargs)
	if(count == 1) {
		va_end(args);
		return str;
	}

	// Repeatedly get the next string, concatenate it to the previous one, and
	// free the previous one.
	int i;
	for(i = 1; i < count; i++) {
		char *str_temp = va_arg(args, char *);
		char *str_temp_2 = str_concat_2(str, str_temp);

		free(str);
		str = str_temp_2;
	}

	// Clean up varargs and use 
	va_end(args);
	return str;
}

/*
 * Creates a string which is the concatenation of the two input strings, and
 * returns a pointer to the concatenated string. Works on arbitrarily long
 * strings and the buffer produced has no wasted space.
 */
char *str_concat_2(char *str1, char *str2) {
	// Record the lengths of the two strings
	int str1_len = strlen(str1);
	int str2_len = strlen(str2);

	// Allocate space for the new string. The length of the new string will be
	// the sum of strlen for each input string, plus one for the null terminator
	// which strlen does not account for
	char *new_str = (char *)safe_alloc(str1_len + str2_len + 1);

	// Copy str1 into the newly allocated space
	strcpy(new_str, str1);

	// Concatenate str2 into new_str after str1
	strcat(new_str, str2);

	// Return the new string
	return new_str;
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
	strcpy(new_str, str);

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
 * Creates a new Linkedlist with the given argument as the first element
 */
LinkedList *LinkedList_init_with(void *element) {
	LinkedList *ll = safe_alloc(sizeof(LinkedList));
	ll->head_node = NULL;
	LinkedList_append(ll, element);
	return ll;
}

/*
 * Used by LinkedList_get to retrieve an item from the LinkedList at the given
 * index by recursing to the required node
 */
static void *LinkedListNode_get(LinkedListNode *lln, int index) {
	// Assertion that the index is positive, and that it is in the list's range
	assert(index >= 0 && (!lln->child_node ? index == 0 : true));

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


	// Assertion that the index is positive, and that it is in the list's range
	// assert(index >= 0 && (!lln->child_node ? index == 0 : true));
	// int i = 0;
	// LinkedlistNode *nextNode = ll->head_node;
	// while(nextNode != NULL && i < index) {
	// 	nextNode = nextNode->child_node;
	// 	i++;
	// }
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
	if(!(ll->head_node)) {
		LinkedListNode *new_node = LinkedListNode_init(element);
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
 * Creates a shallow copy of a LinkedList i.e. a new list of pointers to the
 * same objects
 */
LinkedList *LinkedList_copy(LinkedList *ll) {
	// Create a new empty list
	LinkedList *new_ll = LinkedList_init();
	
	// Repeatedly append the elements from the old list
	int i;
	for(i = 0; i < LinkedList_length(ll); i++) {
		LinkedList_append(new_ll, LinkedList_get(ll, i));
	}

	// Return the new list
	return new_ll;
}

/*
 * Recursive supporting function for LinkedList_remove
 */
void LinkedListNode_remove(LinkedListNode *lln, int index) {
	// index SHOULD be guaranteed never to be zero for this function - the 0
	// case is handled by LinkedList_remove. It does, an error has occured, so
	// print a message and exit
	if(index == 0) {
		printf("Error in recursion logic for LinkedList_remove - hit index == \
			0 within LinkedListNode_remove\n");
		exit(EXIT_FAILURE);
	}

	// If index == 1, we handle the removal, rather than recurse
	else if(index == 1) {

		// If the current node has a subnode...
		if (lln->child_node) {

			// If the child has a child of its own, set the child of the current
			// node as the child of the previous node, so the chain bypasses the
			// node being removed. Also free the node being removed!
			if(lln->child_node->child_node) {
				LinkedListNode *child_of_child = lln->child_node->child_node;
				free(lln->child_node);
				lln->child_node = child_of_child;
			}

			// If the child has no child of its own, it is the last node of the
			// list. We can free it and just set the child of the current node
			// as null.
			else {
				free(lln->child_node);
				lln->child_node = NULL;
			}
		}

		// If the current node doesn't have a subnode, there was an index out
		// of bounds so raise an error
		else {
			printf("Error: LinkedList removal out of bounds\n");
			exit(EXIT_FAILURE);
		}
	}

	// If index > 1, handle the removal at a deeper recursion level
	else LinkedListNode_remove(lln->child_node, index - 1);
}

/*
 * Removes the LinkedList node at the given index (DOES NOT FREE THE CONTENTS - 
 * MUST BE DONE MANUALLY OR MEMORY LEAK MAY OCCUR)
 */
void LinkedList_remove(LinkedList *ll, int index) {
	// If the LinkedList is empty, removal is undefined, so assert that the list
	// is not empty
	if(!(ll->head_node)) {
		printf("Cannot remove from an empty LinkedList\n");
		exit(EXIT_FAILURE);
	}

	// If we're removing the first node...
	if(index == 0) {

		// ...and the first node is not the only node...
		if(ll->head_node->child_node) {
			// ...then free the first node, and set the head node to be the
			// node that was previously the second node.
			LinkedListNode *old_head = ll->head_node;
			ll->head_node = ll->head_node->child_node;
			free(old_head);
		}

		// ...and the first node IS the only node...
		else { // if(!(ll->head_node->child_node)) {
			// ...then simply free the head node and nullify the pointer
			free(ll->head_node);
			ll->head_node = NULL;
		}
	}

	// If we're not removing the first node, we must use the recursive
	// supporting function for LinkedListNode
	else LinkedListNode_remove(ll->head_node, index);
}

/*
 * Recursive function used by LinkedList_free to recursively free LinkedList
 * nodes
 */
static void LinkedListNode_free_recursive(LinkedListNode *lln) {
	if(lln->child_node) LinkedListNode_free_recursive(lln->child_node);
	free(lln);
}

/*
 * Deallocates an entire LinkedList (but not the elements in it). Will cause
 * memory leaks if contained elements are objects, as they will not be freed
 */
void LinkedList_free(LinkedList *ll) {
	if(ll->head_node) LinkedListNode_free_recursive(ll->head_node);
	free(ll);
}