#include "global.h"
#include "struct.h"
#include "scan.h"
#include "util.h"
#include "parse.h"

FILE *src_fd;

int main(int argc, char **argv)
{
    close(0);
    src_fd = fopen(argv[1],"r");
    parse();
    return 0;
}
