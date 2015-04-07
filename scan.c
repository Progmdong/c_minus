#include"global.h"
#include"struct.h"
#include"scan.h"
int line_no = 1; /* current line number */
int line_siz = 0; /* current line's size */
int line_idx = 0; /* position in current line */
char line_txt[ 1024 ];
char token_str[64];
char peek = ' ';
typedef struct _reserved_wd
{
    char *literal;
    token_type tk;
}reserved_wd;
typedef enum _lex_state{ LS_START, LS_NUM, LS_ID, LS_DIV, LS_COMMENT1, LS_COMMENT2, LS_NE,
                        LS_LT, LS_GT, LS_EQ, LS_ERR, LS_DONE } lex_state;
 reserved_wd rwds[] =
 {
     {"", 0},
     {"", 0},
     {"if", TK_IF},  // key = 2
     {"int", TK_INT},    // key = 3
     {"else", TK_ELSE},  // key = 4
     {"while", TK_WHILE},  //key = 5
     {"return", TK_RETURN}, // key = 6
     {"", 0},
     {"", 0},
     {"void", TK_VOID}  // key = 9
 };
#define RWD_MAX_HASH_VAL 9
/* hash mesh come from Benjamin Fowler */
static unsigned int hash( const char* str, unsigned int len )
{
    static unsigned char asso_vals[] =
    {
        10,10,10,10,10,10,10,10,10,10,
        10,10,10,10,10,10,10,10,10,10,
        10,10,10,10,10,10,10,10,10,10,
        10,10,10,10,10,10,10,10,10,10,
        10,10,10,10,10,10,10,10,10,10,
        10,10,10,10,10,10,10,10,10,10,
        10,10,10,10,10,10,10,10,10,10,
        10,10,10,10,10,10,10,10,10,10,
        10,10,10,10,10,10,10,10,10,10,
        10,10,10,10,10,10,10,10,10,10,
        5, 0, 0,10,10, 0,10,10,10,10,
        0,10,10,10, 0,10, 0,10, 0, 0,
        10,10,10,10,10,10,10,10,10,10,
        10,10,10,10,10,10,10,10,10,10,
        10,10,10,10,10,10,10,10,10,10,
        10,10,10,10,10,10,10,10,10,10,
        10,10,10,10,10,10,10,10,10,10,
        10,10,10,10,10,10,10,10,10,10,
        10,10,10,10,10,10,10,10,10,10,
        10,10,10,10,10,10,10,10,10,10,
        10,10,10,10,10,10,10,10,10,10,
        10,10,10,10,10,10,10,10,10,10,
        10,10,10,10,10,10,10,10,10,10,
        10,10,10,10,10,10,10,10,10,10,
        10,10,10,10,10,10,10,10,10,10,
        10,10,10,10,10,10
    };
    return len + asso_vals[ (unsigned char)str[len-1] ] + asso_vals[ (unsigned char)str[0] ];
}
reserved_wd* is_reserved( const char *str, unsigned int len )
{
    unsigned int key = hash( str, len );
    if( key>=0 && key <= RWD_MAX_HASH_VAL )
    {
        const char *s = rwds[ key ].literal;
        if( !strcmp( str, s ) )
        {
            return &rwds[key];
        }
    }
    return NULL;
}
static char next_char()
{
    if( line_idx>= line_siz )
    {
        if( fgets(line_txt, 1024, src_fd) )
        {
            line_siz = strlen(line_txt);
            line_idx=0;
            line_no++;
        }
        else return EOF;
  
    }
    return line_txt[ line_idx++ ];
}
static void undo_next_char()
{
    --line_idx;
}
token get_token()
{
    #define RET {peek=' ';return tk;}
    lex_state state = LS_START;
    token tk;
    int tk_idx = 0;
    //skip white spaces
    while( peek ==' ' || peek =='\t' || peek =='\n')
        peek = next_char();
    if(peek==EOF)
    {
        tk.k = TK_EOF;
        return tk;
    }
    while( state!= LS_DONE )
    {
        switch(state)
        {
            case LS_START:
            if( isdigit(peek) )
            {
                state = LS_NUM;
                token_str[tk_idx++] = peek;
            }
            else if( isalpha(peek) )
            {
                state = LS_ID;
                token_str[tk_idx++] = peek;
            }
            else if( peek=='=' )
            {
                state = LS_EQ;
            }
            else if( peek=='>' )
            {
                state = LS_GT;
            }
            else if( peek=='<' )
            {
                state = LS_LT;
            }
            else if(peek=='!')
            {
                state = LS_NE;
            }
            else if(peek=='/')
            {
                state = LS_DIV;
            }
            else
            {
                switch(peek)
                {
                    case '+':
                    tk.k = TK_PLUS;
                    break;
                    case '-':
                    tk.k = TK_MINUS;
                    break;
                    case '*':
                    tk.k = TK_MUL;
                    break;
                    case ',':
                    tk.k = TK_COMMA;
                    break;
                    case ';':
                    tk.k = TK_SEMI;
                    break;
                    case '[':
                           tk.k = TK_LSQ;
                           break;
                           case ']':
                    tk.k = TK_RSQ;
                    break;
                    case '{':
                           tk.k = TK_LBRACE;
                           break;
                           case '}':
                    tk.k = TK_RBRACE;
                    break;
                    case '(':
                           tk.k = TK_LPAREN;
                           break;
                           case ')':
                    tk.k = TK_RPAREN;
                    break;
                    default:
                    tk.k = TK_ERR;
                    break;
                }
                RET;
            }
            break;
        case LS_NUM:
            if( !isdigit(peek) )
            {
                token_str[tk_idx] = '\0';
                tk.k = TK_NUM;
                tk.u.num = atoi( token_str );
                undo_next_char();
                RET;
            }
            else
            {
                token_str[tk_idx++] = peek;
            }
            break;
        case LS_ID:
            if( isdigit(peek) || isalpha(peek) )
            {
                token_str[tk_idx++] = peek;
            }
            else
            {
                token_str[tk_idx] = '\0';
                reserved_wd *wd = is_reserved( token_str, tk_idx );
                if( wd!= NULL )
                {
                    tk.k = wd->tk;
                    undo_next_char();
                    RET;
                }
                tk.k = TK_ID;
                tk.u.literal.s = (char*)malloc( tk_idx+1 );
                strcpy( tk.u.literal.s, token_str );
                tk.u.literal.len = tk_idx - 1; /* except \0 */
                undo_next_char();
                RET;
            }
            break;
        case LS_EQ:
            if( peek=='=' )
            {
                tk.k = TK_EQ;
                RET;
            }
            else
            {
                tk.k = TK_ASSIGN;
                undo_next_char();
                RET;
            }
            break;
        case LS_GT:
            if( peek=='=' )
            {
                tk.k = TK_GTE;
                RET;
            }
            else
            {
                tk.k = TK_GT;
                undo_next_char();
                RET;
            }
            break;
        case LS_LT:
            if( peek=='=' )
            {
                tk.k = TK_LTE;
                RET;
            }
            else
            {
                tk.k = TK_LT;
                undo_next_char();
                RET;
            }
            break;
        case LS_DIV:
            if( peek=='*' )
            {
                state = LS_COMMENT1;
            }
            else
            {
                tk.k = TK_DIV;
                undo_next_char();
                RET;
            }
            break;
        case LS_COMMENT1:
            if( peek=='*' )
            {
                state = LS_COMMENT2;
            }
            break;
        case LS_COMMENT2:
            if( peek=='/' )
            {
                state = LS_START;
            }
            else
            {
                tk.k = TK_ERR;
                undo_next_char();
                RET;
            }
            break;
        }
        peek = next_char();
        if(peek==EOF)
        {
            tk.k = TK_EOF;
            return tk;
        }
        }
    tk.k = TK_ERR;
    return tk;
#undef RET
}
