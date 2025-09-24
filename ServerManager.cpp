/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilazar <ilazar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 21:26:20 by ilazar            #+#    #+#             */
/*   Updated: 2025/09/24 16:25:06 by ilazar           ###   ########.fr       */
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
    : _ep(MAX_EVENTS), _servers(servers) {}

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

/* Accept new clients and add to epoll.
EPOLLIN - Wake up on new data.
EPOLLET + non-blocking - Never block.
EPOLLRDHUP - Get notified immediately if a client half-closes.
EPOLLERR - Get notified on socket errors.
*/
void    ServerManager::newConnection(int listenFd) {
    int clientFd;
    
    while ((clientFd = _servers[getServerIndex(listenFd)]->acceptConnection()) >= 0) {
        Epoll::setNonBlocking(clientFd);
        _ep.addFd(clientFd, EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLERR);
        // _ep.addFd(clientFd, EPOLLIN | EPOLLRDHUP | EPOLLERR);
        std::cout << "[fd " << clientFd << "] Client. was added to ep\n";
        _clientBuffers[clientFd] = "";
    }
    if (errno != EAGAIN && errno != EWOULDBLOCK) //otherwise means normal no more pending connections
        std::cerr << "accept() error: " << strerror(errno) << std::endl;
}


bool    ServerManager::clientRead(int clientFd) {
    char buffer[READ_BUFF_SIZE];

    while (1) {
        ssize_t n = recv(clientFd, buffer, sizeof(buffer), 0);
        if (n > 0) { //append recieved data to client's buffer
            _clientBuffers[clientFd].append(buffer, n);
            
            //FOR TESTING:
            std::cout << "[fd " << clientFd << "] Recieved: ------> "
                    << std::string(buffer, n);
        
        } else if (n == 0) {
            std::cout << "[fd " << clientFd << "] disconnected (EOF)" << std::endl;
            return true;
            break;
        } else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                std::cout << "[fd " << clientFd << "] (EAGAIN/EWOULDBLOCK)" << std::endl;
                break; //no more data
            } else {
                //TODO: report/log the error
                return true;
                break;
            }
        }
    }
    return false;
}


void    ServerManager::closeClient(int clientFd) {
    _ep.delFd(clientFd);
    close(clientFd);
    _clientBuffers.erase(clientFd);
    //TODO put into log report?
    std::cout << "[fd " << clientFd  << "] has closed ##" << std::endl;
}


//main loop
void    ServerManager::run(void) {
    //regester all listening sokects on epoll
    for (size_t i = 0; i < _servers.size(); ++i) {
        int listenFd = _servers[i]->getSocketFd();
        Epoll::setNonBlocking(listenFd);
        _ep.addFd(listenFd, EPOLLIN | EPOLLET | EPOLLRDHUP);
        // _ep.addFd(listenFd, EPOLLIN | EPOLLRDHUP);
        std::cout << "[fd " << listenFd << "] Listening. was added to interest list\n";
    }

    struct epoll_event events[MAX_EVENTS];
    

    while (1) {
        int numEvents = _ep.wait(events, MAX_EVENTS, -1); //blocks
        for (int i = 0; i < numEvents; ++i) {
            bool disconnet = false;
            int fd = events[i].data.fd;
            uint32_t ev = events[i].events;
            
            //new client
            if (isListenFd(fd)) {
                newConnection(fd);
                continue;
            }
            if (ev & EPOLLIN) {
                disconnet = clientRead(fd); //read
            }
            if (disconnet || ev & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
                closeClient(fd); //errors / disconnections
            }
        }
    }
}

