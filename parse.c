#include "global.h"
#include "scan.h"
#include "parse.h"
#include "util.h"


static ast_node* declaration_list();
static ast_node* declaration();
static ast_node* param_list();
static ast_node* param();
static ast_node* compound_stmt();
static ast_node* local_declarations();
static ast_node* statement_list();
static ast_node* var_declaration();
static ast_node* statement();
static ast_node* selection_statement();
static ast_node* iteration_statement();
static ast_node* return_statement();
static ast_node* expression_statement();
static ast_node* expression();
static ast_node* simple_expression(ast_node*);
static ast_node* additive_expression(ast_node*);
static ast_node* var_and_call();
static ast_node* term(ast_node*);
static ast_node* factor(ast_node*);
static ast_node* args();
static ast_node* arg_list();

static token tk;

#define SYNTAX_ERR(x) { fprintf(stderr,">>>syntax error at line%d: %s\n",line_no,(x) );}
#define PRINT_TOKEN(x) { fprintf(stderr,"\t\tunexpected token %s\n",(x));}


static exp_type get_exp_type()
{
    exp_type e = EXP_VOID;
    switch(tk.k)
    {
    case TK_VOID:
        e = EXP_VOID;
        tk = get_token();
        break;
    case TK_INT:
        e = EXP_INT;
        tk = get_token();
        break;
    default:
        SYNTAX_ERR("expect a type identifier but got other token");
        PRINT_TOKEN( TK_TO_STR(tk.k) );
        break;

    }

    return e;
}

static void match(token_type expected_type)
{
    if(tk.k == expected_type)
        tk = get_token();
    else
    {
        
        SYNTAX_ERR("unexpected token ");
        PRINT_TOKEN( TK_TO_STR(tk.k) );
    }
}

static ast_node* declaration_list()
{
    ast_node* t;
    ast_node* p;

    t = declaration();
    p = t;

    while( tk.k != TK_EOF )
    {
        ast_node *q;
        q = declaration();
        p->sibling = q;
        p = q;
     }

    return t;
}

static ast_node* declaration()
{
    ast_node* t;
    string  identifier;
    
    exp_type e = get_exp_type();
    identifier = tk.u.literal;
    match( TK_ID ); /* check whether it is a token */

    switch(tk.k)
    {
    case TK_SEMI:/* variable declaration */
        t = new_ast_node(DEC_PATTERN, DEC_SCALAR );
        if(t!=NULL)
        {
            t->meta_data.scalar_dec.name = identifier;
            t->meta_data.scalar_dec.var_type = e;
            t->meta_data.scalar_dec.is_param = 0;
        }
        match(TK_SEMI);
        break;
    case TK_LSQ: /* array declaration */
        t = new_ast_node(DEC_PATTERN, DEC_ARRAY);
        if(t!=NULL)
        {
            t->meta_data.array_dec.var_type = e;
            t->meta_data.array_dec.name = identifier;
            t->meta_data.array_dec.is_param = 0;
        }
        match(TK_LSQ);
        if(t!=NULL)
            t->meta_data.array_dec.size = tk.u.num;
        match(TK_NUM);
        match(TK_RSQ);
        match(TK_SEMI);
        break;
    case TK_LPAREN: /* function declaration */
        t = new_ast_node(DEC_PATTERN, DEC_FUNC);
        if(t!=NULL)
        {
            t->meta_data.func_dec.name = identifier;
            t->meta_data.func_dec.return_type = e;
        }
        match(TK_LPAREN);
        if(t!=NULL)
        {
            t->child[0] = param_list();
        }
        match(TK_RPAREN);
        if(t!=NULL)
        {
            t->child[1] = compound_stmt();
        }
        break;
    default:
        SYNTAX_ERR("unexpected token ");
        PRINT_TOKEN( TK_TO_STR(tk.k) );
        tk = get_token(); /* continue parsing */
        break;
    }
    
}
static ast_node* var_declaration()
{
    ast_node* t;
    string  identifier;
    
    exp_type e = get_exp_type();
    identifier = tk.u.literal;
    match( TK_ID ); /* check whether it is a token */

    switch(tk.k)
    {
    case TK_SEMI:/* variable declaration */
        t = new_ast_node(DEC_PATTERN, DEC_SCALAR);
        if(t!=NULL)
        {
            t->meta_data.scalar_dec.name = identifier;
            t->meta_data.scalar_dec.var_type = e;
            t->meta_data.scalar_dec.is_param = 0;
        }
        match(TK_SEMI);
        break;
    case TK_LSQ: /* array declaration */
        t = new_ast_node(DEC_PATTERN, DEC_ARRAY);
        if(t!=NULL)
        {
            t->meta_data.array_dec.var_type = e;
            t->meta_data.array_dec.name = identifier;
            t->meta_data.array_dec.is_param = 0;
        }
        match(TK_LSQ);
        if(t!=NULL)
            t->meta_data.array_dec.size = tk.u.num;
        match(TK_NUM);
        match(TK_RSQ);
        match(TK_SEMI);
        break;   
    default:
        SYNTAX_ERR("unexpected token ");
        PRINT_TOKEN( TK_TO_STR(tk.k) );
        tk = get_token(); /* continue parsing */
        break;
    }   
}

