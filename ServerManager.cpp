/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilazar <ilazar@student.42.de>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 21:26:20 by ilazar            #+#    #+#             */
/*   Updated: 2025/09/23 22:51:46 by ilazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include <sys/epoll.h>
#include <iostream>
#include <cerrno>
#include <cstring>
// #include <unistd.h>   // close
// #include <sys/socket.h> // recv



//Constructor Destructor
ServerManager::ServerManager(const std::vector<Server*> &servers)
    : _servers(servers), _ep(MAX_EVENTS) {}

ServerManager::~ServerManager() {
    for (std::map<int, std::string>::iterator it = _clientBuffers.begin();
        it != _clientBuffers.end(); ++it)
    {
        close(it->first);
    }
}


//Helper functions
bool    ServerManager::isListenFd(int fd) {
    for (size_t i = 0; i < _servers.size(); ++i) {
        if (_servers[i]->getSocketFd() == fd)
            return true; //a listening socket
    }
    return false; //a client
}

//get the index of the server
int     ServerManager::getServerIndex(int listenFd) {
    for (size_t i = 0; i < _servers.size(); ++i) {
        if (_servers[i]->getSocketFd() == listenFd)
            return i;
    }
    return -1;
}

//Accept new clients and add to epoll
void    ServerManager::newConnection(int listenFd) {
    int clientFd;
    
    while ((clientFd = _servers[getServerIndex(listenFd)]->acceptConnection()) >= 0) {
        Epoll::setNonBlocking(clientFd);
        _ep.addFd(clientFd, EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLERR);
        _clientBuffers[clientFd] = "";
    }
    if (errno != EAGAIN && errno != EWOULDBLOCK) //otherwise means normal no more pending connections
        std::cerr << "accept() error: " << strerror(errno) << std::endl;
}