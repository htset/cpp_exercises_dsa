#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"

using namespace std;

int client_socket;

void *receive_handler(void *arg);

int main()
{
  struct sockaddr_in server_addr;
  pthread_t receive_thread;

  // Create client socket
  if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  // Configure server address
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0)
  {
    perror("Invalid address");
    close(client_socket);
    exit(EXIT_FAILURE);
  }

  // Connect to server
  if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
  {
    perror("Connect failed");
    close(client_socket);
    exit(EXIT_FAILURE);
  }

  cout << "Connected to server." << endl;

  // Create thread to handle receiving messages from server
  if (pthread_create(&receive_thread, NULL, receive_handler, NULL) != 0)
  {
    perror("Thread creation failed");
    close(client_socket);
    exit(EXIT_FAILURE);
  }

  // Main thread to send messages to the server
  string buffer;
  while (1)
  {
    cout << "\nEnter your bid (or 'q' to quit): ";
    getline(cin, buffer);

    if (buffer == "q")
    {
      break;
    }

    if (send(client_socket, buffer.c_str(), strlen(buffer.c_str()), 0) == -1)
    {
      perror("Send failed");
      close(client_socket);
      exit(EXIT_FAILURE);
    }
  }

  return 0;
}

void *receive_handler(void *arg)
{
  char buffer[1024] = {0};

  while (1)
  {
    // Receive message from server
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received == -1)
    {
      perror("Receive failed");
      break;
    }
    else if (bytes_received == 0)
    {
      cout << "Server disconnected." << endl;
      break;
    }

    buffer[bytes_received] = '\0';
    cout << "\nServer: " << buffer << endl;

    if (strncmp(buffer, "Auction", 7) == 0)
    {
      cout << "Auction ended. Exiting program." << endl;

      close(client_socket);

      exit(0);
    }
    memset(buffer, 0, sizeof(buffer));
    cout << "\nEnter your bid (or 'q' to quit): ";
  }

  close(client_socket);
  pthread_exit(NULL);
}
