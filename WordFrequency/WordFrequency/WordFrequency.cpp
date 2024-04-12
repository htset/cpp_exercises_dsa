#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <algorithm>
#include <cctype>

using namespace std;

string cleanWord(const string& word);

int main()
{
  map<string, int> wordFrequency;

  // Read text from file
  ifstream inputFile("input.txt");
  if (!inputFile)
  {
    cerr << "Error opening file." << endl;
    return 1;
  }

  // Process each word in the input
  string word;
  while (inputFile >> word)
  {
    word = cleanWord(word);
    if (!word.empty())
    {
      wordFrequency[word]++;
    }
  }

  inputFile.close();

  // Display word frequencies
  cout << "Word Frequencies:" << endl;
  for (const auto& pair : wordFrequency)
  {
    cout << pair.first << ": " << pair.second << endl;
  }

  return 0;
}

// Clean and normalize a word
string cleanWord(const string& word)
{
  string cleanedWord;
  for (char c : word)
  {
    if (isalpha(c))
    {
      cleanedWord += tolower(c);
    }
  }
  return cleanedWord;
}