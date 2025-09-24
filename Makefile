# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ilazar <ilazar@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/09/24 13:53:19 by ilazar            #+#    #+#              #
#    Updated: 2025/09/24 14:08:59 by ilazar           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98


NAME = webserv

#SRCDIR = src
OBJDIR = obj

SRCS 	= Server.cpp Epoll.cpp ServerManager.cpp main.cpp

OBJS		= $(addprefix $(OBJDIR)/,$(SRCS:.cpp=.o))


all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(OBJS:.o=.d)

$(OBJDIR):
	mkdir -p $(OBJDIR)


clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)


re: fclean all

# Phony targets
.PHONY: all clean fclean re
