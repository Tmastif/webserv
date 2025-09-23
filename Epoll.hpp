/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Epoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilazar <ilazar@student.42.de>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 19:16:18 by ilazar            #+#    #+#             */
/*   Updated: 2025/09/23 21:23:29 by ilazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

class Epoll {
    
    private:
        int _epfd;
        Epoll(const Epoll &);
        Epoll& operator=(const Epoll &);

    public:
        Epoll(int maxEvents);
        ~Epoll();

        //Add an Fd to the epoll interests list
        //@events is EPOLLIN, EPOLLOUT, EPOLLERR 
        void    addFd(int fd, unsigned int events);
        
        //Modify interest events for an existing Fd
        void    modFd(int fd, unsigned int events);

        //remove an Fd from epoll interest list
        void    delFd(int fd);

        //Wait for events
        int     wait(struct epoll_event *events, int maxEvents, int timeout);

        //Get epoll fd
        int     getFd() const;

        //Make a socket non-blocking
        static void     setNonBlocking(int fd);
};