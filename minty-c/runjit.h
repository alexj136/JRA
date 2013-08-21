#ifndef RUNJIT
#define RUNJIT

#define byte unsigned char

/*
 * Struct storing an array and its length in bytes, useful for handling machine
 * code.
 */
typedef struct {
	byte *arr;
	int len;
} ArrLen;

ArrLen *ArrLen_init(byte *arr, int len);

ArrLen *ArrLen_copy(ArrLen *original);

ArrLen *ArrLen_concat_2(ArrLen *al1, ArrLen *al2);

ArrLen *ArrLen_concat(int count, ...);

ArrLen *jitcode_expression(Expression *expr, Program *prog);

int jitexec_expression(ArrLen *expr_code);

#endif // RUNJIT