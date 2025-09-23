/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilazar <ilazar@student.42.de>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 14:49:15 by ilazar            #+#    #+#             */
/*   Updated: 2025/09/17 18:28:22 by ilazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ListeningSocket.hpp"
#include <iostream>

Socket::Socket(int domain, int type, int protocol, int port, u_long ip_addrs) {
    address.sin_family = domain;
    address.sin_addr.s_addr = htonl(ip_addrs);
    address.sin_port = htons(port);
    sock = socket(domain, type, protocol);
    if (sock < 0)
        throw std::runtime_error(std::string("socket() failed: ") + strerror(errno));
}


void    Socket::bind_socket(void) {
    if (bind(sock, (struct sockaddr*)&address, sizeof(address)) < 0)
        throw std::runtime_error(std::string("bind() failed: ") + strerror(errno));
}

void    Socket::connect_socket(void) {
    if (connect(sock, (struct sockaddr*)&address, sizeof(address)) < 0)
        throw std::runtime_error(std::string("connect() failed: ") + strerror(errno));
}

void    Socket::listening(int backlog, int to_bind) {
    if (to_bind)
        this->bind_socket();
    else
        this->connect_socket();
    if (listen(sock, backlog) < 0)
        throw std::runtime_error(std::string("listen() failed: ") + strerror(errno));
}