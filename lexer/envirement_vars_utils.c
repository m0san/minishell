#include "../include/lexer.h"
#include "../include/minishell.h"

extern t_minishell	g_config;

size_t	get_env_var_last_index(t_lexer *l)
{
	int	counter;

	counter = l->position;
	while (ft_isalnum(l->ch) || ft_isspecial(l->ch))
		next_char(l);
	return (counter);
}

char	*get_env_value(t_lexer *l)
{
	char	*env_value;
	char	*env_name;
	size_t	start;

	if (l->ch == '?')
	{
		env_name = "?";
		next_char(l);
		return (get_var(g_config.envp, env_name));
	}
	env_name = ft_malloc(1024);
	ft_bzero(env_name, 1024);
	start = get_env_var_last_index(l);
	slice_str(l->input, env_name, start, l->position);
	env_value = get_var(g_config.envp, env_name);
	free(env_name);
	if (env_value == NULL)
		return (NULL);
	return (env_value);
}

int	get_env_var_end(t_string str)
{
	int	counter;

	counter = 0;
	while (ft_isalnum(str[counter]))
		counter++;
	return (counter);
}

char	*handle_env_variables(t_string str, int i, int k)
{
	char	*tmp;
	char	*env_value;
	char	env_name[100];

	tmp = ft_malloc(1204 * sizeof(char));
	ft_memset(tmp, 0, 256);
	while (str[++i] != 0 && i < (int) ft_strlen(str))
	{
		if (str[i] == '$' && (ft_isalnum(str[i + 1]) || str[i + 1] == '?'))
		{
			parse_and_get_env_value(str, env_name, &i, &env_value);
			if (env_value == NULL)
			{
				i += (int) ft_strlen(env_name);
				ft_memset(env_name, 0, 100);
				continue ;
			}
			tmp = strcat(tmp, env_value);
			i += (int) ft_strlen(env_name);
			k += (int) ft_strlen(env_value);
			continue ;
		}
		tmp[k++] = str[i];
	}
	return (tmp);
}
