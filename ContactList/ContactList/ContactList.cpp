#include <iostream>
#define HASH_SIZE 100

using namespace std;

class Contact
{
public:
  string name;
  string phone;
  Contact* next;
};

class ContactList
{
public:
  Contact* bucket_table[HASH_SIZE];

  ContactList();
  unsigned int hash(const char* name);
  void contact_add(string name, string phone);
  void contact_remove(string name);
  void contact_search(string name);
};

ContactList::ContactList()
{
  for (int i = 0; i < HASH_SIZE; i++)
  {
    bucket_table[i] = nullptr;
  }
}

unsigned int ContactList::hash(const char* name)
{
  unsigned int hash = 0;
  int c;
  while (c = *name++)
  {
    hash = ((hash << 5) + hash) + c;
  }
  return hash % HASH_SIZE;
}

void ContactList::contact_add(string name, string phone)
{
  unsigned int hash_index = hash(name.c_str());
  Contact* new_contact = new Contact();
  if (!new_contact)
  {
    cout << "Memory allocation failed." << endl;
    return;
  }
  new_contact->name = name;
  new_contact->phone = phone;
  new_contact->next = this->bucket_table[hash_index];
  this->bucket_table[hash_index] = new_contact;
}

void ContactList::contact_remove(string name)
{
  unsigned int index = hash(name.c_str());
  Contact* contact = this->bucket_table[index];
  Contact* previous = nullptr;

  while (contact != nullptr)
  {
    if (contact->name == name)
    {
      if (previous == nullptr)
      {
        // Contact to remove is the head of the list
        this->bucket_table[index] = contact->next;
      }
      else
      {
        // Contact to remove is not the head of the list
        previous->next = contact->next;
      }
      delete contact;
      cout << "Contact '" << name << "' removed successfully." << endl;
      return;
    }
    previous = contact;
    contact = contact->next;
  }
  cout << "Contact '" << name << "' not found." << endl;
}

void ContactList::contact_search(string name)
{
  unsigned int hash_index = hash(name.c_str());
  Contact* contact = this->bucket_table[hash_index];
  while (contact != nullptr)
  {
    if (contact->name == name)
    {
      cout << "Name: " << contact->name 
          << "\nPhone Number : " << contact->phone << endl;
      return;
    }
    contact = contact->next;
  }
  cout << "Contact '" << name << "' not found." << endl;
}

int main()
{
  ContactList phonebook;
  phonebook.contact_add("John", "235454545");
  phonebook.contact_add("Jane", "775755454");
  phonebook.contact_add("George", "4344343477");

  phonebook.contact_search("John");
  phonebook.contact_search("Alex");
  phonebook.contact_search("George");

  phonebook.contact_remove("Jake");
  phonebook.contact_remove("Jane");
  phonebook.contact_search("Jane");

  return 0;
}
