#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "svosdef.h"
#include "parser.h"

#define ARGENTRY_ADD_INCLEMENTNUM 8
#define VALUEENTRY_ADD_INCREMENTNUM 32

#define PARSER_STS_KEY  1
#define PARSER_STS_VAL  2

typedef struct _ARGENTRY
{
    UCHAR typec;
    UCHAR exist;
    char* key1;
    char* key2;
    void* valueptr;
    int valuenumi;
    int valuemaxi;
}ARGENTRY;

typedef struct _ARGLIST
{
    int keynumi;
    int keymaxi;
    ARGENTRY **argv;
}ARGLIST;

static ARGLIST argmaster = {0};
static int parser_status = PARSER_STS_KEY;
static int parser_target = -1;

/* 内部関数 */
static int  parser_expand_keylist(void);
static void parser_makevaluelist(ARGENTRY*, UCHAR);
static void parser_addvalue(ARGENTRY*, UWLNG);
static void parse_key(const char*);
static int  parse_val(const char*);

void parser_initialize(void)
{
    argmaster.argv = (ARGENTRY**)calloc(ARGENTRY_ADD_INCLEMENTNUM, sizeof(ARGENTRY*));
    argmaster.keymaxi = ARGENTRY_ADD_INCLEMENTNUM;
    atexit(parser_clear);
}

void parser_clear(void)
{
    int loop;
    fprintf(stderr, " [info] parser temp data clear.\n");
    for(loop = 0; loop < argmaster.keymaxi; ++loop)
    {
        if(argmaster.argv[loop] == (ARGENTRY*)NULL)
        {
            break;
        }else;

        fprintf(stderr, "  |- key %d  %s\n", loop, STRING_OK);
        if(argmaster.argv[loop]->valueptr != NULL)
        {
            free(argmaster.argv[loop]->valueptr);
            fprintf(stderr, "      |- value %s\n", STRING_OK);
        }else;
        free(argmaster.argv[loop]);
    }

    free(argmaster.argv);
    argmaster.keymaxi = 0;
    fprintf(stderr, " [info] parser end.\n");
}

/*! @fn parser_addkey(char*, char*, char)
 *  @brief プログラム引数を追加する
 */
int parser_addkey(char* key1, char* key2, char typec)
{
    int rtnsbi;
    int position;
    if(argmaster.keynumi == argmaster.keymaxi)
    {
        rtnsbi = parser_expand_keylist();
        if(rtnsbi == -1)
        {
            return -1;
        }
    }else;

    position = argmaster.keynumi;
    argmaster.argv[position] = (ARGENTRY*)calloc(1, sizeof(ARGENTRY));
    if(argmaster.argv[position] == (ARGENTRY*)NULL)
    {
        return -1;
    }else;
    argmaster.keynumi += 1;

    argmaster.argv[position]->key1 = key1;
    if(strlen(key1) != sizeof(char))
    {
        fprintf(stderr, " [program warning] \"-%s\" is out of rules. it is correct \"-%c\"\n", key1, key1[0]);
    }else;
    argmaster.argv[position]->key2 = key2;
    argmaster.argv[position]->typec = typec;
    argmaster.argv[position]->valueptr = (void*)NULL;
    argmaster.argv[position]->exist = FALSE;

    return 0;
}

/*! @fn parser_start(int, char**)
 *  @brief プログラム引数をパースしてデータ登録する
 */
void parser_start(int argnum, char **argv)
{
    int argloop;
    for(argloop = 1; argloop < argnum; ++argloop)
    {
        if(parser_status == PARSER_STS_VAL)
        {
            if(parse_val(argv[argloop]) == PARSER_STS_VAL)
            {
                continue;
            }else;

        }else;

        parser_status = PARSER_STS_KEY;
        parser_target = -1;
        parse_key(argv[argloop]);
    }
}

/*! @fn parser_isexist(char*)
 *  @brief パース結果からオプション指定されているかを取得する
 */
char parser_isexist(char *arg)
{
    int keyloop;
    ARGENTRY** entry = argmaster.argv;
    int found = -1;
    for(keyloop = 0; keyloop < argmaster.keynumi; ++keyloop)
    {
        if((entry[keyloop] == (ARGENTRY*)NULL))
        {
            break;
        }else;

        if((entry[keyloop]->key1 != NULL) &&
           (strncmp(arg, entry[keyloop]->key1, strlen(entry[keyloop]->key1)) == 0))
        {
            found = keyloop;
            break;
        }else;

        if((entry[keyloop]->key2 != NULL) &&
           (strncmp(arg, entry[keyloop]->key2, strlen(entry[keyloop]->key2)) == 0))
        {
            found = keyloop;
            break;
        }else;
    }

    if(found == -1)
    {
        return FALSE;
    }else;

    if(entry[found]->exist != TRUE)
    {
        return FALSE;
    }
    return TRUE;
}


