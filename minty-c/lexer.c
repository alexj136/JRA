#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
 
/* This file contains functions that are used for lexical analysis.
 */

int in_valueless_tokens(char str[]);
int char_indices(char letter);
int string_equal(char *str1, char *str2);

/* Struct used to represent a token
 */
typedef struct Token {
	char *type;
	char *info;
};

int transition_table[54][36] = {//                                     abcdjkx
// e   f   g   h   i   l   m   n   o   p   q   r   s   t   u   v   w   yzCAPS_   0-9   {   }   (   )   *   /   %   ,   ;   ?   :   <   >   =   +   -   !
{  50,  1, 49, 49,  4, 49, 49, 49, 49,  2, 49,  3, 49, 49, 49, 49,  5, 49,       48,   37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 23, 26, 36, 28, 31, 34 },  // state 0
{  49, 49, 49, 49, 49, 49, 49, 17,  6, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 1
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,  8, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 2
{  12, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 3
{  49, 18, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 4
{  49, 49, 49, 19, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 5
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,  7, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 6
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 7
{  49, 49, 49, 49,  9, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 8
{  49, 49, 49, 49, 49, 49, 49, 10, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 9
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 11, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 10
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 11
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 13, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 12
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 14, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 13
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 15, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 14
{  49, 49, 49, 49, 49, 49, 49, 16, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 15
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 16
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 17
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 18
{  49, 49, 49, 49, 20, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 19
{  49, 49, 49, 49, 49, 21, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 20
{  22, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 21
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 22
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 25, -1, 24, -1 },  // state 23
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 24
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 25
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 27, -1, -1, -1 },  // state 26
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 27
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 30, 29, -1, -1 },  // state 28
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 29
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 30
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 33, -1, 32, -1 },  // state 31
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 32
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 33
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 35, -1, -1, -1 },  // state 34
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 35
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 36
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 37
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 38
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 39
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 40
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 41
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 42
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 43
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 44
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 45
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 46
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 47
{  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       48,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 48
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 49
{  49, 49, 49, 49, 49, 51, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 50
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 52, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 51
{  53, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // state 52
{  49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,       49,   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }}; // state 53

/* Maps possible input characters to columns in the finite state machine table.
 * If a character that does not belong in the alphabet of the language, -1 is
 * returned to indicate an error.
 */
int char_indices(char letter) {
	if     (letter == 'a') return 17; else if(letter == 'b') return 17;
	else if(letter == 'c') return 17; else if(letter == 'd') return 17;
	else if(letter == 'e') return  0; else if(letter == 'f') return  1;
	else if(letter == 'g') return  2; else if(letter == 'h') return  3;
	else if(letter == 'i') return  4; else if(letter == 'j') return 17;
	else if(letter == 'k') return 17; else if(letter == 'l') return  5;
	else if(letter == 'm') return  6; else if(letter == 'n') return  7;
	else if(letter == 'o') return  8; else if(letter == 'p') return  9;
	else if(letter == 'q') return 10; else if(letter == 'r') return 11;
	else if(letter == 's') return 12; else if(letter == 't') return 13;
	else if(letter == 'u') return 14; else if(letter == 'v') return 15;
	else if(letter == 'w') return 16; else if(letter == 'x') return 17;
	else if(letter == 'y') return 17; else if(letter == 'z') return 17;
	else if(letter == 'A') return 17; else if(letter == 'B') return 17;
	else if(letter == 'C') return 17; else if(letter == 'D') return 17;
	else if(letter == 'E') return 17; else if(letter == 'F') return 17;
	else if(letter == 'G') return 17; else if(letter == 'H') return 17;
	else if(letter == 'I') return 17; else if(letter == 'J') return 17;
	else if(letter == 'K') return 17; else if(letter == 'L') return 17;
	else if(letter == 'M') return 17; else if(letter == 'N') return 17;
	else if(letter == 'O') return 17; else if(letter == 'P') return 17;
	else if(letter == 'Q') return 17; else if(letter == 'R') return 17;
	else if(letter == 'S') return 17; else if(letter == 'T') return 17;
	else if(letter == 'U') return 17; else if(letter == 'V') return 17;
	else if(letter == 'W') return 17; else if(letter == 'X') return 17;
	else if(letter == 'Y') return 17; else if(letter == 'Z') return 17;
	else if(letter == '0') return 18; else if(letter == '1') return 18;
	else if(letter == '2') return 18; else if(letter == '3') return 18;
	else if(letter == '4') return 18; else if(letter == '5') return 18;
	else if(letter == '6') return 18; else if(letter == '7') return 18;
	else if(letter == '8') return 18; else if(letter == '9') return 18;
	else if(letter == '_') return 17; else if(letter == '{') return 19;
	else if(letter == '}') return 20; else if(letter == '(') return 21;
	else if(letter == ')') return 22; else if(letter == '*') return 23;
	else if(letter == '/') return 24; else if(letter == '%') return 25;
	else if(letter == ',') return 26; else if(letter == ';') return 27;
	else if(letter == '?') return 28; else if(letter == ':') return 29;
	else if(letter == '<') return 30; else if(letter == '>') return 31;
	else if(letter == '=') return 32; else if(letter == '+') return 33;
	else if(letter == '-') return 34; else if(letter == '!') return 35;
	else return -1;
}

/* Maps final states to strings that represent token types, for example, if
 * the FSM traversal finished on final state 3, the token type would be "ID"
 * because final_mapping[3] = "ID".
 */
char *final_mapping[] = {
	"ERROR", "ID", "ID", "ID", "ID", "ID", "ID", "for", "ID", "ID", "ID",
	"print", "ID", "ID", "ID", "ID", "return", "fn", "if", "ID", "ID", "ID",
	"while", "<", "<-", "<=", ">", ">=", "+", "++", "+=", "-", "--", "-=",
	"ERROR", "!=", "=", "{", "}", "(", ")", "*", "/", "%%", ",", ";", "?", ":",
	"INT", "ID", "ID", "ID", "ID", "else"
};

int in_valueless_tokens(char str[]) {
	if (string_equal(str, "fn")    || string_equal(str, "for")    ||
		string_equal(str, "if")    || string_equal(str, "while")  ||
		string_equal(str, "print") || string_equal(str, "return") ||
		string_equal(str, "else")  || string_equal(str, "<")      ||
		string_equal(str, "<-")    || string_equal(str, "<=")     ||
		string_equal(str, ">")     || string_equal(str, ">=")     ||
		string_equal(str, "+")     || string_equal(str, "++")     ||
		string_equal(str, "+=")    || string_equal(str, "-")      ||
		string_equal(str, "--")    || string_equal(str, "-=")     ||
		string_equal(str, "!=")    || string_equal(str, "=")      ||
		string_equal(str, "{")     || string_equal(str, "}")      ||
		string_equal(str, "(")     || string_equal(str, ")")      ||
		string_equal(str, "*")     || string_equal(str, "/")      ||
		string_equal(str, "%%")    || string_equal(str, ",")      ||
		string_equal(str, ";")     || string_equal(str, ";")      ||
		string_equal(str, "?")     || string_equal(str, ":")) return 1;
		else return 0;
}


/* Function that decides if two strings are equal. Returns 1 (true) if they are
 * the same, or 0 (false) if they differ.
 */
int string_equal(char *str1, char *str2) {
	return !strncmp(str1, str2,
		(strlen(str1) >= strlen(str2)) ? strlen(str1) : strlen(str2));
}

/* Trims the leading whitespace from a string, for example, "    \n  \t hello"
 * becomes "hello", "  four  \n", becomes "four  \n". If a string has only
 * whitespace and a null terminator, only the null terminator will remain. If
 * no null terminator is reached, an error will occur.
 *     This function takes a pointer and returns a pointer, and as such, a
 * reference to the array initially declared should be kept, so that it can
 * be freed once finished with. The pointer that this function returns will not
 * free the (entire) array that was declared.
 */
char *trim_whitespace(char *str) {
	while(isspace(*str)) str += sizeof(char);
	return str;
}

int input_left(char *str) {
	return *str == '\0';
}

//WILL CHANGE - TAKE STR FROM HEAP, ALLOCATE NEW MEM & MOVE, THEN ADD NEW CHAR,
//THEN FREE OLD STR
/* Takes a char array (stack-allocated) and returns a pointer to a value-equal
 * string (but for the appended character) on the heap. Will cause a memory leak
 * if the string at the returned pointer is not freed when appropriate.
 * May cause an error if appending exceeds the size of the given array.
 */
char str_append(char *str, char c) {
	int i = 0;
	while(str[i] != '\0') i++;
	str[i] = c;

	// We allocate with the size of the original string, not +1, because we
	// assumed in the first place that the passed array had extra space for
	// the new character
	char *str_ptr = malloc(sizeof(str))
}

char *get_next_token(char *input) {

	// Char array to build the token array into
	char token_arr[32] = "";

	// The state number for the final state after FSM traversal
	int final_state;

	// The state in the FSM that we are currently in, during traversal
	int current_state = 0;

	// Trim the leading whitespace from the input
	input = trim_whitespace(input);

	while(current_state != -1 && *input != '\0') {

	}

	return "lol";
} 

char **lex(char *input) {

	// The number of tokens found in the input
	int num_tokens;

	// Allocate an array of (pointers to) strings on the heap
	char **token_list = malloc(num_tokens * sizeof(char*));

	// Allocate memory for a string for each pointer in the array, where strings
	// can actually be stored
	int i;
	for(i = 0; i < num_tokens; i++) token_list[i] = malloc(32 * sizeof(char));

	// Do some stuff

	return token_list;
}

int main() {
	printf("%d\n", isspace(' '));
	printf("%d\n", isspace('\t'));
	printf("%d\n", isspace('\v'));
	printf("%d\n", isspace('\n'));
	printf("%d\n", isspace('\f'));
	printf("%d\n", isspace('\r'));
	printf("%d\n", isspace('a'));
	printf("%d\n", isspace('?'));
	return 0;
}