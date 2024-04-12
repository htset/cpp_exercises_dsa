#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>

struct Book
{
  char title[100];
  char author[100];
  int available;
};

struct LendingEvent
{
  char bookTitle[100];
  char userName[100];
  time_t lendingDate;
  int returned;
};

class Library 
{
public:
  void book_add();
  void book_list();
  void book_lend();
  void book_return();
  void lending_events_list();
private:
  const std::string booksFilename = "books.bin";
  const std::string lendingFilename = "lending_events.bin";
  void create_books_file(const std::string& filename);
  void create_lending_events_file(const std::string& filename);
};

void Library::book_add()
{
  std::ofstream file(booksFilename, std::ios::binary | std::ios::app);
  if (!file.is_open())
  {
    create_books_file(booksFilename);
    file.open(booksFilename, std::ios::binary | std::ios::app);
  }

  Book book;
  std::cout << "Book title: ";
  std::cin.getline(book.title, sizeof(book.title));

  std::cout << "Author: ";
  std::cin.getline(book.author, sizeof(book.author));

  book.available = 1;

  file.seekp(0, std::ios::end);
  file.write(reinterpret_cast<const char*>(&book), sizeof(Book));
  file.close();
  std::cout << "Book added successfully.\n";
}

void Library::book_list()
{
  std::ifstream file(booksFilename, std::ios::binary | std::ios::app);
  if (!file.is_open())
  {
    create_books_file(booksFilename);
    file.open(booksFilename, std::ios::binary | std::ios::app);
  }

  Book book;
  std::cout << "Books available in the library:\n";
  while (file.read(reinterpret_cast<char*>(&book), sizeof(Book)))
  {
    std::cout << "Title: " << book.title << std::endl;
    std::cout << "Author: " << book.author << std::endl;
    std::cout << "Available: " << (book.available == 1 ? "True" : "False") << std::endl;
    std::cout << "-----------------------------\n";
  }
  file.close();
}

void Library::book_lend()
{
  std::fstream booksFile(booksFilename, std::ios::binary | std::ios::app | std::ios::in | std::ios::out);
  if (!booksFile.is_open())
  {
    create_books_file(booksFilename);
    booksFile.open(booksFilename, std::ios::binary | std::ios::app | std::ios::in | std::ios::out);
  }

  std::fstream lendingFile(lendingFilename, std::ios::binary | std::ios::app | std::ios::in | std::ios::out);
  if (!lendingFile.is_open())
  {
    create_lending_events_file(lendingFilename);
    lendingFile.open(lendingFilename, std::ios::binary | std::ios::app | std::ios::in | std::ios::out);
  }

  char bookTitle[100];
  char userName[100];
  std::cout << "Enter the title of the book to lend: ";
  std::cin.getline(bookTitle, sizeof(bookTitle));

  booksFile.seekg(0, std::ios::beg);
  std::cout << booksFile.tellg();

  Book book;
  int bookFound = 0;
  while (booksFile.read(reinterpret_cast<char*>(&book), sizeof(Book)))
  {
    if (strcmp(book.title, bookTitle) == 0 && book.available)
    {
      book.available = 0;
      booksFile.seekp(-static_cast<long>(sizeof(Book)), std::ios::cur);
      booksFile.write(reinterpret_cast<const char*>(&book), sizeof(Book));
      bookFound = 1;

      std::cout << "Enter your name: ";
      std::cin.getline(userName, sizeof(userName));

      LendingEvent event;
      strcpy(event.bookTitle, bookTitle);
      strcpy(event.userName, userName);
      event.lendingDate = time(nullptr);
      event.returned = 0;

      lendingFile.seekp(0, std::ios::end);
      lendingFile.write(reinterpret_cast<const char*>(&event), sizeof(LendingEvent));
      std::cout << "Book '" << bookTitle << "' has been lent to " << userName << ".\n";
      break;
    }
  }
  if (!bookFound)
  {
    std::cout << "Book '" << bookTitle << "' not found or not available.\n";
  }

  booksFile.close();
  lendingFile.close();
}

