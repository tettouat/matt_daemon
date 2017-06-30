/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tettouat <tettouat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/03 15:04:24 by tettouat          #+#    #+#             */
/*   Updated: 2017/02/03 16:40:50 by tettouat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <sys/time.h>
# include <sys/types.h>
# include <unistd.h>

# include <sys/socket.h>
# include <stdlib.h>
# include <netinet/in.h>

# include <iostream>
#include <stdlib.h>

# include <sys/resource.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <cstdio>
# include <string>
# include <iostream>
# include <sstream>

# include <signal.h>
# include <string.h>

# include <Tintin_reporter.hpp>

# define BUF_SIZE		1024

# define MAX_USER		3

# define FD_FREE		0
# define FD_SERV		1
# define FD_CLIENT		2

struct							s_fds
{
	int							type;
	void						(*fct_read)(int);
	void						(*fct_write)(int);
	char						buf_read[BUF_SIZE + 1];
	char						buf_write[BUF_SIZE + 1];
};

class							Socket
{
	public:
		Socket();
		Socket(int port);
		Socket(Socket const &src);
		Socket &operator=(Socket const &src);
		~Socket();

		void					initfd(void);
		void					do_select(void);
		void					check_fd(void);
		void					loop(void);
	
		static void				cleanclient(struct s_fds *fds);
		static void				clientread(int cs);
		static void				clientwrite(int cs);
		static void				acceptclient(int i);
		void					createsocket(void);
	private:
		int						_maxsd;
		int						_dmax;
		int						_r;
		int						_port;
		fd_set					_writefd;
		fd_set					_readfd;

		static Tintin_reporter	_log;
		static int				_sd;
		static struct s_fds 	*_fds;
		static int				_client;
};

#endif
