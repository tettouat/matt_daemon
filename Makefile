# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tettouat <tettouat@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2017/02/03 15:01:09 by tettouat          #+#    #+#              #
#    Updated: 2017/02/03 16:41:07 by tettouat         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = Matt_daemon

INCDIR = includes

FLAG = -Wall -Wextra -Werror

SRCDIR = src/

OBJ = 	$(SRC:.c=.o)

SRC =   $(SRCDIR)main.cpp \
	    $(SRCDIR)Socket.cpp \
	    $(SRCDIR)Tintin_reporter.cpp

all: $(NAME)

$(NAME): $(OBJ)
	@g++ -o $(NAME) $(OBJ) -I $(INCDIR) $(FLAG)

%.o: %.c
	@g++ -o $@ -c $< $(FLAG)

clean:
	@rm -f $(OBJ)

fclean:
	@rm -f $(NAME)

re: fclean all 

.PHONY: re fclean clean all
