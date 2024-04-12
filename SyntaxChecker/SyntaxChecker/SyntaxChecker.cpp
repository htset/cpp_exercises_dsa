#include <iostream>
#include <string>
#include <fstream>
#define MAX_SIZE 100

using namespace std;

class Stack
{
  char items[MAX_SIZE];
  int top;

public:
  Stack();
  void push(char c);
  char pop();
  bool check_empty();
};

Stack::Stack()
{
  top = -1;
}

void Stack::push(char c)
{
  if (top == MAX_SIZE - 1)
  {
    cout << "Stack is full" << endl;
    exit(1);
  }
  items[++top] = c;
}

char Stack::pop()
{
  if (top == -1)
  {
    cout << "Stack is empty" << endl;
    exit(1);
  }
  return items[top--];
}

bool Stack::check_empty()
{
  return (top == -1);
}

int check_balanced(string filename)
{
  ifstream file(filename);
  if (!file.is_open())
  {
    cout << "Error opening file." << endl;
    exit(1);
  }

  char c;
  Stack stack;

  while (file.get(c))
  {
    if (c == '(' || c == '[' || c == '{')
    {
      stack.push(c);
    }
    else if (c == ')' || c == ']' || c == '}')
    {
      // If stack is empty --> Return 'Unbalanced'
      if (stack.check_empty())
      {
        file.close();
        return 0;
      }
      char openingChar = stack.pop();
      if ((c == ')' && openingChar != '(') ||
        (c == ']' && openingChar != '[') ||
        (c == '}' && openingChar != '{'))
      {
        file.close();
        // If closing character doesn't match top of stack
        // --> return 'Unbalanced' 
        return 0;
      }
    }
  }

  // If stack is empty, after we have finished 
  // checking the input file
  // --> return 'Balanced' 
  int result = stack.check_empty();
  file.close();
  return result;
}

int main()
{
  string filename;

  cout << "Path to the source file: ";
  getline(cin, filename);

  if (check_balanced(filename))
    cout << "The input file is balanced." << endl;
  else
    cout << "The input file is not balanced." << endl;

  return 0;
}
