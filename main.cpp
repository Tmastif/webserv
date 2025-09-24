/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilazar <ilazar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 18:26:11 by ilazar            #+#    #+#             */
/*   Updated: 2025/09/24 13:10:00 by ilazar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerManager.hpp"
#include <iostream>

int main() {

    try {
        Server test("127.0.0.1", 8080);
        test.init();
        
        std::vector<Server*> servers;
        servers.push_back(&test);
        
        ServerManager manager(servers);
        manager.run();
        
    }
    catch (const std::exception &e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}