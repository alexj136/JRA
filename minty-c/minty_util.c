#include <malloc.h>
#include <assert.h>
#include <string.h>

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