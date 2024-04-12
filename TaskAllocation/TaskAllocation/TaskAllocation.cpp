#include <iostream>
#include <queue>
#include <vector>
#include <string>

using namespace std;

// Task structure
class Task
{
public:
  string description;
  int duration;

  Task(const string& description, int duration) 
    : description(description), duration(duration) {}
};

// Worker structure
class Worker
{
public:
  int id;
  int workload;

  Worker(int id, int workload) 
    : id(id), workload(workload) {}

  // Overload the < operator for priority comparison
  bool operator<(const Worker& other) const
  {
    // Workers with shorter workload have higher priority
    return workload > other.workload;
  }
};

void addTask(priority_queue<Worker>& workerQueue, vector<Task>& tasks);
void displayTasks(const vector<Task>& tasks);
void printWorkersQueue(const priority_queue<Worker>& workerQueue);

int main()
{
  vector<Task> tasks;
  priority_queue<Worker> workerQueue;

  int numWorkers;
  cout << "Enter the number of workers: ";
  cin >> numWorkers;
  cin.ignore();

  // Initialize workers with ID and 0 workload
  for (int i = 0; i < numWorkers; ++i)
  {
    workerQueue.push(Worker(i, 0));
  }

  int choice;
  do
  {
    cout << "\nMenu:\n1. Add Task\n2. Display Tasks\n3. Print Workers Queue\n4. Exit\nEnter your choice: ";
    cin >> choice;
    cin.ignore();

    switch (choice)
    {
    case 1:
      addTask(workerQueue, tasks);
      break;
    case 2:
      displayTasks(tasks);
      break;
    case 3:
      printWorkersQueue(workerQueue);
      break;
    case 4:
      cout << "Exiting program...\n";
      break;
    default:
      cout << "Invalid choice! Please try again.\n";
    }
  } while (choice != 4);

  return 0;
}

// Function to add a task and allocate it to a worker
void addTask(priority_queue<Worker>& workerQueue, vector<Task>& tasks)
{
  string description;
  int duration;

  cout << "Enter task description: ";
  getline(cin, description);
  cout << "Enter task duration (in minutes): ";
  cin >> duration;
  cin.ignore();

  if (workerQueue.empty())
  {
    cout << "No workers available! Task cannot be assigned." << endl;
    return;
  }

  // Dequeue the worker with the shortest workload
  Worker worker = workerQueue.top();
  workerQueue.pop();

  // Assign the task to the worker and update workload
  tasks.push_back(Task(description, duration));
  cout << "Task added successfully and allocated to Worker " 
    << worker.id << "!" << endl;

  // Update workload
  worker.workload += duration; 
  // Enqueue the worker back with updated workload
  workerQueue.push(worker); 
}

// Function to display all tasks
void displayTasks(const vector<Task>& tasks)
{
  cout << "Task List:" << endl;
  for (const Task& task : tasks)
  {
    cout << "Task description: " << task.description 
      << ", Duration: " << task.duration << " minutes" << endl;
  }
}

// Function to print the workers queue
void printWorkersQueue(const priority_queue<Worker>& workerQueue)
{
  priority_queue<Worker> tempQueue = workerQueue; // Create a copy of the queue
  cout << "Workers Queue:" << endl;
  while (!tempQueue.empty())
  {
    Worker worker = tempQueue.top();
    cout << "Worker ID: " << worker.id 
      << ", Workload: " << worker.workload << " minutes" << endl;
    tempQueue.pop();
  }
}

