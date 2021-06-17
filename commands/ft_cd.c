#include "ft_buildin.h"

int ft_cd(int ac, char **av)
{
	int ret;
	t_var *var;
	char pwd[1025];

	if (ac == 1)
		ret = chdir(get_var(g_envp, "HOME"));
	else if (ac >= 2)
		ret = chdir(av[1]);
	if (ret == -1)
	{
		// char *err = strerror(errno);
		// write(1, err, strlen(err));
		// write(1, "\n", 1);
		perror("Error Message");
		return (ret);
	}
	set_var2(g_envp, "OLDPWD", get_var(g_envp, "PWD"));
	getcwd(pwd, 1024);
	set_var2(g_envp, "PWD", pwd);
	return (ret);
}