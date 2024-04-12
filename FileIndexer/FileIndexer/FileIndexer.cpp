#include <iostream>
#include <string>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

class FileIndexer
{
private:
  struct Node
  {
    std::string fileName;
    std::string filePath;
    Node* left;
    Node* right;

    Node(const std::string& name, const std::string& path)
      : fileName(name), filePath(path), left(nullptr), right(nullptr) {}
  };

  Node* root;

  void insert_node(const std::string& fileName, const std::string& filePath);
  void index_directory_helper(const fs::path& dirPath);
  void delete_subtree(Node* root);
  void traverse(Node* root);

public:
  FileIndexer() : root(nullptr) {}
  void index_directory(const std::string& directoryPath);
  void print_files();
  std::string search_file_location(const std::string filename);
  ~FileIndexer();
};

// Insert node to tree
void FileIndexer::insert_node(const std::string& fileName, const std::string& filePath)
{
  // If the tree is empty, insert node here
  if (root == nullptr)
  {
    root = new Node(fileName, filePath);
    return;
  }

  // If not empty, then go down the tree
  Node* current = root;
  while (true)
  {
    if (fileName < current->fileName)
    {
      if (current->left == nullptr)
      {
        current->left = new Node(fileName, filePath);
        return;
      }
      current = current->left;
    }
    else
    {
      if (current->right == nullptr)
      {
        current->right = new Node(fileName, filePath);
        return;
      }
      current = current->right;
    }
  }
}

// Index the specified directory
void FileIndexer::index_directory_helper(const fs::path& dirPath)
{
  //if it's not a directory, return
  if (!fs::exists(dirPath) || !fs::is_directory(dirPath))
    return;

  //get iterator of files within directory
  for (const auto& entry : fs::directory_iterator(dirPath))
  {
    if (fs::is_regular_file(entry))
    {
      std::string fileName = entry.path().filename().string();
      std::string filePath = entry.path().string();
      insert_node(fileName, filePath);
    }
    else if (fs::is_directory(entry))
    {
      //recursively index sub-directory
      index_directory_helper(entry);
    }
  }
}

// Deallocate memory recursively
void FileIndexer::delete_subtree(Node* root)
{
  if (root != nullptr) 
  {
    delete_subtree(root->left);
    delete_subtree(root->right);
    delete root;
  }
}

void FileIndexer::traverse(Node* root)
{
  if (root != nullptr)
  {
    traverse(root->left);
    std::cout << root->fileName << ": " << root->filePath << std::endl;
    traverse(root->right);
  }
}


/////////////////////////////////////
//public functions
/////////////////////////////////////

void FileIndexer::index_directory(const std::string& directoryPath)
{
  root = nullptr;
  index_directory_helper(directoryPath);
}

void FileIndexer::print_files()
{
  std::cout << "Indexed files:" << std::endl;
  traverse(root);
}

// Search for a file in the BST
std::string FileIndexer::search_file_location(const std::string filename)
{
  // Traverse the tree until a match is found or the tree is exhausted
  while (root != nullptr)
  {
    if (filename == root->fileName)
    {
      return root->filePath; // File found
    }
    else if (filename < root->fileName)
    {
      root = root->left; // Search in the left subtree
    }
    else
    {
      root = root->right; // Search in the right subtree
    }
  }
  return ""; // File not found
}

FileIndexer::~FileIndexer()
{
  // Call deleteSubtree to deallocate memory for the binary search tree nodes
  delete_subtree(root);
}

int main()
{
  std::string path;
  std::cout << "Path to index recursively: ";
  std::getline(std::cin, path);

  FileIndexer indexer;
  indexer.index_directory(path);
  indexer.print_files();

  std::string filenameToSearch;
  std::cout << "Let's search for a file's location. Give the file name: ";
  std::getline(std::cin, filenameToSearch);

  std::string location = indexer.search_file_location(filenameToSearch);
  if (location != "")
  {
    std::cout << "File " << filenameToSearch << " found. Location: " << location << std::endl;
  }
  else
  {
    std::cout << "File " << filenameToSearch << " not found. " << std::endl;
  }
  return 0;
}
