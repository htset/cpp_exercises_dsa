#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <cstdlib>
#include <winsock2.h>
#include <ws2tcpip.h>

#define PORT 8080
#define MAX_REQUEST_SIZE 1024
#define CACHE_SIZE 3

#pragma comment(lib, "ws2_32.lib")

using namespace std;

// Node structure for doubly linked list
class Node
{
public:
  string url;
  string content;
  Node* prev;
  Node* next;
};

// Cache structure
class LRUCache
{
  int capacity;
  int size;
  Node* head;
  Node* tail;

public:
  LRUCache();
  ~LRUCache();
  Node* createNode(string url, string content);
  void deleteNode(Node* node);
  void insertAtHead(Node* node);
  void moveToHead(Node* node);
  string getContent(string url);
  void putContent(string url, string content);
};

class HttpServer
{
  LRUCache cache;
public:
  HttpServer():cache() { }
  void handleRequest(SOCKET clientSocket);
};

int main()
{
  HttpServer httpServer;
  WSADATA wsaData;
  SOCKET listenSocket, clientSocket;
  struct sockaddr_in serverAddr, clientAddr;
  int addrLen = sizeof(struct sockaddr_in);

  // Initialize Winsock
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
  {
    perror("WSAStartup");
    return 1;
  }

  // Create socket
  if ((listenSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
  {
    perror("socket");
    return 1;
  }

  // Initialize server address
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  serverAddr.sin_port = htons(PORT);

  // Bind socket to address
  if (bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
  {
    perror("bind");
    closesocket(listenSocket);
    WSACleanup();
    return 1;
  }

  // Listen for connections
  if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
  {
    perror("listen");
    closesocket(listenSocket);
    WSACleanup();
    return 1;
  }

  cout << "Server started on port " << PORT << endl;

  while (1)
  {
    // Accept connections
    if ((clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &addrLen)) == INVALID_SOCKET)
    {
      perror("accept");
      continue;
    }

    char* str = new char[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddr.sin_addr), str, INET_ADDRSTRLEN);
    cout << "Connection from " << str << endl;
    // Handle client request
    httpServer.handleRequest(clientSocket);

    // Close client socket
    closesocket(clientSocket);
  }

  // Close server socket
  closesocket(listenSocket);

  // Cleanup Winsock
  WSACleanup();

  return 0;
}

/*****************
** HttpServer functions
*******************/

// Handle HTTP GET requests 
void HttpServer::handleRequest(SOCKET clientSocket)
{
  char request[MAX_REQUEST_SIZE];
  int bytesReceived = recv(clientSocket, request, sizeof(request), 0);
  if (bytesReceived <= 0)
  {
    perror("recv");
    return;
  }

  //create a string object
  string req = string(request);
  string url;
  char delimiter = ' ';
  //use istringstream to parse the string
  istringstream iss(req);

  //get the text until the delimiter (' ')
  getline(iss, url, delimiter);
  if(url != "GET")
  {
    perror("Only GET requests are supported.");
    return;
  }

  //get the next text until the delimiter (' ')
  getline(iss, url, delimiter);
  if (url == " ")
  {
    perror("Invalid request format.");
    return;
  }

  //locate content in cache
  string content = cache.getContent(url);
  if (content == "") //not found
  {
    // Serve the page from disk
    ifstream file(url.substr(1, url.length()));
    if (!file.is_open())
    {
      // File not found, return 404 response
      char response[] = "HTTP/1.1 404 Not Found\n\n";
      cout << "File not found: " << url.substr(1, url.length()) << endl;
      send(clientSocket, response, strlen(response), 0);
      return;
    }

    string response;
    response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
    send(clientSocket, response.c_str(), strlen(response.c_str()), 0);

    char buffer[MAX_REQUEST_SIZE] = {0};

    while (file.read(buffer, sizeof(buffer)))
    {
      send(clientSocket, buffer, file.gcount(), 0);
    }
    //get the last bytes of the request, that are less than the buffer size
    send(clientSocket, buffer, file.gcount(), 0);
    file.close();
    cout << "Got content from file: " << buffer << endl;

    // Cache the page content
    cache.putContent(url, buffer);
  }
  else
  {
    // Serve the page from cache
    string response;
    response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
    send(clientSocket, response.c_str(), strlen(response.c_str()), 0);

    send(clientSocket, content.c_str(), strlen(content.c_str()), 0);
  }
}

/*****************
** Cache functions
*******************/
// Get the content associated with a URL from the cache
string LRUCache::getContent(string url)
{
  Node* current = head;
  while (current != nullptr)
  {
    if (current->url == url)
    {
      moveToHead(current);
      cout << "Got content from cache: " << current->content << endl;

      return current->content;
    }
    current = current->next;
  }
  // Return empty string if the URL is not found in cache
  return "";
}

// Put a URL-content pair into the cache
void LRUCache::putContent(string url, string content)
{
  if (size == CACHE_SIZE)
  {
    deleteNode(tail);
    size--;
  }
  Node* newNode = createNode(url, content);
  insertAtHead(newNode);
  size++;
}

// Create a new node
Node* LRUCache::createNode(string url, string content)
{
  Node* newNode = new Node;
  newNode->url = url;
  newNode->content = content;
  newNode->prev = nullptr;
  newNode->next = nullptr;
  cout << "New node created: " << content << endl;
  return newNode;
}

// Insert a new node at the head of the cache
void LRUCache::insertAtHead(Node* node)
{
  node->next = head;
  node->prev = nullptr;
  if (head != nullptr)
  {
    head->prev = node;
  }
  head = node;
  if (tail == nullptr)
  {
    tail = node;
  }
  cout << "Node inserted at head: " << node->content << endl;
}

// Move a node to the head of the cache
void LRUCache::moveToHead(Node* node)
{
  if (node == head)
  {
    // Node is already at the head, no need to move
    return;
  }

  // Adjust pointers to remove node from its current position
  if (node->prev != nullptr)
  {
    node->prev->next = node->next;
  }
  if (node->next != nullptr)
  {
    node->next->prev = node->prev;
  }

  // Update pointers to insert node at the head
  node->prev = nullptr;
  node->next = head;
  if (head != nullptr)
  {
    head->prev = node;
  }
  head = node;
  if (tail == nullptr)
  {
    tail = node;
  }
  cout << "Node moved to head: " << node->content << endl;
}

// Delete a node from the cache
void LRUCache::deleteNode(Node* node)
{
  if (node == nullptr)
    return;

  // If the node is the head of the list
  if (node == head)
  {
    head = node->next;
  }

  // If the node is the tail of the list
  if (node == tail)
  {
    tail = node->prev;
  }

  // Adjust pointers of neighboring nodes
  if (node->prev != nullptr)
  {
    node->prev->next = node->next;
  }
  if (node->next != nullptr)
  {
    node->next->prev = node->prev;
  }
  cout << "Node deleted: " << node->content << endl;

  delete node;
}

LRUCache::LRUCache()
{
  capacity = CACHE_SIZE;
  size = 0;
  head = nullptr;
  tail = nullptr;
}

LRUCache::~LRUCache()
{
  // Free cache memory
  Node* current = head;
  while (current != nullptr)
  {
    Node* temp = current;
    current = current->next;
    deleteNode(temp);
  }
}
