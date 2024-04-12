#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <stack>

using namespace std;

// Structure to represent each city
struct City
{
  string name;
  unordered_map<string, int> flights; // Map of connected cities and their costs
  City(string n) : name(n) {}
};

// Graph class to represent all cities and flights
class FlightGraph
{
public:
  void addCity(const string& name);
  void addFlight(const string& src, const string& dest, int cost);
  vector<string> findCheapestRoute(const string& src, const string& dest, 
    int& totalPrice);
  void displayAllFlights(const string& src, const string& dest);
private:
  unordered_map<string, City*> cities; // Map of city names and their objects
  void dfs(const string& src, const string& dest, unordered_set<string>& visited, 
    stack<string> path);
  void printPath(stack<string> path);
};

// Add a city to the graph
void FlightGraph::addCity(const string& name)
{
  cities[name] = new City(name);
}

// Add a flight between two cities and its cost
void FlightGraph::addFlight(const string& src, const string& dest, int cost)
{
  // Assuming flights are bidirectional
  cities[src]->flights[dest] = cost;
  cities[dest]->flights[src] = cost;
}

// Function to find the cheapest route between two cities using Dijkstra's algorithm
vector<string> FlightGraph::findCheapestRoute(const string& src, const string& dest, int& totalPrice)
{
  unordered_map<string, int> dist;
  unordered_map<string, string> prev;
  priority_queue<pair<int, string>, vector<pair<int, string>>, greater<pair<int, string>>> pq;

  for (auto& city : cities)
  {
    dist[city.first] = INT_MAX;
    prev[city.first] = "";
  }

  dist[src] = 0;
  pq.push({ 0, src });

  while (!pq.empty())
  {
    string u = pq.top().second;
    pq.pop();

    for (auto& flight : cities[u]->flights)
    {
      string v = flight.first;
      int cost = flight.second;

      if (dist[u] != INT_MAX && dist[u] + cost < dist[v])
      {
        dist[v] = dist[u] + cost;
        prev[v] = u;
        pq.push({ dist[v], v });
      }
    }
  }

  // Reconstructing the path
  vector<string> path;
  string current = dest;
  while (!prev[current].empty())
  {
    path.push_back(current);
    current = prev[current];
  }
  path.push_back(src);
  reverse(path.begin(), path.end());

  // Calculate total price
  totalPrice = dist[dest];

  return path;
}

// Display all possible flights between two cities using DFS
void FlightGraph::displayAllFlights(const string& src, const string& dest)
{
  if (cities.find(src) == cities.end() || cities.find(dest) == cities.end())
  {
    cout << "Invalid cities entered." << endl;
    return;
  }

  unordered_set<string> visited;
  stack<string> path;
  path.push(src);
  dfs(src, dest, visited, path);
}

// Recursive DFS function to find all flights between source and destination
void FlightGraph::dfs(const string& src, 
  const string& dest, unordered_set<string>& visited, stack<string> path)
{
  visited.insert(src);

  if (src == dest)
  {
    printPath(path);
  }
  else
  {
    for (const auto& flight : cities[src]->flights)
    {
      if (visited.find(flight.first) == visited.end())
      {
        path.push(flight.first);
        dfs(flight.first, dest, visited, path);
        path.pop();
      }
    }
  }

  visited.erase(src);
}

// Helper function to print a path (stack content)
void FlightGraph::printPath(stack<string> path)
{
  stack<string> temp;
  while (!path.empty())
  {
    temp.push(path.top());
    path.pop();
  }
  while (!temp.empty())
  {
    cout << temp.top();
    temp.pop();
    if (!temp.empty())
      cout << " -> ";
  }
  cout << endl;
}

int main()
{
  FlightGraph graph;

  graph.addCity("London");
  graph.addCity("Paris");
  graph.addCity("Berlin");
  graph.addCity("Rome");
  graph.addCity("Madrid");
  graph.addCity("Amsterdam");

  graph.addFlight("London", "Paris", 100);
  graph.addFlight("London", "Berlin", 150);
  graph.addFlight("London", "Madrid", 200);
  graph.addFlight("Paris", "Berlin", 120);
  graph.addFlight("Paris", "Rome", 180);
  graph.addFlight("Berlin", "Rome", 220);
  graph.addFlight("Madrid", "Rome", 250);
  graph.addFlight("Madrid", "Amsterdam", 170);
  graph.addFlight("Amsterdam", "Berlin", 130);

  string departure, destination;
  cout << "Enter departure city: ";
  cin >> departure;
  cout << "Enter destination city: ";
  cin >> destination;

  // Display all possible flights
  cout << "All possible flights between " << departure 
    << " and " << destination << ":" << endl;
  graph.displayAllFlights(departure, destination);

  // Find the cheapest route and total price
  int totalPrice;
  vector<string> route = graph.findCheapestRoute(departure, destination, totalPrice);

  // Display the cheapest route and total price
  cout << "Cheapest Route: ";
  for (string city : route)
  {
    cout << city << " -> ";
  }
  cout << "Total Price: " << totalPrice << endl;

  return 0;
}
