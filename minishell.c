#include "minishell.h"

const char *prompt = "minishell-0.1$ ";

void fill_out_env_command(t_cmd *tmp_cmd, const char *tmp);

/*
 * Here the shell runs in a loop,
 * which we know as the Read-Eval-Print-Loop, or REPL.
 */

size_t readline(char *line) {
    ft_memset(line, 0, 1024);
    size_t n = read(0, line, 1024);
    if (n)
        line[n - 1] = 0;
    return n;
}

void str__(t_token *tok) {
    printf("TokenType: [%s], Literal: [%s]\n", getTypeName(tok->type), tok->literal);
}

const char *getTypeName(enum e_val_type type) {
    switch (type) {
        case left:
            return "Left Redirection";
        case right:
            return "Right Redirection";
        case right_append:
            return "Right Append";
        case _pipe:
            return "PIPE";
        case env_var:
            return "Environment Variable";
        case none:
            return "None";
        case VAL_STR:
            return "Value String";
        default:
            return "No such type";
    }
}

char *Print(struct s_cmd *this) {

    if (this == NULL)
        ft_exit("Error\nNull CMD!", 1);


    char *line = malloc(1024);
    bzero(line, 1024);
    for (int i = 0; this->argv[i] != 0; i++) {
        strcat(line, this->argv[i]);
        if (this->argv[i + 1] != 0)
            strcat(line, " ");
    }
    return line;
}

t_cmd *create_cmd() {
    t_cmd *command = malloc(sizeof(t_cmd));
    ft_memset(command, 0, sizeof(t_cmd));
    command->redirs = new_vector();
    return command;
}

t_vector *fill_out_vector_with_commands(t_node *ast_node) {
    t_node *child;
    t_vector *vector = new_vector();
    t_cmd *tmp_cmd = NULL;


    child = ast_node->first_child;
    tmp_cmd = create_cmd();
    while (child) {
        if (child->val_type == right) {
            struct s_redir *tmp = create_redir(right, child->next_sibling->val.str);
            child = child->next_sibling;
            insert(tmp_cmd->redirs, tmp);
        } else if (child->val_type == left) {
            insert(tmp_cmd->redirs, create_redir(left, child->next_sibling->val.str));
            child = child->next_sibling;
        } else if (child->val_type == right_append) {
            insert(tmp_cmd->redirs, create_redir(right_append, child->next_sibling->val.str));
            child = child->next_sibling;
        } else if (child->val.str[0] == '$' && child->val_type == env_var) {
            if (child->val.str != NULL) {
                char *tmp = handle_env_variables(child->val.str, 0, 0);
                if (tmp != NULL)
                    fill_out_env_command(tmp_cmd, tmp);
            } else
                tmp_cmd->argv[tmp_cmd->count++] = child->val.str;
        } else if (child->val_type == _pipe) {
            insert(vector, tmp_cmd);
            tmp_cmd = create_cmd();
        } else
            tmp_cmd->argv[tmp_cmd->count++] = child->val.str;
        child = child->next_sibling;
    }
    insert(vector, tmp_cmd);
    return vector;
}

void fill_out_env_command(t_cmd *tmp_cmd, const char *tmp) {
    char **splited_env_value = ft_split(tmp, ' ');
    int i = -1;
    while (splited_env_value[++i] != 0)
        tmp_cmd->argv[tmp_cmd->count++] = splited_env_value[i];
}

t_error *check_first_token(t_parser *p) {
    t_error *error = malloc(sizeof(t_error));
    ft_memset(error, 0, sizeof(t_error));
    enum e_val_type types[] = {illegal, end_of, semicolon, _pipe, right, left, right_append};
    for (int i = 0; i < 7; ++i) {
        if ((p->cur_token->type == types[i] && p->peek_token->type == end_of) || p->cur_token->type == end_of)
            set_error(error, ERR1);
    }
    return error;
}

void parse_and_execute(t_lexer *lexer) {
    t_cmd *cmd;
    t_node *ast_node = NULL;
    t_parser *p = new_parser(lexer);
    cmd = malloc(sizeof(t_cmd));
    ft_memset(cmd, 0, sizeof(t_cmd));
    cmd->redirs = new_vector();

    t_error *err = check_first_token(p);
    if (err->is_error) {
        printf("%s '%s'\n", err->error_msg, p->peek_token->literal);
        return;
    }

    ast_node = parse_command(ast_node, p);
    if (ast_node == NULL)
        return;
    t_vector *vector = fill_out_vector_with_commands(ast_node);
    run_cmds((t_vector *) vector);
}

t_bool check_semicolon_errors(const char *line)
{
    t_parser *p = new_parser(new_lexer(line, (int) ft_strlen(line)));
    while (p->cur_token->type != end_of) {
        next_token_p(p);
        if (p->cur_token->type == semicolon && p->peek_token->type == semicolon) {
            free(p);
            return true;
        }
    }
    free(p);
    return false;
}

void signal_handler_parent(int sig)
{
	if (sig == SIGQUIT && g_is_forked)
		dprintf(1, "Quit: 3");
	if (!(sig == SIGQUIT && !g_is_forked))
		dprintf(1, "\n");
    if (sig == SIGINT && !g_is_forked)
        dprintf(1, "%s", prompt);
}

void signal_handler(int sig) {
    // if (sig == SIGQUIT)
    // 	dprintf(2, "^\\\n");
    // else if (sig == SIGINT)
    // 	dprintf(2, "^C\n");
}

#if (1)

int	_read(char **line)
{
	char	*buffer;
	int		rd;

	while (true)
	{
		write(1, prompt, ft_strlen(prompt));
        char *buffer = malloc(1024 * sizeof(char));
		rd = readline(buffer);
		if (!rd || buffer[0] == '\0' || strcmp(buffer, "\n") == 0) {
            free(buffer);
            continue;
        }
		line = ft_strjoin(line, buffer);
		t_bool has_error = check_semicolon_errors(line);
        if (has_error) {
            printf("minishell: syntax error near unexpected token `;;'\n");
            free(line);
            return 1;
        }

        char **splited_line = ft_split(line, ';');
        int i = 0;
        while (splited_line[i] != 0) {
            t_lexer *lexer = new_lexer(splited_line[i], (int) ft_strlen(splited_line[i]));
            parse_and_execute(lexer);
            i++;
            free(lexer);
        }

        free(line);
	}
	if (strlen(line) > 0)
		_read(line);
	exit(0);
}

int main(int ac, char **av, char **env) {
	g_is_forked = false;
    fill_envp(env);
    signal(SIGQUIT, signal_handler_parent);
    signal(SIGINT, signal_handler_parent);
    while (true) {
        write(1, prompt, ft_strlen(prompt));
        char *line = malloc(1024 * sizeof(char));
        size_t n = readline(line);
        if (!n || line[0] == '\0' || strcmp(line, "\n") == 0) {
            free(line);
            continue;
        }

        if (strcmp(line, "exit") == 0) {
            dprintf(2, "shell is exiting...\n");
            free(line);
            break;
        }

        t_bool has_error = check_semicolon_errors(line);
        if (has_error) {
            printf("minishell: syntax error near unexpected token `;;'\n");
            free(line);
            return 1;
        }

        char **splited_line = ft_split(line, ';');
        int i = 0;
        while (splited_line[i] != 0) {
            t_lexer *lexer = new_lexer(splited_line[i], (int) ft_strlen(splited_line[i]));
            parse_and_execute(lexer);
            i++;
            free(lexer);
        }

        free(line);
    }
    return (EXIT_SUCCESS);
}

#endif 
