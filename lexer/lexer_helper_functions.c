#include "lexer.h"

t_lexer	*new_lexer(const char *line, int cmd_length)
{
	t_lexer	*lexer;

	lexer = malloc(sizeof(t_lexer));
	*lexer = (t_lexer) {.input = (char *) line, .len = cmd_length, .read_position = 0, .position = 0, .ch = ' ', .is_error = false};
	next_char(lexer);
	return (lexer);
}

void	skip_white_spaces(t_lexer *l)
{
	if (!l || !l->input)
	{
		errno = ENODATA;
		return ;
	}
	while (l->ch == ' ' || l->ch == '\t' || l->ch == '\n')
		next_char(l);
}

char	peek_char(t_lexer *l)
{
	if (l->read_position >= l->len)
		return (0);
	else
		return (l->input[l->read_position]);
}

void	next_char(t_lexer *lexer)
{
	if (!lexer || !lexer->input)
		errno = ENODATA;
	if (lexer->read_position >= lexer->len)
		lexer->ch = 0;
	else
		lexer->ch = lexer->input[lexer->read_position];
	lexer->position = lexer->read_position;
	++lexer->read_position;
}

enum e_bool	has_char(char c)
{
	const char	*buff = "+:?=._-/\\$";
	for (int i = 0; buff[i] != 0; ++i)
		if (buff[i] == c)
			return (true);
	return (false);
}
