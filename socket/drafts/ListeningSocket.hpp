/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilazar <ilazar@student.42.de>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 18:25:47 by ilazar            #+#    #+#             */
/*   Updated: 2025/09/17 18:38:49 by ilazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <cstring>
#include <stdexcept>
#include <unistd.h>


class ListeningSocket {
private:
    int _fd;

public:
    ListeningSocket(u_long ip_addr, int port, int backlog);
    ~ListeningSocket();

    int getFd() const;


};




    /*
    class Socket {
    private:
        struct sockaddr_in address;
        int sock;
        void    bind_socket();
        void    connect_socket();

    public:
        Socket(int domain, int type, int protocol, int port, unsigned long ip_addrs);
        
    
        void    listening(int backlog, int to_bind);
    };*/