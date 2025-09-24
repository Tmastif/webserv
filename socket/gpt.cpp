#include "ServerManager.hpp"
#include <iostream>
#include <cerrno>
#include <cstring>
#include <unistd.h>   // close
#include <sys/socket.h> // recv

// ----------------------
// Constructor
// ----------------------
ServerManager::ServerManager(const std::vector<Server*>& servers)
    : _servers(servers)
{
    // Nothing else to do here
    // Epoll instance (_ep) is constructed automatically
}

// ----------------------
// Destructor
// ----------------------
ServerManager::~ServerManager() {
    // Close all client sockets
    for (std::map<int, std::string>::iterator it = _clientBuffers.begin();
         it != _clientBuffers.end(); ++it)
    {
        close(it->first);
    }
}

// ----------------------
// Check if fd is a listening socket
// ----------------------
bool ServerManager::isListenFd(int fd) {
    for (size_t i = 0; i < _servers.size(); ++i) {
        if (_servers[i]->getListenFd() == fd)
            return true;
    }
    return false;
}

// ----------------------
// Get index of server by listening fd
// ----------------------
int ServerManager::getServerIndex(int listenFd) {
    for (size_t i = 0; i < _servers.size(); ++i) {
        if (_servers[i]->getListenFd() == listenFd)
            return i;
    }
    return -1; // should not happen
}

// ----------------------
// Accept new clients and add to epoll
// ----------------------
void ServerManager::handleNewConnection(int listenFd) {
    int clientFd;
    // Accept in a loop: handle all pending connections
    while ((clientFd = _servers[getServerIndex(listenFd)]->acceptConnection()) >= 0) {
        // Make client non-blocking (required for ET mode)
        Epoll::setNonBlocking(clientFd);

        // Add client socket to epoll with edge-triggered read
        _ep.addFd(clientFd, EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLERR);

        // Initialize buffer for this client
        _clientBuffers[clientFd] = "";
    }
    // If accept returned -1, check for EAGAIN/EWOULDBLOCK: not an error
    if (errno != EAGAIN && errno != EWOULDBLOCK) {
        std::cerr << "accept() error: " << strerror(errno) << std::endl;
    }
}

// ----------------------
// Read data from client until EAGAIN
// ----------------------
void ServerManager::handleClientRead(int clientFd) {
    char buffer[4096];

    while (true) {
        ssize_t n = recv(clientFd, buffer, sizeof(buffer), 0);

        if (n > 0) {
            // Append received data to client's buffer
            _clientBuffers[clientFd].append(buffer, n);

            // For testing, just print what we received
            std::cout << "Received from fd " << clientFd << ": "
                      << std::string(buffer, n) << std::endl;

        } else if (n == 0) {
            // Client closed connection
            closeClient(clientFd);
            break;

        } else { // n < 0
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // No more data right now
                break;
            } else {
                // Some other error occurred
                closeClient(clientFd);
                break;
            }
        }
    }
}

// ----------------------
// Close client socket and clean up
// ----------------------
void ServerManager::closeClient(int clientFd) {
    _ep.delFd(clientFd);               // Remove from epoll
    close(clientFd);                   // Close fd
    _clientBuffers.erase(clientFd);    // Remove buffer
    std::cout << "Closed client fd " << clientFd << std::endl;
}

// ----------------------
// Main event loop
// ----------------------
void ServerManager::run() {
    // 1️⃣ Register all listening sockets in epoll
    for (size_t i = 0; i < _servers.size(); ++i) {
        int listenFd = _servers[i]->getListenFd();
        Epoll::setNonBlocking(listenFd);         // Make non-blocking
        _ep.addFd(listenFd, EPOLLIN | EPOLLET);  // Add ET listening
    }

    struct epoll_event events[64];

    while (true) {
        int numEvents = _ep.wait(events, 64, -1); // Block until events

        for (int i = 0; i < numEvents; ++i) {
            int fd = events[i].data.fd;
            uint32_t ev = events[i].events;

            // 1️⃣ New client connections
            if (isListenFd(fd)) {
                handleNewConnection(fd);
            }
            // 2️⃣ Errors or disconnects
            else if (ev & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
                closeClient(fd);
            }
            // 3️⃣ Readable client
            else if (ev & EPOLLIN) {
                handleClientRead(fd);
            }
        }
    }
}
