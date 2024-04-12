#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

class Customer
{
public:
  Customer(string name) : name(name) {}

  string getName()
  {
    return name;
  }

private:
  string name;
};

class Table
{
public:
  Table(int id, int capacity) : id(id), capacity(capacity) {}

  int getCapacity()
  {
    return capacity;
  }

  int getID()
  {
    return id;
  }

private:
  int id;
  int capacity;
};

class Reservation
{
public:
  Reservation(Customer* customer, Table* table, 
    int startTimeSlot, int endTimeSlot)
    : customer(customer), table(table), 
    startTimeSlot(startTimeSlot), endTimeSlot(endTimeSlot) {}

  Customer* getCustomer() 
  {
    return customer;
  }

  Table* getTable()
  {
    return table;
  }

  int getStartTimeSlot()
  {
    return startTimeSlot;
  }

  int getEndTimeSlot()
  {
    return endTimeSlot;
  }

private:
  Customer* customer;
  Table* table;
  int startTimeSlot;
  int endTimeSlot;
};

class Restaurant
{
public:
  Restaurant() {}

  void addTable(Table* table)
  {
    tables.push_back(table);
  }

  // Find if table is available in specified timeslot
  bool isTableAvailable(Table* table, int startTimeSlot, int endTimeSlot)
  {
    for (Reservation* reservation : reservations)
    {
      if (reservation->getTable()->getID() == table->getID() &&
        ((startTimeSlot >= reservation->getStartTimeSlot()
          && startTimeSlot < reservation->getEndTimeSlot())
          ||
          (endTimeSlot > reservation->getStartTimeSlot()
            && endTimeSlot <= reservation->getEndTimeSlot())
          ||
          (startTimeSlot <= reservation->getStartTimeSlot()
            && endTimeSlot >= reservation->getEndTimeSlot())))
      {
        return false;
      }
    }
    return true;
  }

  // Find available tables for specified timeslots
  vector<Table*> findAvailableTables(int capacity, int startTimeSlot, int endTimeSlot) 
  {
    vector<Table*> availableTables;
    for (Table* table : tables)
    {
      if (table->getCapacity() >= capacity 
        && isTableAvailable(table, startTimeSlot, endTimeSlot))
      {
        availableTables.push_back(table);
      }
    }
    // Sort available tables by capacity in ascending order
    sort(availableTables.begin(), availableTables.end(), [](Table* a, Table* b)
      { return a->getCapacity() < b->getCapacity(); });

    return availableTables;
  }

  void addReservation(const char* name, int capacity, int startSlot, int endSlot)
  {
    vector<Table*> availableTables = findAvailableTables(capacity, startSlot, endSlot);
    if (!availableTables.empty())
    {
      // if a suitable table is found, create a reservation
      reservations.push_back(new Reservation(
                                new Customer(name), 
                                availableTables.front(), 
                                startSlot, 
                                endSlot));
      cout << "Reservation successfully added.\n";
    }
    else
    {
      cout << "No available tables for the requested time slot.\n";
    }
  }

  void printReservations() 
  {
    cout << "All reservations:\n";
    for (Reservation* reservation : reservations)
    {
      cout << "Customer: " << reservation->getCustomer()->getName()
        << ", Table Capacity: " << reservation->getTable()->getCapacity()
        << ", Start Time Slot: " << reservation->getStartTimeSlot()
        << ", End Time Slot: " << reservation->getEndTimeSlot() << "\n";
    }
  }

private:
  vector<Table*> tables;
  vector<Reservation*> reservations;
};

int main()
{
  Restaurant restaurant;

  // Add tables
  restaurant.addTable(new Table(1,6));
  restaurant.addTable(new Table(2,4));
  restaurant.addTable(new Table(3,2));

  // Find available tables for a new reservation
  restaurant.addReservation("Customer 1", 4, 1, 3);
  restaurant.addReservation("Customer 2", 6, 2, 4);
  restaurant.addReservation("Customer 3", 4, 3, 5);
  restaurant.addReservation("Customer 4", 4, 1, 3);

  restaurant.printReservations();

  return 0;
}
