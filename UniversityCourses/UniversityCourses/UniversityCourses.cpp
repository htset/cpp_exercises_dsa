#include <iostream>
#include <string>

#define MAX_COURSES 100

using namespace std;

// Structure to represent a student
class Student
{
public:
  int id;
  string name;
  int courses[MAX_COURSES];
  int courseCount;
};

// Structure to represent a course
class Course
{
public:
  int id;
  string name;
  int prereqCount;
  int prereqIDs[MAX_COURSES]; // IDs of prerequisite courses

  // Check if a student can enroll in a course
  bool canEnroll(Student student)
  {
    for (int i = 0; i < prereqCount; ++i)
    {
      bool hasPrereq = false;
      for (int j = 0; j < student.courseCount; ++j)
      {
        if (student.courses[j] == prereqIDs[i])
        {
          hasPrereq = true;
          break;
        }
      }
      if (!hasPrereq)
      {
        return false;
      }
    }
    return true;
  }
};

int main()
{
  // Sample course data
  Course courses[] = {
      {0, "Intro to Programming", 0, {-1}}, // -1 represents no prerequisite
      {1, "Data Structures", 1, {0}},
      {2, "Algorithms", 1, {1}},
      {3, "Database Management", 1, {0}},
      {4, "Web Development", 1, {0}},
      {5, "Operating Systems", 2, {1, 2}},
      {6, "Computer Networks", 2, {1, 5}},
      {7, "Software Engineering", 2, {1, 2}},
      {8, "Machine Learning", 2, {1, 2}},
      {9, "Distributed Systems", 1, {5}},
      {10, "Cybersecurity", 2, {2, 3}},
      {11, "Cloud Computing", 2, {2, 3}},
      {12, "Mobile App Development", 1, {4}},
      {13, "Game Development", 1, {0}},
      {14, "Artificial Intelligence", 2, {2, 8}},
      {15, "Big Data Analytics", 2, {2, 3}},
      {16, "Blockchain Technology", 2, {2, 3}},
      {17, "UI/UX Design", 1, {14}},
      {18, "Embedded Systems", 2, {1, 5}},
      {19, "Computer Graphics", 1, {0}}
  };

  // Create a sample student
  Student student;
  student.id = 1;
  student.name = "John Doe";
  student.courseCount = 5;
  student.courses[0] = 0; // Intro to Programming
  student.courses[1] = 1; // Data Structures
  student.courses[2] = 2; // Algorithms
  student.courses[3] = 3; // Database Management
  student.courses[4] = 4; // Web Development

  // Check if the student can enroll in specific courses
  Course targetCourses[] = {
      courses[13], // Game Development
      courses[16], // Blockchain Technology
      courses[17], // UI/UX Design (student cannot enroll)
      courses[18]  // Embedded Systems
  };

  cout << "Enrollment status for " << student.name << ": " << endl;
  for (int i = 0; i < 4; ++i)
  { // Loop through all four courses
    if (targetCourses[i].canEnroll(student))
    {
      cout << "- Can enroll in " << targetCourses[i].name << endl;
    }
    else
    {
      cout << "- Cannot enroll in " << targetCourses[i].name 
          << " due to missing prerequisites." << endl;
    }
  }

  return 0;
}