/*! @fn    parser_getvalue(char*, void**, int*)
 *  @brief パース結果からオプション値を取得する
 */
char parser_getvalue(char *arg, void **outp, int* nump)
{
    int keyloop;
    ARGENTRY** entry = argmaster.argv;
    int found = -1;
    for(keyloop = 0; keyloop < argmaster.keynumi; ++keyloop)
    {
        if((entry[keyloop] == (ARGENTRY*)NULL))
        {
            break;
        }else;

        if((entry[keyloop]->key1 != NULL) &&
           (arg[0] == entry[keyloop]->key1[0]))
        {
            found = keyloop;
            break;
        }else;

        if((entry[keyloop]->key2 != NULL) &&
           (strncmp(arg, entry[keyloop]->key2, strlen(entry[keyloop]->key2)) == 0))
        {
            found = keyloop;
            break;
        }else;
    }

    if(found == -1)
    {
        return FALSE;
    }else;

    if(entry[found]->exist != TRUE)
    {
        return FALSE;
    }

    *outp = entry[found]->valueptr;
    *nump = entry[found]->valuenumi;
    return TRUE;
}

/* -------------------------------------------------------------------------- */

static int parser_expand_keylist(void)
{
    ARGENTRY **result;
    int loop;
    result = (ARGENTRY**)realloc(argmaster.argv, 
                        sizeof(ARGENTRY*) * (argmaster.keymaxi + ARGENTRY_ADD_INCLEMENTNUM));
    if(result == (ARGENTRY**)NULL)
    {
        return -1;
    }else;

    argmaster.argv = result;
    argmaster.keymaxi += ARGENTRY_ADD_INCLEMENTNUM;
    for(loop = 0; loop < ARGENTRY_ADD_INCLEMENTNUM; ++loop)
    {
        argmaster.argv[loop] = (ARGENTRY*)NULL;
    }
    return 0;
}

/*! @fn    parser_makevaluelist(ARGENTRY*, UCHAR)
 *  @brief プログラム引数のオプション値を記録する配列を新規確保する
 */
static void parser_makevaluelist(ARGENTRY *ptr, UCHAR typec)
{
    ptr->valuenumi = 0;
    switch(typec)
    {
        case ARGTYPE_STRING:
            ptr->valueptr = (void*)calloc(VALUEENTRY_ADD_INCREMENTNUM, sizeof(char*));
            ptr->valuemaxi = VALUEENTRY_ADD_INCREMENTNUM;
            break;
        case ARGTYPE_INT8  :
            ptr->valueptr = (void*)calloc(VALUEENTRY_ADD_INCREMENTNUM, sizeof(char));
            ptr->valuemaxi = VALUEENTRY_ADD_INCREMENTNUM;
            break;
        case ARGTYPE_INT16 :
            ptr->valueptr = (void*)calloc(VALUEENTRY_ADD_INCREMENTNUM, sizeof(short));
            ptr->valuemaxi = VALUEENTRY_ADD_INCREMENTNUM;
            break;
        case ARGTYPE_INT32 :
            ptr->valueptr = (void*)calloc(VALUEENTRY_ADD_INCREMENTNUM, sizeof(int));
            ptr->valuemaxi = VALUEENTRY_ADD_INCREMENTNUM;
            break;
        case ARGTYPE_INT64 :
            ptr->valueptr = (void*)calloc(VALUEENTRY_ADD_INCREMENTNUM, sizeof(long));
            ptr->valuemaxi = VALUEENTRY_ADD_INCREMENTNUM;
            break;
    }
}

#define HEXSTR_ONECHAR_BIT 4
#define HEX2STR_STRMAXLENGTH 16
/*!  @fn hexstr2val(const char*, UCHAR*)
 *   @brief Hex表記の文字列を数値に変換する
 */