static ast_node* param_list()
{
   if(tk.k == TK_VOID)
    {
        match(TK_VOID);
        return NULL;
    }

    ast_node* t = param();
    ast_node* p = t;
    ast_node* node;
    if(t==NULL)
    {
        return NULL;
    }
    while( tk.k==TK_COMMA  )
    {
        match(TK_COMMA);
        node = param();
        p->sibling = node;
        p = node;
    }

    return t;
}

static ast_node* param()
{
    ast_node *node;
    exp_type e;
    string identifier;

    e = get_exp_type();

    identifier = tk.u.literal;
    match(TK_ID);
    if( tk.k == TK_LSQ )
    {
        match(TK_LSQ);
        match(TK_RSQ);
        node = new_ast_node(DEC_PATTERN, DEC_ARRAY);
        if(node)
        {
            node->meta_data.array_dec.name = identifier;
            node->meta_data.array_dec.var_type = e;
            node->meta_data.array_dec.is_param = 1;
        }
    }
    else
    {
        node = new_ast_node(DEC_PATTERN, DEC_SCALAR);
        if(node)
        {
            node->meta_data.scalar_dec.name = identifier;
            node->meta_data.scalar_dec.var_type = e;
            node->meta_data.scalar_dec.is_param = 1;
        }
    }

    return node;
}


static ast_node* compound_stmt()
{
    ast_node* t;
    match(TK_LBRACE);
    if( ( tk.k!=TK_RBRACE ) && (t = new_ast_node(STMT_PATTERN, STMT_COMPOUND) ))
    {
        if(tk.k == TK_INT || tk.k ==TK_VOID )
        {
            /* local variable declaration */
            t->child[0] = local_declarations();
        }
        if(tk.k!=TK_RBRACE)
        {
            t->child[1] = statement_list();
        }
    }

    match(TK_RBRACE);
    return t;

}

static ast_node* statement_list()
{
    ast_node *t, *p;
    t = statement();
    if(t!=NULL)
    {
        p = t;
        while( tk.k != TK_RBRACE  )
        {
            ast_node *new_nd = statement();
            p->sibling = new_nd;
            p = new_nd;
        }
    }
    return t;
    
}

static ast_node* statement()
{
    ast_node *t = NULL;
    switch( tk.k )
    {
    case TK_IF:
        /* if statement */
        t = selection_statement();
        break;
    case TK_WHILE:
        /* while statement */
        t = iteration_statement();
        break;
    case TK_RETURN:
        t = return_statement();
        break;
    case TK_LBRACE:
        t = compound_stmt();
        break;
    case TK_ID:
    case TK_SEMI:
    case TK_LPAREN:
        t = expression_statement();
        break;
    default:
        SYNTAX_ERR("unexpected token ");
        PRINT_TOKEN( TK_TO_STR(tk.k) );
        tk = get_token(); /* continue parsing */
        break;
    }

    return t;
}

