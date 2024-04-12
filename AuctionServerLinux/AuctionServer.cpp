#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>

#define PORT 8080
#define MAX_CLIENTS 5

using namespace std;

typedef struct
{
  int socket;
  struct sockaddr_in address;
  int addr_len;
  int id;
} Client;

Client clients[MAX_CLIENTS];
int server_socket;
int best_bid = 0;
int winning_client = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
time_t last_bid_time;

void *timer_thread(void *arg);
void *client_handler(void *arg);

int main()
{
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  pthread_t threads[MAX_CLIENTS];
  pthread_t timer;
  time(&last_bid_time);

  // Create server socket
  if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  // Bind server socket
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);
  if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
  {
    perror("Bind failed");
    close(server_socket);
    exit(EXIT_FAILURE);
  }

  // Listen for incoming connections
  if (listen(server_socket, MAX_CLIENTS) == -1)
  {
    perror("Listen failed");
    close(server_socket);
    exit(EXIT_FAILURE);
  }

  cout << "Server listening on port" << PORT << endl;

  // Create thread for timer
  if (pthread_create(&timer, NULL, timer_thread, NULL) != 0)
  {
    perror("Timer thread creation failed");
    close(server_socket);
    exit(EXIT_FAILURE);
  }

  while (1)
  {
    int client_socket;
    Client client;

    // Accept incoming connection
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_socket == -1)
    {
      perror("Accept failed");
      close(server_socket);
      exit(EXIT_FAILURE);
    }

    // Add client to the clients array
    int c = 0;
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
      if (clients[i].socket == 0)
      {
        clients[i].socket = client_socket;
        clients[i].address = client.address;
        clients[i].addr_len = client.addr_len;
        clients[i].id = i + 1;
        c = i;
        cout << "Client no." << i+1 << " connected." << endl;
        break;
      }
    }

    if (pthread_create(&threads[c], NULL, client_handler, (void *)&clients[c]) != 0)
    {
      perror("Thread creation failed");
      close(server_socket);
      exit(EXIT_FAILURE);
    }
  }

  close(server_socket);

  return 0;
}


void *client_handler(void *arg)
{
  Client *client = (Client *)arg;
  int client_socket = client->socket;
  char buffer[1024] = {0};
  int bid_amount;

  while (1)
  {
    // Receive bid from client
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received == -1)
    {
      perror("Receive failed");
      break;
    }
    else if (bytes_received == 0)
    {
      cout << "Client disconnected." << endl;
      break;
    }

    pthread_mutex_lock(&mutex);
    bid_amount = atoi(buffer);
    cout << "Received bid " << bid_amount << " from client " << client->id << endl;
    if (bid_amount > best_bid)
    {
      best_bid = bid_amount;
      winning_client = client->id;

      // Inform all clients about the new best bid
      string msg = "New best bid: " + to_string(best_bid) 
        + "(Client: " + to_string(winning_client) + ")";
      for (int i = 0; i < MAX_CLIENTS; i++)
      {
        if (clients[i].socket != 0)
        {
          send(clients[i].socket, msg.c_str(), strlen(msg.c_str()), 0);
        }
      }
      time(&last_bid_time); // Update the last bid time
    }
    else
    {
      string msg = "Received lower bid. Best bid remains at: " + to_string(best_bid);
      for (int i = 0; i < MAX_CLIENTS; i++)
      {
        if (clients[i].socket != 0)
        {
          send(clients[i].socket, msg.c_str(), strlen(msg.c_str()), 0);
        }
      }
    }

    pthread_mutex_unlock(&mutex);
  }
}


void *timer_thread(void *arg)
{
  while (1)
  {
    time_t current_time;
    time(&current_time);
    double elapsed_seconds = difftime(current_time, last_bid_time);

    if (elapsed_seconds >= 20)
    {
      pthread_mutex_lock(&mutex);
      cout << "Auction finished. Winning bid: " << best_bid 
        << ", winner: client no." << winning_client << endl;

      // Inform all clients about the end of the auction
      string msg = "Auction finished. Winning bid: " + to_string(best_bid) 
        + ", winner: client no." + to_string(winning_client);
      for (int i = 0; i < MAX_CLIENTS; i++)
      {
        if (clients[i].socket != 0)
        {
          send(clients[i].socket, msg.c_str(), strlen(msg.c_str()), 0);
        }
      }
      close(server_socket);

      pthread_mutex_unlock(&mutex);

      exit(0);
    }

    sleep(1);
  }

  pthread_exit(NULL);
}
