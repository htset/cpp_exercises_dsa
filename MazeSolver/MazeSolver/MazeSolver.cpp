#include <iostream>
#define ROWS 15
#define COLS 15
using namespace std;

class Point
{
public:
  int row, col;

  Point()
  {
    row = 0;
    col = 0;
  }

  Point(int x, int y)
  {
    row = x;
    col = y;
  }
};

template <class T>
class Stack
{
  T* items;
  int top;

public:
  Stack(int capacity)
  {
    items = new T[capacity];
    top = -1;
  }

  bool is_empty()
  {
    return top == -1;
  }

  void push(T t)
  {
    top++;
    items[top] = t;
  }

  T pop()
  {
    return items[top--];
  }
};

class Maze
{
  Stack<Point> stack;

  int matrix[ROWS][COLS] = {
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0},
    {0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0},
    {0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0},
    {0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0},
    {0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0},
    {0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0},
    {0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0},
    {0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0},
    {0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0},
    {0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}
  };

public:
  Maze();
  bool canMove(int row, int col);
  void print();
  void print_path();
  int solve(int row, int col);
};

Maze::Maze() : stack(ROWS* COLS) { }

// Check if we can move to this cell
bool Maze::canMove(int row, int col)
{
  return (row >= 0 
            && row < ROWS 
            && col >= 0 
            && col < COLS 
            && matrix[row][col] == 0);
}

void Maze::print()
{
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++)
    {
      cout << matrix[i][j] << " ";
    }
    cout << "" << endl;
  }
}

// Solve the maze using backtracking
int Maze::solve(int row, int col)
{
  if (row == ROWS - 1 && col == COLS - 1)
  {
    // destination reached
    stack.push(Point(row, col));
    return 1;
  }

  if (canMove(row, col))
  {
    stack.push(Point(row, col));
    matrix[row][col] = 2; // Marking visited

    // Move right
    if (solve(row, col + 1))
      return 1;

    // Move down
    if (solve(row + 1, col))
      return 1;

    // Move left
    if (solve(row, col - 1))
      return 1;

    // Move up
    if (solve(row - 1, col))
      return 1;

    // If none of the above movements work, backtrack
    stack.pop();
    return 0;
  }

  return 0;
}

void Maze::print_path()
{
  while (!stack.is_empty()) {
    Point p = stack.pop();
    cout << "(" << p.row << ", " << p.col << "), ";
  }
}

int main()
{
  Maze maze;

  cout << "This is the maze:" << endl;
  maze.print();

  if (maze.solve(0, 0))
  {
    cout << "\n\n This is the path found:" << endl;
    maze.print_path();

    cout << "\n\nThis is the maze with all the points crossed:" << endl;
    maze.print();
  }
  else
  {
    cout << "No path found" << endl;
  }
  return 0;
}
