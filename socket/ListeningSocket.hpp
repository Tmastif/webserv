/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilazar <ilazar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 18:25:47 by ilazar            #+#    #+#             */
/*   Updated: 2025/09/19 17:32:13 by ilazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <cstring>
#include <stdexcept>
#include <unistd.h>


/*
A class to create a socket, bind it and have it listening
a TCP socket Ipv4

Class dosn't allow copy and assigment 
*/

class ListeningSocket {
private:
    int _fd;
    ListeningSocket(const ListeningSocket &oth);
    ListeningSocket& operator=(const ListeningSocket &oth);

public:
    ListeningSocket();
    ListeningSocket(u_long ip_addr, int port, int backlog);
    ~ListeningSocket();

    int getFd() const;
};