/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilazar <ilazar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 21:26:20 by ilazar            #+#    #+#             */
/*   Updated: 2025/09/24 16:10:18 by ilazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Server.hpp"
#include "Epoll.hpp"
#include <vector>
#include <map>
#include <string>

#define MAX_EVENTS 64 //maximum number of events that epoll can return in one call
#define READ_BUFF_SIZE 4096 //4kb - a convetion to handle most small requests in one read.

class ServerManager {
  
    public:
        ServerManager(const std::vector<Server*> &servers);
        ~ServerManager();
        
        void    run();
    
    private:
        Epoll                   _ep;
        std::vector<Server*>    _servers;
        std::map<int, std::string> _clientBuffers; //stores partial read per client

        //Disable copying
        ServerManager(const ServerManager &);
        ServerManager& operator=(const ServerManager &);
        
        //Helper functions
        bool    isListenFd(int fd);             //check if fd is a listening socket
        int     getServerIndex(int listenFd);   
        void    newConnection(int listenFd);    //accepts clients, add to epoll
        bool    clientRead(int clientFd);       //read data until EAGAIN
        // void    clientWrite(int clientFd);
        void    closeClient(int clientFd);      //remove fd from epoll
};