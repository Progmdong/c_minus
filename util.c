#include"util.h"
#include"global.h"

char* token_type_corresponding_str[TK_SIZ] = { "","TK_EOF","TK_ERR","TK_IF","TK_ELSE","TK_INT",
                                        "TK_RETURN","TK_VOID", "TK_WHILE", "TK_ID", "TK_NUM",
                                        "TK_PLUS","TK_MINUS","TK_MUL","TK_DIV","TK_LT","TK_GT",
                                        "TK_ASSIGN","TK_NE","TK_SEMI","TK_COMMA","TK_LPAREN",
                                        "TK_RPAREN", "TK_LSQ", "TK_RSQ","TK_LBRACE","TK_RBRACE",
                                        "TK_LTE","TK_GTE","TK_EQ"};


static ast_node* alloc_node()
{
    ast_node* node;
    node = (ast_node*)malloc( sizeof(ast_node) );
    if(!node)
    {
       PERROR("out of memory at line"); 
    }
    else
    {
        int i;
        for(i=0; i<3; ++i)node->child[i]=NULL;
        node->sibling = NULL;
    }

    return node;
}

ast_node* new_ast_node( node_pattern np, int detail )
{
    ast_node* node;
    node = alloc_node();
    switch(np)
    {
    case DEC_PATTERN:
        node->pattern = np;
        node->detail_pattern.dec = (dec_pattern)detail;
        break;
    case STMT_PATTERN:
        node->pattern = np;
        node->detail_pattern.dec = (stmt_pattern)detail;
        break;
    case EXP_PATTERN:
        node->pattern = np;
        node->detail_pattern.dec = (exp_pattern)detail;
        break;
    default:
        break;
    }

    return node;
}

