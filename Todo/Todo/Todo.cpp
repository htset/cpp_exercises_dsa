#include <iostream>
#include <string>
using namespace std;

class Task
{
public:
  string description;
  int priority;
  Task* next;
};

class TodoList
{
  Task* head;
  int size;

public:
  TodoList();
  void add_task(string description, int priority);
  void remove_task(int index);
  void display_tasks();
  void sort_tasks();
};

int main()
{
  TodoList list;

  int choice;
  string description;
  int priority;
  int index;

  do
  {
    cout << "\nTo-Do List Manager" << endl;
    cout << "1. Add Task" << endl;
    cout << "2. Remove Task" << endl;
    cout << "3. Display Tasks" << endl;
    cout << "4. Sort Tasks by Priority" << endl;
    cout << "0. Exit" << endl;
    cout << "Enter your choice: ";
    cin >> choice;
    cin.ignore();

    switch (choice)
    {
    case 1:
      cout << "Enter task description: ";
      getline(cin, description);
      cout << "Enter priority: ";
      cin >> priority;
      cin.ignore();
      list.add_task(description, priority);
      cout << "Task added successfully." << endl;
      break;
    case 2:
      cout << "Enter number of task to remove: ";
      cin >> index;
      cin.ignore();
      list.remove_task(index - 1);
      cout << "Task removed successfully." << endl;
      break;
    case 3:
      cout << "List of tasks:" << endl;
      list.display_tasks();
      break;
    case 4:
      list.sort_tasks();
      cout << "Tasks sorted by priority." << endl;
      break;
    case 5:
      cout << "Exiting..." << endl;
      break;
    default:
      cout << "Invalid choice. Please try again." << endl;
    }
  } while (choice != 0);

  return 0;
}

TodoList::TodoList()
{
  this->head = nullptr;
  this->size = 0;
}

void TodoList::add_task(string description, int priority)
{
  Task* task = new Task;
  if (!task)
  {
    cout << "Memory allocation failed." << endl;
    return;
  }
  task->description = description;
  task->priority = priority;
  task->next = nullptr;

  if (this->head == nullptr)
  {
    // List is empty
    this->head = task;
  }
  else
  {
    Task* temp = this->head;
    // Find the last node
    while (temp->next != nullptr)
    {
      temp = temp->next;
    }
    // Insert the new task after the last node
    temp->next = task;
  }
  this->size++;
}

void TodoList::remove_task(int index)
{
  if (this->head == nullptr)
  {
    cout << "List is empty." << endl;
    return;
  }

  if (index == 0)
  {
    // If we remove the first item in the list
    Task* temp = this->head;
    this->head = this->head->next;
    delete temp;
    this->size--;
    return;
  }

  Task* previous = nullptr;
  Task* current = this->head;
  int i = 0;
  // Go to the selected index
  while (current != nullptr && i < index)
  {
    previous = current;
    current = current->next;
    i++;
  }

  if (current == nullptr)
  {
    cout << "Index out of bounds." << endl;
    return;
  }

  previous->next = current->next;
  delete current;
  this->size--;
}

void TodoList::display_tasks()
{
  Task* temp = this->head;
  int i = 1;
  while (temp != nullptr)
  {
    cout << i++
      << ") Description: " << temp->description 
      << ", Priority : " << temp->priority << endl;

    temp = temp->next;
  }
}

void TodoList::sort_tasks()
{
  int swapped;
  Task* ptr1;
  Task* ptr2 = nullptr;

  if (this->head == nullptr)
    return;

  do
  {
    swapped = 0; // will change if swapping happens
    ptr1 = this->head;

    while (ptr1->next != ptr2)
    {
      if (ptr1->priority > ptr1->next->priority)
      {
        // Swap data of adjacent nodes
        int tempPriority = ptr1->priority;
        ptr1->priority = ptr1->next->priority;
        ptr1->next->priority = tempPriority;

        string tempDescription;
        tempDescription = ptr1->description;
        ptr1->description = ptr1->next->description;
        ptr1->next->description = tempDescription;

        swapped = 1; // swap happened in this loop pass; don't stop yet
      }
      ptr1 = ptr1->next;
    }
    ptr2 = ptr1;
  } while (swapped); // quit loop when no swap happened
}
