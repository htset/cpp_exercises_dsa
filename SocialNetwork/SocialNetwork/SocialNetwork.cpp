#include <iostream>
#define MAX_USERS 100
using namespace std;

struct FriendNode
{
  string name;
  FriendNode* next;
};

struct User
{
  string name;
  FriendNode* friends;
};

class Queue
{
  struct QueueNode
  {
    int user_index;
    QueueNode* next;
  };

  QueueNode* front;
  QueueNode* rear;

public:
  Queue();
  bool is_empty();
  void enqueue(int user_index);
  int dequeue();
};

Queue::Queue()
{
  front = rear = nullptr;
}

bool Queue::is_empty()
{
  return (front == nullptr);
}

void Queue::enqueue(int user_index)
{
  QueueNode* newNode = new QueueNode;
  newNode->user_index = user_index;
  newNode->next = nullptr;

  if (is_empty())
  {
    front = rear = newNode;
  }
  else
  {
    rear->next = newNode;
    rear = newNode;
  }
}

int Queue::dequeue()
{
  if (is_empty())
  {
    cout << "Queue is empty!" << endl;
    return -1;
  }

  QueueNode* temp = front;
  int user_index = temp->user_index;
  front = front->next;

  if (front == nullptr)
  {
    rear = nullptr;
  }

  delete temp;
  return user_index;
}

class Graph
{
  User users[MAX_USERS];
  int num_users;

public:
  Graph();
  void add_user(string name);
  void add_connection(int src, int dest);
  void recommend_friends(int user_index);
};

Graph::Graph()
{
  num_users = 0;
}

void Graph::add_user(string name)
{
  if (num_users >= MAX_USERS)
  {
    cout << "Max user limit reached!" << endl;
    return;
  }

  users[num_users].name = name;
  users[num_users].friends = nullptr;
  num_users++;
}

void Graph::add_connection(int src, int dest)
{
  if (src < 0 || src >= num_users || dest < 0 || dest >= num_users)
  {
    cout << "Invalid user index!" << endl;
    return;
  }

  FriendNode* new_node_src = new FriendNode;
  new_node_src->name = users[dest].name;
  new_node_src->next = users[src].friends;
  users[src].friends = new_node_src;

  FriendNode* new_node_dest = new FriendNode;
  new_node_dest->name = users[src].name;
  new_node_dest->next = users[dest].friends;
  users[dest].friends = new_node_dest;
}

void Graph::recommend_friends(int user_index)
{
  cout << "Recommended friends for " << users[user_index].name << ":" << endl;

  Queue queue;
  int visited[MAX_USERS] = { 0 };

  visited[user_index] = 1;
  queue.enqueue(user_index);

  while (!queue.is_empty())
  {
    int current_user_index = queue.dequeue();
    FriendNode* current = users[current_user_index].friends;

    while (current != nullptr)
    {
      int friend_index = -1;
      for (int i = 0; i < num_users; i++)
      {
        if (current->name == users[i].name)
        {
          friend_index = i;
          break;
        }
      }

      if (friend_index != -1 && !visited[friend_index])
      {
        cout << "- " << current->name << endl;
        visited[friend_index] = 1;
        queue.enqueue(friend_index);
      }

      current = current->next;
    }
  }
}

int main()
{
  Graph graph;
  graph.add_user("User A");
  graph.add_user("User B");
  graph.add_user("User C");
  graph.add_user("User D");
  graph.add_user("User E");
  graph.add_user("User F");
  graph.add_user("User G");
  graph.add_user("User H");

  graph.add_connection(0, 1);
  graph.add_connection(1, 2);
  graph.add_connection(2, 3);
  graph.add_connection(4, 5);
  graph.add_connection(5, 7);
  graph.add_connection(3, 6);

  graph.recommend_friends(0);
  graph.recommend_friends(1);
  graph.recommend_friends(7);

  return 0;
}
