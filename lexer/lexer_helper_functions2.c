#include "lexer.h"

enum e_bool	is_letter(char ch)
{
	return (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || \
	('0' <= ch && ch <= '9') || has_char(ch));
}

int *has_next_quote(t_lexer *l, char quote)
{
	int	i;
	int	las_quote_index;
	int	n_of_quotes;
	int	*res;

	las_quote_index = 0;
	n_of_quotes = 0;
	res = malloc(2 * sizeof(int));
	i = l->position;
	while (i < l->len)
	{
		if(l->input[i] == quote)
		{
			n_of_quotes++;
			las_quote_index = i;
		}
		i++;
	}
	if (n_of_quotes == 0 && las_quote_index == 0)
		return (NULL);
	res[0] = n_of_quotes;
	res[1] = (las_quote_index);
	return (res);
}
