
#ifndef _STRUCT_H
#define _STRUCT_H

typedef enum _token_type {

    TK_NULL,
    TK_EOF,TK_ERR,

    TK_IF, TK_ELSE, TK_INT, TK_RETURN, TK_VOID, TK_WHILE, /* reserved words */

    TK_ID, TK_NUM,

    TK_PLUS, TK_MINUS, TK_MUL, TK_DIV, TK_LT, TK_GT, TK_ASSIGN, TK_NE, TK_SEMI, TK_COMMA,
    TK_LPAREN, TK_RPAREN, TK_LSQ, TK_RSQ, TK_LBRACE, TK_RBRACE, TK_LTE, TK_GTE, TK_EQ, /* operators */

    TK_SIZ

}token_type;



typedef struct _string{
    char *s;
    int len;
}string;

typedef struct _token{
    token_type k;
    union
    {
        string literal;
        int num;
    }u;

}token;

typedef enum _exp_type{ EXP_VOID, EXP_INT } exp_type;

typedef enum _node_pattern{ STMT_PATTERN, EXP_PATTERN, DEC_PATTERN } node_pattern;
typedef enum _dec_pattern{ DEC_SCALAR, DEC_ARRAY, DEC_FUNC  } dec_pattern;
typedef enum _exp_pattern{ EXP_ID, EXP_OP, EXP_ASSIGN,EXP_CALL, EXP_CONST  } exp_pattern;
typedef enum _stmt_pattern{ STMT_SELECTION, STMT_ITERATION, STMT_COMPOUND,STMT_RETURN,
                             } stmt_pattern;



typedef struct _ast_node
{
    struct _ast_node* child[3];
    struct _ast_node* sibling;    
    
    node_pattern pattern;

    union{
        dec_pattern dec;
        exp_pattern exp;
        stmt_pattern stmt;
    }detail_pattern;

    union{
        struct{ string name; exp_type var_type;int is_param;   }scalar_dec;
        struct{ string name; exp_type var_type; int size; int is_param;  }array_dec;
        struct{ string name; exp_type return_type; }func_dec;
        struct{ string name;  }id_exp;
        struct{ token_type op;  }op_exp;
        struct{ string name;  }call_exp;
        struct{ int data;  }const_exp;
    }meta_data;

    token op;
    int val;
    string name;

}ast_node;

#endif // _STRUCT_H
