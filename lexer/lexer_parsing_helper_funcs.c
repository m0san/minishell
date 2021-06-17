#include "lexer.h"

t_bool	is_quote(const t_lexer *l)
{
	return (l->ch == '\'' || l->ch == '"');
}

void	slice_str(const char *str, char *buffer, size_t start, size_t end)
{
	size_t	j;
	size_t	i;

	j = 0;
	i = start;
	while(i < end)
	{
		buffer[j++] = str[i];
		i++;
	}
	buffer[j] = 0;
}

char	*read_identifier(t_lexer *l, int i)
{
	char	*s;
	char *env_name;

	s = malloc(1024);
	ft_bzero(s, 1024);
	while(is_letter(l->ch))
	{
		if (l->ch == '\\')
			next_char(l);
		if (l->ch == '$' && ft_isalnum(l->input[l->read_position]))
		{
			next_char(l);
			if (ft_isdigit(l->ch))
			{
				next_char(l);
				continue ;
			}
			env_name = get_env_value(l);
			if (env_name == NULL)
				continue ;
			s = strcat(s, env_name);
			i += (int) ft_strlen(env_name);
			continue ;
		}
		s[i++] = l->ch;
		next_char(l);
	}
	return (s);
}

void	read_single_quoted(t_lexer *l, char quote, char *s, int *index)
{
	next_char(l);
	while (l->ch != quote)
	{
		s[(*index)++] = l->ch;
		next_char(l);
	}
	next_char(l);
}

char *parse_quoted(t_lexer *l, char quote, 	int	i, int s_index)
{
	char	*s;
	int		*infos;
	int		pos;

	init_parse_quoted_vars(l, quote, &s, &infos);
	while (i++ < infos[1] && l->ch != 0)
	{
		if(l->ch == '\\')
		{
			handle_escape(l, s, &i, &s_index);
			continue ;
		}
		if(l->ch == '"')
			handle_double_quotes(l, s, &i, &s_index, &pos);
		else if(l->ch == '\'')
			handle_single_quote(l, s, &i, &s_index, &pos);
		else if(ft_isalnum(l->ch))
			handle_alphanum_identifier(l, pos, &s, &i, &s_index);
		if (is_quote(l))
			infos = has_next_quote(l, l->ch);
		if(l->ch == ' ')
			return (s);
	}
	return (s);
}