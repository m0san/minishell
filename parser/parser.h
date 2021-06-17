//
// Created by Mohammad  Boustta on 4/26/21.
//

#ifndef MINISHELL_PARSER_H
#define MINISHELL_PARSER_H

#include <stdlib.h>
#include <stdio.h>
#include "../errors/error.h"
#include "../lexer/lexer.h"


typedef struct s_parser {
    t_lexer *lexer;
    t_token *cur_token;
    t_token *peek_token;
}               t_parser;



t_bool cur_token_is(t_parser *p, t_token_type t);
t_bool peek_token_is(t_parser *p, t_token_type t);
t_bool expect_peek(t_parser *p, t_lexer l, t_token_type t);
void next_token_p(t_parser *p);
t_parser *new_parser(t_lexer *l);
t_error *catch_errors(t_parser *p, t_error *error);

#endif //MINISHELL_PARSER_H
