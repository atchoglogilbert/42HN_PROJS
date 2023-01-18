/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_execute.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: katchogl <katchogl@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/03 14:23:25 by katchogl          #+#    #+#             */
/*   Updated: 2023/01/17 18:34:36 by katchogl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	ft_upperlvl(t_data *data, int lvl, int *i)
{
	if (data->cmds[*i].instr == I_START
		|| (data->cmds[*i].instr == I_OR && data->status != 0)
		|| (data->cmds[*i].instr == I_AND && data->status == 0))
		ft_loop (data, data->cmds[*i].lvl, i);
	else
	{
		while (data->cmds[*i].lvl > lvl && *i < data->cmdsc)
			(*i)++;
	}
}

static void	ft_run(t_data *data, int *i)
{
	int	j;
	int	temp_i;

	j = -1;
	temp_i = *i;
	while (++j < data->cmdsc_pps)
		ft_child (data, (*i)++, j);
	if (data->pipes != NULL)
	{
		j = -1;
		while (++j < (data->cmdsc_pps - 1) * 2)
			close (data->pipes[j]);
	}
	ft_close_curr_lvl (data, temp_i);
}

static void	ft_currentlvl(t_data *data, int *i)
{
	if (data->cmds[*i].instr == I_START
		|| (data->cmds[*i].instr == I_OR && data->status != 0)
		|| (data->cmds[*i].instr == I_AND && data->status == 0))
	{
		data->cmdsc_pps = ft_anticipate_cmdsc (data, *i);
		data->pipes = ft_initpipes (data, data->cmdsc_pps);
		data->tty_attr.c_lflag |= ECHOCTL;
    	if (tcsetattr(STDIN_FILENO, TCSADRAIN, &data->tty_attr) != 0)
			ft_throw (data, ERR_FAIL, "ft_currentlvl settattr fail", true);
		ft_run (data, i);
	}
	else
		(*i)++;
}

bool	ft_loop(t_data *data, int lvl, int *i)
{
	ft_assert_not_null (data, data);
	while (data->cmds[*i].lvl >= lvl && *i < data->cmdsc)
	{
		if (data->cmds[*i].lvl > lvl)
			ft_upperlvl (data, lvl, i);
		else
			ft_currentlvl (data, i);
	}
	return (true);
}

void	ft_execute(t_data *data)
{
	int	i;

	ft_assert_not_null (data, data);
	if (data->cmds == NULL)
		ft_throw (data, ERR_NULL_PTR, "ft_execute data->cmds", true);
	i = 0;
	ft_loop (data, 0, &i);
}
