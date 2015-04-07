#ifndef _UTIL_H
#define _UTIL_H
#include "struct.h"
#define PERROR(x) { fprintf(stderr, "%s\n", (x) );}

/* used for debug */
extern char* token_type_corresponding_str[];

#define TK_TO_STR(x) token_type_corresponding_str[(x)]


ast_node* new_ast_node( node_pattern nd, int detail );

#endif // _UTIL_H
