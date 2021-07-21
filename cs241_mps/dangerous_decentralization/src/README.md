To make executable client and server:

clang -O2 -Wall -Wextra -Werror -Wno-error=unused-parameter -Wmissing-declarations -Wmissing-variable-declarations -I./includes -std=c99 -c -MMD -MP -D_GNU_SOURCE -pthread client.c -o .objs/client-release.o

clang -O2 -Wall -Wextra -Werror -Wno-error=unused-parameter -Wmissing-declarations -Wmissing-variable-declarations -I./includes -std=c99 -c -MMD -MP -D_GNU_SOURCE -pthread utils.c -o .objs/utils-release.o

clang .objs/client-release.o .objs/utils-release.o -pthread -lncurses -lm -o client

clang -O2 -Wall -Wextra -Werror -Wno-error=unused-parameter -Wmissing-declarations -Wmissing-variable-declarations -I./includes -std=c99 -c -MMD -MP -D_GNU_SOURCE -pthread server.c -o .objs/server-release.o

clang .objs/server-release.o .objs/utils-release.o -pthread -lncurses -lm -o server

client usage

       Usage: ./client <address> <port> <username> [output_file]

server usage

       Usage: ./server <port1> <port2> <port3>


In my network, each client can connect to a server by port number. When a client is connected to a server, client_fd which is its id will be printed out for sending messages. For example, if node A has client_fd 4, node B has client_fd 5 and both of them connect to server 0, node A will need to let the client to receive user input by typing "hi /5". Only client has client_fd that is specified by number after "/" will receive messages. Any number after "/" should be a valid client_fd. Otherwise, message will not be received by any clients which are connected to the same server. The connection between server was not finished. I thought I could create 3 thread for each server with 3 different port numbers given by user. Each of them will have a different clients array to trace the client_fd(s) so that a node from one server can communicate with a node from another server. Each server will run its process within a while loop and create more threads to handle more clients. If a server receives a clients_fd that is not in its client_fds array, it will search it from other server's array and use flag(s) to indicate that a message should be transfered.  