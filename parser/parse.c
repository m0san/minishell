//
// Created by moboustt on 4/4/2021.
//

#include "../minishell.h"
#include "parser.h"

/*
 *  the parser is the part of our Command Line Interpreter that calls the lexical scanner to retrieve tokens,
 *  then constructs an Abstract Syntax Tree, or AST, out of these tokens.
 *  This AST is what we'll pass to the executor to be, well, executed.
 */



t_error *catch_errors(t_parser *p, t_error *error) {
    if (p->cur_token->type  == illegal || p->peek_token->type  == illegal)
        set_error(error, "Illegal Syntax!");
    if (p->cur_token->type == right || p->cur_token->type == left || p->cur_token->type == right_append) {
        if (!expect_peek(p, *p->lexer, arg))
            set_error(error, ERR1);
    } else if (p->cur_token->type == _pipe) {
        if (expect_peek(p, *p->lexer, _pipe))
            set_error(error, ERR1);
    } else if (p->cur_token->type == semicolon) {
        if (expect_peek(p, *p->lexer, semicolon))
            set_error(error, ERR1);
    }
    return error;
}

void replace_token(t_parser *p) {
    if (p->cur_token->type == tilde)
        p->cur_token->literal = get_var(g_envp,"HOME");
}

t_node *parse_command(t_node *ast_node, t_parser *p) {
    if (!p->cur_token)
        return NULL;
    ast_node = new_node(NODE_COMMAND);
    if (!ast_node) {
        free_token(p->cur_token);
        return NULL;
    }
    t_error *error = malloc(sizeof(t_error));
    ft_memset(error, 0, sizeof(t_error));
    while (p->cur_token->type != end_of) {
        replace_token(p);
        catch_errors(p, error);
        if (catch_errors(p, error)->is_error) {
            printf("%s \n", error->error_msg);
            return NULL;
        }
        t_node *arg = new_node(NODE_ARG);
        if (!arg) {
            // free node tree -> ast_node;
            // free the token -> tok;
            return NULL;
        }
        set_node_val_str(arg, p->cur_token->literal, p->cur_token->type);
        add_child_node(ast_node, arg);
        //tok->str__(tok);
        //free_token(tok);
        next_token_p(p);
    }
    return ast_node;
}

void free_token(t_token *tok) {
    if (tok->literal)
        free(tok->literal);
    free(tok);
}

void next_token_p(t_parser *p) {
    p->cur_token = p->peek_token;
    p->peek_token = next_token(p->lexer);
}

t_parser *new_parser(t_lexer *l) {
    t_parser *p = malloc(sizeof(t_parser));
    p->lexer = l;
    next_token_p(p);
    next_token_p(p);
    return p;
}

t_bool cur_token_is(t_parser *p, t_token_type t) {
    return p->cur_token->type == t;
}

t_bool peek_token_is(t_parser *p, t_token_type t) {
    return p->peek_token->type == t;
}


t_bool expect_peek(t_parser *p, t_lexer l, t_token_type t) {
    if (peek_token_is(p, t)) {
        next_token(&l);
        return true;
    } else
        return false;
}