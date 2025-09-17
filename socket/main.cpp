/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilazar <ilazar@student.42.de>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 16:32:25 by ilazar            #+#    #+#             */
/*   Updated: 2025/09/17 18:37:29 by ilazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ListeningSocket.hpp"
#include <iostream>

#define PORT 8080

int main(void) {

    
    try {
        ListeningSocket sock(INADDR_ANY, PORT, 10);
    } catch (const std::exception &e) {
        std::cerr << "Socket error: " << e.what() << std::endl;
        // don't crash — just skip this socket or exit cleanly
    }
    
    
    
    
    return 0;
};