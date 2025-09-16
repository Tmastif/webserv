#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <vector>

const int MAX_EVENTS = 1024;
const int PORT = 8080;

// Utility: set a socket to non-blocking mode
void setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main() {
    // 1. Create listening socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(listen_fd, (sockaddr*)&addr, sizeof(addr));
    listen(listen_fd, SOMAXCONN);

    setNonBlocking(listen_fd);

    // 2. Create epoll instance
    int epfd = epoll_create1(0);

    // 3. Register listening socket with epoll
    epoll_event ev{};
    ev.events = EPOLLIN;           // ready to accept new connections
    ev.data.fd = listen_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev);

    // 4. Event loop
    std::vector<epoll_event> events(MAX_EVENTS);

    while (true) {
        int n = epoll_wait(epfd, events.data(), MAX_EVENTS, -1);
        for (int i = 0; i < n; i++) {
            int fd = events[i].data.fd;

            // --- New client connection ---
            if (fd == listen_fd) {
                sockaddr_in client_addr;
                socklen_t len = sizeof(client_addr);
                int client_fd = accept(listen_fd, (sockaddr*)&client_addr, &len);
                setNonBlocking(client_fd);

                epoll_event client_ev{};
                client_ev.events = EPOLLIN | EPOLLET;  // Edge-triggered
                client_ev.data.fd = client_fd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &client_ev);

                std::cout << "New client connected: " << client_fd << "\n";
            } 
            // --- Data from client ---
            else if (events[i].events & EPOLLIN) {
                char buf[4096];
                while (true) {
                    ssize_t count = read(fd, buf, sizeof(buf));
                    if (count > 0) {
                        std::cout << "Received from " << fd << ": " 
                                  << std::string(buf, count) << "\n";
                    } else if (count == 0) {
                        // Client closed
                        close(fd);
                        std::cout << "Client disconnected: " << fd << "\n";
                        break;
                    } else {
                        // EAGAIN means no more data now
                        if (errno != EAGAIN) perror("read");
                        break;
                    }
                }
            }
        }
    }

    close(listen_fd);
    close(epfd);
    return 0;
}


/*
Event Handling

If listen_fd is ready → call accept() to create a new client socket.

If a client fd is ready → call read() (and later write()) until EAGAIN.

If read() returns 0 → client closed, so close(fd) and remove it from epoll.

*/