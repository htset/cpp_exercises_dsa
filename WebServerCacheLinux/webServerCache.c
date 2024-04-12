#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define PORT 8080
#define MAX_REQUEST_SIZE 1024
#define CACHE_SIZE 3

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
  void handleRequest(int clientSocket);
};

int main()
{
  HttpServer httpServer;
	int serverSocket, clientSocket;
	struct sockaddr_in serverAddr, clientAddr;
	socklen_t addrSize;

	// Create socket
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0)
	{
		perror("Socket creation failed");
		return errno;
	}

	// Bind socket
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(PORT);
	if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		perror("Socket binding failed");
		return errno;
	}

	// Listen for connections
	if (listen(serverSocket, 5) < 0)
	{
		perror("Listen failed");
		return errno;
	}

	cout << "Server listening on port " << PORT << endl;

	while (1)
	{
		addrSize = sizeof(clientAddr);
		// Accept incoming connection
		clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrSize);
		if (clientSocket < 0)
		{
			perror("Accept failed");
			return errno;
		}

		// Convert client IP address and port to printable format
		char clientIP[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);

		// Print connection information
		printf("Connection accepted from %s:%d\n", clientIP, ntohs(clientAddr.sin_port));

		// Handle client request
		httpServer.handleRequest(clientSocket);

		// Close client socket
		close(clientSocket);
	}

	// Close server socket
	close(serverSocket);

	return 0;
}


void HttpServer::handleRequest(int clientSocket)
{
  char request[MAX_REQUEST_SIZE];
  int bytesReceived = recv(clientSocket, request, sizeof(request), 0);
  if (bytesReceived <= 0)
  {
    perror("recv");
    return;
  }
  request[bytesReceived] = '\0';

  string req = string(request);
  string url;
  char delimiter = ' ';
  istringstream iss(req);

  getline(iss, url, delimiter);
  if(url != "GET")
  {
    perror("Only GET requests are supported.");
    return;
  }

  getline(iss, url, delimiter);
  if (url == " ")
  {
    perror("Invalid request format.");
    return;
  }

  string content = cache.getContent(url);
  if (content == "")
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
