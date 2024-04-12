#include <iostream>
#include <string>
#include <ctime>
using namespace std;

class Product
{
public:
  int id;
  string name;
  float price;
  int quantity;
};

class InventoryNode
{
public:
  Product product;
  InventoryNode* left;
  InventoryNode* right;
  int height;
};

class Inventory
{
private:
  InventoryNode *root;

  int get_height(InventoryNode* node);
  int get_balance(InventoryNode* node);
  InventoryNode* new_node(Product product);
  InventoryNode* rotate_right(InventoryNode* y);
  InventoryNode* rotate_left(InventoryNode* x);
  InventoryNode* insert_product(InventoryNode* node, Product product);
  void traverse_tree(InventoryNode* node);
  InventoryNode* search_product(InventoryNode* node, int id);

public:
  Inventory() { root = nullptr; }
  void insert_product(Product product);
  void traverse_tree();
  InventoryNode* search_product(int id);
};

int Inventory::get_height(InventoryNode* node)
{
  if (node == nullptr)
    return 0;
  return node->height;
}

int Inventory::get_balance(InventoryNode* node)
{
  if (node == nullptr)
    return 0;
  return get_height(node->left) - get_height(node->right);
}

InventoryNode* Inventory::new_node(Product product)
{
  InventoryNode* node = new InventoryNode;
  if (node != nullptr)
  {
    node->product = product;
    node->left = nullptr;
    node->right = nullptr;
    node->height = 1;
    return node;
  }
  else
  {
    cout << "Error allocating memory. Exiting...";
    exit(1);
  }
}

InventoryNode* Inventory::rotate_right(InventoryNode* y)
{
  cout << "right rotate" << endl;
  InventoryNode* x = y->left;
  InventoryNode* T2 = x->right;

  x->right = y;
  y->left = T2;

  y->height = max(get_height(y->left), get_height(y->right)) + 1;
  x->height = max(get_height(x->left), get_height(x->right)) + 1;

  return x;
}

InventoryNode* Inventory::rotate_left(InventoryNode* x)
{
  cout << "left rotate" << endl;
  InventoryNode* y = x->right;
  InventoryNode* T2 = y->left;

  y->left = x;
  x->right = T2;

  x->height = max(get_height(x->left), get_height(x->right)) + 1;
  y->height = max(get_height(y->left), get_height(y->right)) + 1;

  return y;
}

InventoryNode* Inventory::insert_product(InventoryNode* node, Product product)
{
  if (node == nullptr)
    return new_node(product);

  if (product.id < node->product.id)
    node->left = insert_product(node->left, product);
  else if (product.id > node->product.id)
    node->right = insert_product(node->right, product);
  else
    return node;

  node->height = 1 + max(get_height(node->left), get_height(node->right));

  int balance = get_balance(node);

  if (balance > 1 && product.id < node->left->product.id)
    return rotate_right(node);

  if (balance < -1 && product.id > node->right->product.id)
    return rotate_left(node);

  if (balance > 1 && product.id > node->left->product.id)
  {
    node->left = rotate_left(node->left);
    return rotate_right(node);
  }

  if (balance < -1 && product.id < node->right->product.id)
  {
    node->right = rotate_right(node->right);
    return rotate_left(node);
  }

  return node;
}

void Inventory::traverse_tree(InventoryNode* node)
{
  if (node != nullptr)
  {
    traverse_tree(node->left);
    cout << "ID: " << node->product.id
      << ", Name: " << node->product.name
      << ", Price : " << node->product.price
      << ", Quantity :" << node->product.quantity << endl;
    traverse_tree(node->right);
  }
}

InventoryNode* Inventory::search_product(InventoryNode* node, int id)
{
  if (node == nullptr || node->product.id == id)
  {
    if (node == nullptr)
      cout << "Product not found.\n";
    else
      cout << "Found product: ID: " << node->product.id
      << ", Name: " << node->product.name
      << ", Price : " << node->product.price
      << ", Quantity :" << node->product.quantity << endl;

    return node;
  }

  cout << "Visited product ID: " << node->product.id << endl;

  if (id < node->product.id)
    return search_product(node->left, id);
  else
    return search_product(node->right, id);
}

void Inventory::insert_product(Product product)
{
  root = insert_product(root, product);
}

void Inventory::traverse_tree()
{
  traverse_tree(root);
}

InventoryNode* Inventory::search_product(int id)
{
  return search_product(root, id);
}

int main()
{
  Inventory inv;
  Product products[100];

  // Adding 100 random products
  srand((unsigned int)time(nullptr));
  for (int i = 0; i < 100; i++)
  {
    products[i].id = i + 1;
    products[i].name = "Product " + to_string(products[i].id);
    products[i].price = (float)(rand() % 1000) / 10.0;
    products[i].quantity = rand() % 100 + 1;
  }

  // Shuffle the array of products
  srand((unsigned int)time(nullptr));
  for (int i = 99; i > 0; i--)
  {
    int j = rand() % (i + 1);
    Product temp = products[i];
    products[i] = products[j];
    products[j] = temp;
  }

  for (int i = 0; i < 100; i++)
  {
    inv.insert_product(products[i]);
  }

  // Print inventory
  cout << "Inventory:" << endl;
  inv.traverse_tree();

  // Search for a product
  int productIdToSearch = 35;
  InventoryNode* foundProduct = inv.search_product(productIdToSearch);
  if (foundProduct != nullptr)
  {
    cout << "Product found:" << endl;
    cout << "ID: " << foundProduct->product.id 
        << ", Name: " << foundProduct->product.name
        << ", Price : " << foundProduct->product.price
        << ", Quantity :" << foundProduct->product.quantity << endl;
  }
  else
  {
    cout << "Product with ID " << productIdToSearch << " not found." << endl;
  }
  return 0;
}
