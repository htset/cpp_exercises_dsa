#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

#define IMAGE_SIZE 784    // Size of each image (28x28)
#define META_DATA_SIZE 15 // Size of meta data at the beginning of the file

using namespace std;

// Structure to represent an image
class Image
{
public:
  unsigned char* data;
  int id;

  Image(unsigned char* data, int id);
  void print();
  double euclideanDistance(Image* img);
};

Image::Image(unsigned char* data, int id)
{
  this->id = id;
  this->data = new unsigned char[IMAGE_SIZE];
  memcpy(this->data, data, IMAGE_SIZE);
}

void Image::print()
{
  for (int i = 0; i < IMAGE_SIZE; i++)
  {
    if (data[i] == 0)
      cout << " ";
    else
      cout << "*";
    if (i % 28 == 0)
      cout << endl;
  }
}

// Calculate Euclidean distance between two images
double Image::euclideanDistance(Image* img)
{
  double distance = 0.0;
  for (int i = 0; i < IMAGE_SIZE; i++)
  {
    distance += sqrt(pow((this->data[i] - img->data[i]), 2));
  }
  return sqrt(distance);
}

int main()
{
  vector<Image*> images;
  ifstream ifs("input.dat", ios::binary);
  unsigned char pixels[IMAGE_SIZE] = { 0 };

  if (!ifs.is_open())
  {
    cout << "Error opening file." << endl;
    exit(EXIT_FAILURE);
  }

  // Skip the meta data at the beginning of the file
  ifs.seekg(META_DATA_SIZE, ios::beg);
 
  int count = 0;
  // Read data from the file and insert images into the vector
  while (ifs.read((char*)pixels, IMAGE_SIZE))
  {
    images.push_back(new Image(pixels, count++));
  }

  cout << "Total images: " << count << endl;

  ifs.close();

  // Example: Find the closest image to a randomly selected image
  // Seed the random number generator
  srand(time(nullptr));

  // Generate a random index within the range of the list length
  int randomIndex = rand() % count;
  cout << "Random index: " << randomIndex << endl;

  Image* randomImage = images.at(randomIndex);
  randomImage->print();

  Image* closestImage = nullptr;
  double minDistance = INFINITY;
  int index = 0, minIndex = 0;

  for (int i = 0; i < images.size(); i++)
  {
    double distance = randomImage->euclideanDistance(images.at(i));
    if (distance != 0 && distance < minDistance)
    {
      minDistance = distance;
      minIndex = i;
      closestImage = images.at(i);
    }
  }

  // Output the label of the closest image
  cout << endl << "Closest image (distance=" << minDistance 
        << ", index = " << minIndex << ")" << endl;

  // Print closest image
  closestImage->print();

  return 0;
}