void Library::book_return()
{
  std::fstream booksFile(booksFilename, std::ios::binary | std::ios::app | std::ios::in | std::ios::out);
  if (!booksFile.is_open())
  {
    create_books_file(booksFilename);
    booksFile.open(booksFilename, std::ios::binary | std::ios::app | std::ios::in | std::ios::out);
  }

  std::fstream lendingFile(lendingFilename, std::ios::binary | std::ios::app | std::ios::in | std::ios::out);
  if (!lendingFile.is_open())
  {
    create_lending_events_file(lendingFilename);
    lendingFile.open(lendingFilename, std::ios::binary | std::ios::app | std::ios::in | std::ios::out);
  }

  char bookTitle[100];
  std::cout << "Enter the title of the book to return: ";
  std::cin.getline(bookTitle, sizeof(bookTitle));

  booksFile.seekg(0, std::ios::beg);

  Book book;
  int bookFound = 0;
  while (booksFile.read(reinterpret_cast<char*>(&book), sizeof(Book)))
  {
    if (strcmp(book.title, bookTitle) == 0 && !book.available)
    {
      book.available = 1;
      booksFile.seekp(-static_cast<long>(sizeof(Book)), std::ios::cur);
      booksFile.write(reinterpret_cast<const char*>(&book), sizeof(Book));
      bookFound = 1;

      lendingFile.seekg(0, std::ios::beg);
      LendingEvent event;
      while (lendingFile.read(reinterpret_cast<char*>(&event), sizeof(LendingEvent)))
      {
        if (strcmp(event.bookTitle, bookTitle) == 0 && !event.returned)
        {
          event.returned = 1;
          lendingFile.seekp(-static_cast<long>(sizeof(LendingEvent)), std::ios::cur);
          lendingFile.write(reinterpret_cast<const char*>(&event), sizeof(LendingEvent));
          std::cout << "Book '" << bookTitle << "' has been returned.\n";
          break;
        }
      }
      break;
    }
  }
  if (!bookFound)
  {
    std::cout << "Book '" << bookTitle << "' not found or already returned.\n";
  }

  booksFile.close();
  lendingFile.close();
}

void Library::lending_events_list()
{
  std::ifstream lendingFile(lendingFilename, std::ios::binary);
  if (!lendingFile.is_open())
  {
    create_lending_events_file(lendingFilename);
    lendingFile.open(lendingFilename, std::ios::binary);
  }

  LendingEvent event;
  std::cout << "Lending events:\n";
  while (lendingFile.read(reinterpret_cast<char*>(&event), sizeof(LendingEvent)))
  {
    std::cout << "Book Title: " << event.bookTitle << std::endl;
    std::cout << "User Name: " << event.userName << std::endl;
    std::cout << "Lending Date: " << ctime(&event.lendingDate);
    std::cout << "Returned: " << (event.returned == 1 ? "True" : "False") << std::endl;
    std::cout << "-----------------------------\n";
    std::cout << std::endl;
  }
}

void Library::create_books_file(const std::string& filename)
{
  std::ofstream file(filename, std::ios::binary | std::ios::app);
  if (!file.is_open())
  {
    std::cerr << "Error creating books file.\n";
    exit(1);
  }
  std::cout << "Books file created successfully.\n";
}

void Library::create_lending_events_file(const std::string& filename)
{
  std::ofstream file(filename, std::ios::binary | std::ios::app);
  if (!file.is_open())
  {
    std::cerr << "Error creating lending events file.\n";
    exit(1);
  }
  std::cout << "Lending events file created successfully.\n";
}

int main()
{
  Library lib;
  int choice;
  do
  {
    std::cout << "\n1. Add a book\n2. List all books\n3. Lend a book\n4. Return a book\n5. List lending events\n0. Exit\n";
    std::cout << "Enter your choice: ";
    std::cin >> choice;
    std::cin.ignore(); // Consume newline left in the buffer by cin

    switch (choice)
    {
    case 1:
      lib.book_add();
      break;
    case 2:
      lib.book_list();
      break;
    case 3:
      lib.book_lend();
      break;
    case 4:
      lib.book_return();
      break;
    case 5:
      lib.lending_events_list();
      break;
    case 0:
      std::cout << "Exiting.\n";
      break;
    default:
      std::cout << "Invalid choice. Please try again.\n";
    }

  } while (choice != 0);

  return 0;
}
