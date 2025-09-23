/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilazar <ilazar@student.42.de>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 14:59:04 by ilazar            #+#    #+#             */
/*   Updated: 2025/09/23 21:47:17 by ilazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <stdexcept>
#include <errno.h>
#include <cstring>
#include <netdb.h>
#include <iostream>


// Constructor Destructor
Server::Server(const std::string &host, int port) :
    _socketFd(-1), _host(host), _port(port) {}


Server::~Server() {
    closeSocket();
}


//Functions
void    Server::init(void) {
    struct  addrinfo    hints;
    struct  addrinfo    *res;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       //IPv4
    hints.ai_socktype = SOCK_STREAM; //TCP
    hints.ai_flags = AI_PASSIVE;     //binds "0.0.0.0" if node == null
    
    std::string portStr = std::to_string(_port);

    int ret = getaddrinfo(_host.c_str(), portStr.c_str(), &hints, &res);
    if (ret != 0) {
        throw std::runtime_error(std::string("getaddrinfo() failed: ") + std::string(gai_strerror(ret)));
    }
    
    _socketFd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (_socketFd < 0) {
        freeaddrinfo(res);
        throw std::runtime_error(std::string("socket() failed: ") + strerror(errno));
    }
    
    // SO_REUSEADDR no TIME_WAIT state, allows socket to bind again immediatly
    int opt = 1;
    setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(_socketFd, res->ai_addr, res->ai_addrlen) < 0) {
        freeaddrinfo(res);
        throw std::runtime_error(std::string("bind() failed: ") + strerror(errno));
    }

    freeaddrinfo(res);
    
    if (listen(_socketFd, SOMAXCONN) < 0) { // Max allowed connections 
        throw std::runtime_error(std::string("listen() failed: ") + strerror(errno));
    }
}


int     Server::acceptConnection(void) {
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    int clientFd = -1;

    while (1) {
        clientFd = accept(_socketFd, (struct sockaddr*)&clientAddr, &addrLen);
        if (clientFd >= 0) {
            std::cout << "Accepted connection from: "
                    << ntohs(clientAddr.sin_port)
                    << " (fd=" << clientFd << ")" << std::endl;
            return clientFd;
        } else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return -1; //no more pending connections
            } else if (errno == EINTR) {
                continue; //interepted by signal, try again
            } else {
                throw std::runtime_error(std::string("accept() failed: ") + strerror(errno));
            }
        }
    }
}


void    Server::closeSocket(void) {
    if (_socketFd != -1)
        close(_socketFd);
    _socketFd = -1;
}


//Getters
int             Server::getSocketFd(void) const {
    return (_socketFd);
}

int             Server::getPort(void) const {
    return (_port);
}

std::string     Server::getHost(void) const {
    return (_host);
}


//Test
void Server::run(void) {
    while (1) {
        int clientFd = acceptConnection();
        
        char buffer[4096];
        ssize_t bytesRead = recv(clientFd, &buffer, sizeof(buffer) - 1, 0);
        

        //THIS IS WHERE WE RECEIVE A REQUEST
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::cout << "== client request ==" << std::endl;
            std::cout << buffer <<std::endl;
            

            //Demo respond
            const char* response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Length: 13\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "Hello, world!";
            
            send(clientFd, response, std::strlen(response), 0);
        }
        close(clientFd);
    }    
}