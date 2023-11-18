

#define ARGTYPE_NOVALUE 0
#define ARGTYPE_STRING  1
#define ARGTYPE_INT8    2
#define ARGTYPE_INT16   3
#define ARGTYPE_INT32   4
#define ARGTYPE_INT64   5

/* 外部提供関数 */
void parser_initialize(void);
void parser_clear(void);

int parser_addkey(char*, char*, char);
void parser_start(int, char **);

char parser_isexist(char*);
char parser_getvalue(char*, void**, int*);