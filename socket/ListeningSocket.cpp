/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilazar <ilazar@student.42.de>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 18:25:25 by ilazar            #+#    #+#             */
/*   Updated: 2025/09/17 18:50:54 by ilazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "ListeningSocket.hpp"
#include <iostream>

//Default Constructor
ListeningSocket::ListeningSocket(u_long ip_addr, int port, int backlog) {
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd < 0)
        throw std::runtime_error(std::string("socket() failed: ") + strerror(errno));

    int opt = 1;
    setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(ip_addr);
    addr.sin_port = htons(port);

    if (bind(_fd, (sockaddr*)&addr, sizeof(addr)) < 0)
        throw std::runtime_error(std::string("bind() failed: ") + strerror(errno));

    if (listen(_fd, backlog) < 0)
        throw std::runtime_error(std::string("listen failed: ") + strerror(errno));
}

//Deconstructor
ListeningSocket::~ListeningSocket() {
    if (_fd >= 0)
        close(_fd);
}


//Getters
int ListeningSocket::getFd() const {
    return _fd;
}


/*
Setsocket SO_REUSEADDR functionality:
When you close a TCP socket, it often stays in the TIME_WAIT state for a short time (usually 1–4 minutes) during which the adress is in use..
- Restart your server without waiting
- Bind multiple sockets to the same port if they use different local IPs
*/