#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "svosdef.h"
#include "parser.h"

void showHelp(void)
{
    fprintf(stdout, " ---- help ---- \n");
    fprintf(stdout, " Usage: main [OPTION]... \n");
    fprintf(stdout, " Example: main -c 00 0f 20 2f -t 8 -f 3448\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "Option:\n");
    fprintf(stdout, "  -c  --core         core# (hex) 00 01 02 ... ff\n");
    fprintf(stdout, "  -f  --funid        trace function id (hex) 0000 0001 0002... ffff\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "Author: \n");
    fprintf(stdout, "  T.Ando  (ERR/INI) \n");
}

int main(int argi, char *argv[])
{
    fprintf(stderr, " [info] parser (Build : %s)\n", __DATE__);
    parser_initialize();

    parser_addkey("h", "help", ARGTYPE_NOVALUE);
    parser_addkey("c", "core", ARGTYPE_INT8);

    parser_start(argi, (char**)argv);


    if(parser_isexist("help") == TRUE)
    {
        showHelp();
        return 0;
    }else;

    char *nump = (char*)NULL;
    int sizei = 0;
    if(parser_getvalue("core", (void**)&nump, &sizei) == TRUE)
    {
        int loop;
        printf(" core filter :");
        for(loop = 0; loop < sizei; ++loop)
        {
            printf(" %02x", (UCHAR)nump[loop]);
        }
        printf("\n");
    }else{
        fprintf(stderr, "Core filter not defined\n");
    }

    return 0;
}