static inline UWLNG hexstr2val(const char *str, UCHAR *result)
{
    int loop;
    UCHAR err = 0;
    UWLNG value = 0;

    UCHAR strlength = strlen(str) - 1;
    if(strlength > HEX2STR_STRMAXLENGTH)
    {
        strlength = HEX2STR_STRMAXLENGTH;
    }

    for(loop = strlength; loop >= 0; --loop)
    {
        if(str[loop] >= '0' && str[loop] <= '9')
        {
            value += (UWLNG)(str[loop] - '0') << (HEXSTR_ONECHAR_BIT * (strlen(str) - 1 - loop));
            continue;
        }
        if(str[loop] >= 'a' && str[loop] <= 'f')
        {
            value += (UWLNG)(str[loop] - 'a' + 10) << (HEXSTR_ONECHAR_BIT * (strlen(str) - 1 - loop));
            continue;
        }
        if(str[loop] >= 'A' && str[loop] <= 'F')
        {
            value += (UWLNG)(str[loop] - 'A' + 10) << (HEXSTR_ONECHAR_BIT * (strlen(str) - 1 - loop));
            continue;
        }
        err = 1;
    }

    *result = err;
    return value;
}

static void parser_addvalue(ARGENTRY *ptr, UWLNG value)
{
    if(ptr->valuenumi == ptr->valuemaxi)
    {
        fprintf(stderr, " [Error] Value Memory is Full !\n");
        return;
    }else;
    switch(ptr->typec)
    {
        case ARGTYPE_STRING:
            ((char**)ptr->valueptr)[ptr->valuenumi] = (char*)(value);
            break;
        case ARGTYPE_INT8  :
            ((char*)ptr->valueptr)[ptr->valuenumi] = (char)(value & 0xFF);
            break;
        case ARGTYPE_INT16 :
            ((short*)ptr->valueptr)[ptr->valuenumi] = (char)(value & 0xFFFF);
            break;
        case ARGTYPE_INT32 :
            ((int*)ptr->valueptr)[ptr->valuenumi] = (char)(value & 0xFFFF);
            break;
        case ARGTYPE_INT64 :
            ((long*)ptr->valueptr)[ptr->valuenumi] = (char)(value & 0xFFFF);
            break;
        default: 
            fprintf(stderr, " [Error] argument type is not specified !\n");
            return;
    }
    ptr->valuenumi += 1;
    ptr->exist = TRUE;
    return;
}


/*! @fn parse_key(const char*)
 *  @brief プログラム引数のオプションキーをパースする
 */
static void parse_key(const char *arg)
{
    int keyloop;
    ARGENTRY** entry = argmaster.argv;

    if(strlen(arg) <= 1)
    {
        return;
    }else;

    if(strncmp(arg, "-", 1) == 0)
    {
        for(keyloop = 0; keyloop < argmaster.keynumi; ++keyloop)
        {
            if((entry[keyloop] == (ARGENTRY*)NULL))
            {
                break;
            }
            else if((strlen(entry[keyloop]->key1) == 0))
            {
                continue;
            }

            if(strncmp(&arg[1], entry[keyloop]->key1, sizeof(char)) != 0)
            {
                continue;
            }

            parser_target = keyloop;
            break;
        }
    }
    if(strncmp(arg, "--", 2) == 0)
    {
        for(keyloop = 0; keyloop < argmaster.keynumi; ++keyloop)
        {
            if((entry[keyloop] == (ARGENTRY*)NULL))
            {
                break;
            }
            else if((strlen(entry[keyloop]->key2) == 0))
            {
                continue;
            }
            if(strncmp(&arg[2], entry[keyloop]->key2, strlen(entry[keyloop]->key2)) != 0)
            {
                continue;
            }

            parser_target = keyloop;
            break;
        }
    }else;

    if(parser_target == -1)
    {
        fprintf(stderr, " [Warning] Unknown type %s\n", arg);
        return ;
    }else;

    if(entry[parser_target]->typec != ARGTYPE_NOVALUE)
    {
        parser_status = PARSER_STS_VAL;
    }else{
        entry[parser_target]->exist = TRUE;
    }
}


/*! @fn parse_val(const char*)
 *  @brief プログラム引数のオプション値をパースする
 */
static int parse_val(const char *arg)
{
    UWLNG valuew;
    UCHAR resultc;
    ARGENTRY *entry;
    
    if((strlen(arg) >= 2) && 
       ((strncmp(arg, "-", 1) == 0) || (strncmp(arg, "--", 12 == 0))))
    {
        return PARSER_STS_KEY;
    }else;

    if(parser_target == -1)
    {
        return PARSER_STS_KEY;
    }else;

    entry = argmaster.argv[parser_target];

    valuew = hexstr2val(arg, &resultc);
    if(resultc != 0)
    {
        fprintf(stderr, " [Warning] option %s, invalid value : %s\n", 
                                        entry->key2, arg);
        return PARSER_STS_VAL;
    }else;

    if(entry->valueptr == (void*)NULL)
    {
        parser_makevaluelist(entry, entry->typec);
    }else;

    parser_addvalue(entry, valuew);

    return PARSER_STS_VAL;
}