static ast_node* selection_statement()
{
    ast_node* t;
    ast_node* expr=NULL;
    ast_node* if_stmt=NULL;
    ast_node* else_stmt=NULL;

    match(TK_IF);
    match(TK_LPAREN);
    expr = expression();
    match(TK_RPAREN);
    if_stmt = statement();

    if(tk.k == TK_ELSE)
    {
        match(TK_ELSE);
        else_stmt = statement();
    }

    t = new_ast_node( STMT_PATTERN, STMT_SELECTION  );
    if(t!=NULL)
    {
        t->child[0] = expr;
        t->child[1] = if_stmt;
        t->child[2] = else_stmt;
    }

    return t;

}

static ast_node* iteration_statement()
{
    ast_node *t = NULL;
    ast_node *expr = NULL;
    ast_node *stmt = NULL;
    match(TK_WHILE);
    match(TK_LPAREN);
    expr = expression();
    match(TK_RPAREN);
    stmt = statement();

    t = new_ast_node(STMT_PATTERN, STMT_ITERATION);
    if(t!=NULL)
    {
        t->child[0] = expr;
        t->child[1] = stmt;
    }

    return t;
}

static ast_node* return_statement()
{
    ast_node *t;
    ast_node *expr;

    match(TK_RETURN);
    t = new_ast_node(STMT_PATTERN, STMT_RETURN);
    if(tk.k!=TK_SEMI)
        expr = expression();
    match(TK_SEMI);
    if(t!=NULL)
        t->child[0] = expr;

    return t;
}

static ast_node* expression_statement()
{
    ast_node *t = NULL;
    if(tk.k!=TK_SEMI)
        t = expression();

    match(TK_SEMI);
    return t;
}

static ast_node* expression()
{
    ast_node *t;
    ast_node *lval=NULL;
    ast_node *rval=NULL;
    int lval_exist = 0;
    if( tk.k==TK_ID )
    {
        lval = var_and_call();
        lval_exist = 1;
    }
    if( (lval_exist==1)&&(tk.k==TK_ASSIGN)  )
    {
        if( (lval!=NULL)&&(lval->pattern==EXP_PATTERN)&&
           (lval->detail_pattern.exp==EXP_ID)  )
        {
            match(TK_ASSIGN);
            rval = expression();
            t = new_ast_node( EXP_PATTERN, EXP_ASSIGN  );
            if(t!=NULL)
            {
                t->child[0] = lval;
                t->child[1] = rval;
            }
        }
        else
        {
            SYNTAX_ERR("attempt to assign to something not a lval.");   
            tk = get_token();
        }
    }
    else
        t = simple_expression(lval);

    return t;
}

static ast_node *simple_expression(ast_node* already_parse)
{
    
    ast_node *t;
    ast_node *lexpr = NULL;
    ast_node *rexpr = NULL;
    token_type operator;

    lexpr = additive_expression(already_parse);

    if( (tk.k==TK_LT)||(tk.k==TK_GT)||(tk.k==TK_NE)||(tk.k==TK_LTE)
      ||(tk.k==TK_GTE)||(tk.k==TK_EQ) )
    {
        operator = tk.k;
        match(tk.k);
        rexpr = additive_expression(NULL);
        t = new_ast_node( EXP_PATTERN, EXP_OP  );
        if(t!=NULL)
        {
            t->child[0] = lexpr;
            t->child[1] = rexpr;
            t->meta_data.op_exp.op = operator;
        }
    }
    else
        t = lexpr;

    return t;
}

