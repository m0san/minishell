/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moboustt <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/16 20:48:38 by moboustt          #+#    #+#             */
/*   Updated: 2021/06/16 20:49:58 by moboustt         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_token *handle_identifier_with_no_quotes(t_lexer *l, t_token *tok);

t_token *next_token(t_lexer *l) {
	t_token *tok;

	tok = malloc(sizeof(t_token));
	tok->lexer = l;
	skip_white_spaces(l);
	if(l->ch == '~')
		tok = new_token(tilde, l->ch, tok);
	else if(l->ch == ';')
		tok = new_token(semicolon, l->ch, tok);
	else if(l->ch == '<')
		tok = new_token(left, l->ch, tok);
	else if(l->ch == '>')
		tok = handle_right_redir(l, tok);
	else if(l->ch == '|')
		tok = new_token(_pipe, l->ch, tok);
	else if(l->ch == '\'')
		handle_single_quote_identifier(l, tok);
	else if(l->ch == '\"')
		handle_double_quotes_identifier(l, tok);
	else if(l->ch == '\\')
	{
		tok->literal = read_identifier(l, 0);
		return tok;
	}
	else if(l->ch == '$')
	{
		tok->type = env_var;
		tok->literal = read_identifier(l, 0);
		return tok;
	}
	else if(l->ch == 0)
	{
		tok->literal = "newline";
		tok->type = end_of;
		return tok;
	}
	else if(is_letter(l->ch))
		return handle_identifier_with_no_quotes(l, tok);
	else
	{
		tok = new_token(illegal, l->ch, tok);
		errno = EPERM;
		return tok;
	}
	next_char(l);
	return tok;
}

t_token *handle_identifier_with_no_quotes(t_lexer *l, t_token *tok) {
	tok->literal = read_identifier(l, 0);
	if(l->ch == '"' || l->ch == '\'')
	{
		char *tmp = parse_quoted(l, l->ch, 0, 0);
		if(tmp == NULL)
		{
			tok->type = illegal;
			return tok;
		}
		tok->literal = strcat(tok->literal, tmp);
	}
	tok->type = lookup_type(tok->literal);
	return tok;
}

void handle_double_quotes_identifier(t_lexer *l, t_token *tok) {// tok = new_token(DOUBLE_QUOTE, l->ch, tok);
	tok->literal = parse_quoted(l, l->ch, 0, 0);
	if(!tok->literal)
		tok->type = illegal;
	else
		tok->type = arg;
}

void handle_single_quote_identifier(t_lexer *l, t_token *tok) {// tok = new_token(single_quote, l->ch, tok);
	tok->literal = parse_quoted(l, l->ch, 0, 0);
	if(!tok->literal)
		tok->type = illegal;
	else
		tok->type = arg;
}

t_token *handle_right_redir(t_lexer *l, t_token *tok) {
	if(peek_char(l) == '>')
	{
		next_char(l);
		tok = new_token(right_append, 'r', tok);
	}
	else
		tok = new_token(right, l->ch, tok);
	return tok;
}

/*
t_token *next_token(t_lexer *l)
{
	t_token	*tok;

	tok = malloc(sizeof(t_token));
	tok->lexer = l;
	skip_white_spaces(l);
	switch(l->ch)
	{
		case '~':
			tok = new_token(tilde, l->ch, tok);
			break;
		case ';':
			tok = new_token(semicolon, l->ch, tok);
			break;
		case '<':
			tok = new_token(left, l->ch, tok);
			break;
		case '>':
			if(peek_char(l) == '>') {
				next_char(l);
				tok = new_token(right_append, 'r', tok);
				break;
			} else {
				tok = new_token(right, l->ch, tok);
				break;
			}
		case '|':
			tok = new_token(_pipe, l->ch, tok);
			break;
		case '\'':
			// tok = new_token(single_quote, l->ch, tok);
			tok->literal = parse_quoted(l, l->ch, 0, 0);
			if(!tok->literal)
				tok->type = illegal;
			else
				tok->type = arg;
			break;
		case '\"':
			// tok = new_token(DOUBLE_QUOTE, l->ch, tok);
			tok->literal = parse_quoted(l, l->ch, 0, 0);
			if(!tok->literal)
				tok->type = illegal;
			else
				tok->type = arg;
			break;
		case '\\':
			tok->literal = read_identifier(l, 0);
			return tok;
		case '$':
			tok->type = env_var;
			tok->literal = read_identifier(l, 0);
			return tok;
		case 0:
			tok->literal = "newline";
			tok->type = end_of;
			return tok;
		default:
			if(is_letter(l->ch)) {
				tok->literal = read_identifier(l, 0);
				if(l->ch == '"' || l->ch == '\'') {
					char *tmp = parse_quoted(l, l->ch,0 , 0);
					if(tmp == NULL) {
						tok->type = illegal;
						return tok;
					}
					tok->literal = strcat(tok->literal, tmp);
				}
				tok->type = lookup_type(tok->literal);
				return tok;
			} else {
				tok = new_token(illegal, l->ch, tok);
				errno = EPERM;
				return tok;
			}
	}
	next_char(l);
	return tok;
}
*/
void read_and_parse_double_quoted(t_lexer *l, char *s, int *index) {
	char const to_be_escaped[] = {'$', '"', '\\'};
	char *env_name;
	next_char(l);
	while(l->ch != '"') {
		if(l->ch == '\\' && ft_strchr(to_be_escaped, peek_char(l))) {
			handle_escape_in_double_quotes(l, s, index);
			continue;
		}
		if(l->ch == '$' && ft_isalnum(l->input[l->read_position])) {
			next_char(l);
			if(ft_isdigit(l->ch)) {
				next_char(l);
				continue;
			}
			env_name = get_env_value(l);
			if(env_name == NULL)
				continue;
			s = strcat(s, env_name);
			(*index) += (int) ft_strlen(env_name);
			continue;
		}
		s[(*index)++] = l->ch;
		next_char(l);
	}
	next_char(l);
}

void handle_escape_in_double_quotes(t_lexer *l, char *s, int *index) {
	next_char(l);
	s[*index] = l->ch;
	(*index) += 1;
	next_char(l);
}

