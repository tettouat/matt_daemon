/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tettouat <tettouat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/03 16:27:05 by tettouat          #+#    #+#             */
/*   Updated: 2017/02/03 16:40:57 by tettouat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Socket.hpp>

int					Socket::_sd;
struct s_fds		*Socket::_fds;
int					Socket::_client;
Tintin_reporter		Socket::_log;
extern 				char **environ;

Socket::Socket(void)
{
	_fds = NULL;
	_sd = 0;
	_port = 4242;
	_client = 0;
	createsocket();
	return ;
}

Socket::Socket(int port) : _port(port)
{
	_fds = NULL;
	_sd = 0;
	_client = 0;
	createsocket();
	return ;
}

Socket::Socket(Socket const &src)
{
	if (this != &src)
	{

	}
	return ;
}

Socket::~Socket(void)
{
	free(_fds);
	close(_sd);
	return ;
}

Socket			&Socket::operator=(Socket const &src)
{
	if (this != &src)
	{

	}
	return (*this);
}

void				Socket::initfd(void)
{
	int			i;

	i = 0;
	_dmax = 0;
	FD_ZERO(&_writefd);
	FD_ZERO(&_readfd);
	while (i < _maxsd)
	{
		if (_fds[i].type != FD_FREE)
		{
			FD_SET(i, &_readfd);
			if (strlen(_fds[i].buf_write) > 0)
				FD_SET(i, &_writefd);
			_dmax = (i > _dmax) ? i : _dmax;
		}
		++i;
	}
}

void				Socket::do_select(void)
{
	_r = select(_dmax + 1, &_readfd, &_writefd, NULL, NULL);
}

void				Socket::check_fd(void)
{
	int		i = 0;

	while ((i < _maxsd) && (_r > 0))
	{
		if (FD_ISSET(i, &_readfd))
			_fds[i].fct_read(i);
		if (FD_ISSET(i, &_writefd))
			_fds[i].fct_write(i);
		if (FD_ISSET(i, &_readfd) || \
				FD_ISSET(i, &_writefd))
			_r--;
		i++;
	}
}

void 				ft_itoa(int value, std::string& buf, int base)
{
	int i = 30;

	buf = "";
	for (; value && i ; --i, value /= base) buf = "0123456789abcdef"[value % base] + buf;

}

void				Socket::loop(void)
{
	string		str;
	string		ptr;
	pid_t		pid = getpid();

	ft_itoa(pid, ptr, 10);
	_log.writelog("INFO", "Entering Daemon mode.");
	freopen( "/dev/null", "r", stdin);
	freopen( "/dev/null", "w", stdout);
	freopen( "/dev/null", "w", stderr);
	stringstream ss;
	ss << "Server Started. PID: " << ptr << " Port: " << _port;
	_log.writelog("INFO", ss.str());
	while (1)
	{
		initfd();
		do_select();
		check_fd();
	}
}

void				Socket::cleanclient(struct s_fds *fds)
{
	fds->type = FD_FREE;
	fds->fct_read = NULL;
	fds->fct_write = NULL;
}

void				Socket::clientread(int cs)
{
	int					r;

	r = recv(cs, _fds[cs].buf_read, BUF_SIZE, 0);
	if (r <= 0)
	{
		Socket::cleanclient(&_fds[cs]);
		close(cs);
		_client--;
		stringstream	ss;
		ss << "Client disconnected! - remaining client: " << _client << "/" << MAX_USER;
		_log.writelog("LOG", ss.str());
	}
	else
	{
		if (strcmp(_fds[cs].buf_read, "quit\n") == 0)
		{
			_log.writelog("INFO", "Request quit.");
			exit(-1);
		}
		_log.writelog("LOG", _fds[cs].buf_read);
		if (strcmp(_fds[cs].buf_read, "/clear\n") == 0)
		{
			_log.writelog("INFO", "Request log clearing");
			if(_log.clearlog() != 1)
				_log.writelog("ERROR", "Error while clearing the log file!");
		}
		bzero(_fds[cs].buf_read, 1024);
	}
}

void				Socket::clientwrite(int cs)
{
	(void)cs;
}

void				Socket::acceptclient(int i)
{
	int					cs;
	struct sockaddr_in	csin;
	socklen_t			csin_len;

	(void)i;
	csin_len = sizeof(csin);
	if ((cs = accept(_sd, (struct sockaddr *)&csin, &csin_len)) == -1)
	{
		_log.writelog("ERROR", "Error accept fail.");
		exit(-1);
	}
	Socket::cleanclient(&_fds[cs]);
	if (_client >= MAX_USER)
	{
		close(cs);
		stringstream	ss;
		ss << "Connections limit exceeded " << _client << "/" << MAX_USER;
		_log.writelog("ERROR", ss.str());
		return ;
	}
	if (_client < MAX_USER)
	{
		stringstream	ss;
		ss << "Client " << _client + 1 << "/" << MAX_USER << " connected!";
		_log.writelog("LOG", ss.str());
	}
	_fds[cs].type = FD_CLIENT;
	_fds[cs].fct_read = &Socket::clientread;
	_fds[cs].fct_write = &Socket::clientwrite;
	_client++;
}

void				Socket::createsocket(void)
{
	struct rlimit		rlp;
	struct sockaddr_in	sin;

	_log.writelog("INFO", "Creating server.");
	if (getrlimit(RLIMIT_NOFILE, &rlp) == -1)
	{
		_log.writelog("ERROR", "Error getrlimit fail.");
		exit(-1);
	}
	_maxsd = rlp.rlim_cur;
	_fds = (struct s_fds *)malloc(sizeof(struct s_fds) * _maxsd);
	if ((_sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		_log.writelog("ERROR", "Error socket fail.");
		exit(-1);
	}
	sin.sin_port = htons(_port);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_family = AF_INET;
	if (bind(_sd, (const struct sockaddr *)&sin, sizeof(sin)) == -1)
	{
		_log.writelog("ERROR", "Error bind fail.");
		exit(-1);
	}
	if (listen(_sd, 42) == -1)
	{
		_log.writelog("ERROR", "Error listen fail.");
		exit(-1);
	}
	_fds[_sd].type = FD_SERV;
	_fds[_sd].fct_read = &Socket::acceptclient;
	_log.writelog("INFO", "Server created.");
}