static ast_node* additive_expression(ast_node* already_parse)
{
    ast_node *t;
    ast_node *n;
    t = term(already_parse);
    while( (tk.k==TK_PLUS)||(tk.k==TK_MINUS)  )
    {
        n = new_ast_node( EXP_PATTERN, EXP_OP  );
        if(n!=NULL)
        {
            n->child[0] = t;
            n->meta_data.op_exp.op = tk.k;
            t = n;
            match(tk.k);
            t->child[1] = term(NULL);
        }
        
    }

    return t;
}

static ast_node* term(ast_node* already_parse)
{
    ast_node* t;
    ast_node* n;
    t = factor(already_parse);
    while( (tk.k==TK_MUL)||(tk.k==TK_DIV)  )
    {
        n = new_ast_node(EXP_PATTERN, EXP_OP);
        n->child[0] = t;
        n->meta_data.op_exp.op = tk.k;
        match(tk.k);
        t = n;
        t->child[1] = factor(NULL);
    }
    return t;
}

static ast_node* var_and_call()
{
    ast_node *expr = NULL;
    ast_node *t = NULL;
    string identifier;
    if(tk.k==TK_ID)
    {
        identifier = tk.u.literal;
        match(TK_ID);
    }

    if(tk.k==TK_LSQ)
    {
        match(TK_LSQ);
        expr = expression();
        match(TK_RSQ);
        t = new_ast_node( EXP_PATTERN, EXP_ID );
        if(t!=NULL)
        {
            t->child[0] = expr;
            t->meta_data.id_exp.name = identifier;
        }
    }
    else if(tk.k==TK_LPAREN)
    {
        match(TK_LPAREN);
        ast_node* args_node = args();
        match(TK_RPAREN);
        t = new_ast_node( EXP_PATTERN, EXP_CALL );
        if(t!=NULL)
        {
            t->child[0] = args_node;
            t->meta_data.call_exp.name = identifier;
        }
    }
    else
    {
        t = new_ast_node( EXP_PATTERN, EXP_ID );
        if(t!=NULL)
        {
            t->meta_data.id_exp.name = identifier;   
        }
    }

    return t;
}

static ast_node* local_declarations()
{
    ast_node *t, *p;
    t = NULL;
    if(tk.k == TK_INT || tk.k == TK_VOID )
    {
        t = var_declaration();
    }
    if(t!=NULL)
    {
        p = t;
        while( tk.k==TK_INT || tk.k==TK_VOID )
        {
            ast_node* new_nd = var_declaration();
            if(new_nd!=NULL)
            {
                p->sibling = new_nd;
                p = new_nd;
            }
        }   
    }
    return t;
}

static ast_node *factor(ast_node* already_parse)
{
    if(already_parse!=NULL)
        return already_parse;
    ast_node *t;
    if(tk.k==TK_LPAREN)
    {
        match(TK_LPAREN);
        t = expression();
        match(TK_RPAREN);
    }
    else if(tk.k==TK_NUM)
    {
        t = new_ast_node(EXP_PATTERN, EXP_CONST);
        if(t!=NULL)
            t->meta_data.const_exp.data = tk.u.num;
        match(TK_NUM);
    }
    else if(tk.k==TK_ID)
    {
        t = var_and_call();
    }
    else
    {
        SYNTAX_ERR("unexpected token ");
        PRINT_TOKEN( TK_TO_STR(tk.k) );
        tk = get_token(); /* continue parsing */
    }

    return t;

}

static ast_node* args()
{
    if(tk.k==TK_LPAREN)
        return NULL;
    ast_node* t;
    t = arg_list();
    return t;
}

static ast_node* arg_list()
{
    ast_node* t;
    ast_node* n;
    ast_node* p;
    t = expression();
    p = t;
    while( tk.k==TK_COMMA  )
    {
        match(TK_COMMA);
        n = expression();
        if( p!=NULL )
        {
            p->sibling = n;
            p = n;
        }
    }

    return t;

}

ast_node* parse()
{
    ast_node *tree;

    tk = get_token();
    tree = declaration_list();

    return tree;
}

