/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilazar <ilazar@student.42.de>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 14:48:49 by ilazar            #+#    #+#             */
/*   Updated: 2025/09/23 18:34:04 by ilazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>


class Server {

    private:
        int                 _socketFd;  // listening socket
        std::string         _host;      // "0.0.0.0"
        int                 _port;      // "8080"
        
        // Prevent copies
        Server(const Server &);
        Server& operator=(const Server &);

    public:
        Server(const std::string &host, int port);
        ~Server();

        //setup
        void    init(); //create socket, bind, listen
        void    closeSocket();

        int     acceptConnection(); //returns new client fd
               
        // Getters
        int          getSocketFd() const;
        int          getPort() const;
        std::string  getHost() const; 

        //Test
        void        run();
};