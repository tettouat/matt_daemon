/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tettouat <tettouat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/03 15:07:11 by tettouat          #+#    #+#             */
/*   Updated: 2017/02/03 16:40:44 by tettouat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Tintin_reporter.hpp>
#include <Socket.hpp>

void					ft_signal(void)
{
	int 		i;

	i = 0;
	while (i < 32)
		signal( i++, &Tintin_reporter::sighandler );

}

int						main(int ac, char **av)
{
	if (ac == 1)
	{
		Socket		socket = Socket(4242);
		ft_signal();
		socket.loop();
	}
	else if (ac != 3)
		std::cout << "Usage: " << av[0] << " [-p port]" << std::endl;
	else
	{
		for (int i = 0 ; i < ac ; ++i)
		{
			if (strcmp(av[i], "-p") == 0 && av[i + 1] != NULL)
			{
				Socket		socket = Socket(atoi(av[i + 1]));
				ft_signal();
				socket.loop();
				exit(EXIT_SUCCESS);
			}		
		}
		std::cout << "Usage: " << av[0] << " [-p port]" << std::endl;
	}
	return (0);
}
