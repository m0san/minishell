#include "lexer.h"

void handle_double_quotes(t_lexer *l, char *s, int *i, int *s_index, int *pos)
{
	(*pos) = l->position;
	read_and_parse_double_quoted(l, s, s_index);
	(*i) += (l->position - (*pos));
}

void handle_single_quote(t_lexer *l, char *s, int *i, int *s_index, int *pos)
{
	(*pos) = l->position;
	read_single_quoted(l, l->ch, s, s_index);
	(*i) += (l->position - (*pos));
}

void init_parse_quoted_vars(t_lexer *l, char quote, char **s, int **infos)
{
	(*s) = malloc(1024);
	ft_bzero((*s), 1024);
	(*infos) = has_next_quote(l, quote);
}

void handle_alphanum_identifier(t_lexer *l, int pos, char **s, int *i, int *s_index) {
	pos = l->position;
	(*s) = strcat((*s), read_identifier(l, 0));
	(*i) += (l->position - pos);
	(*s_index) += (l->position - pos);
}

void handle_escape(t_lexer *l, char *s, int *i, int *s_index) {
	next_char(l);
	s[(*s_index)++] = l->ch;
	next_char(l);
	(*i) += 2;
}
