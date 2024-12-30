#include <iostream>
#include <string>
#include <algorithm> // for using transform
#include <cctype>    // for using toupper
using namespace std;

int main()
{
  // initializing variables
  string key, plaintext;

  // input key
  cout << "Key - ";
  cin >> key;

  // input plaintext
  cout << "Plaintext - ";
  cin.ignore();
  getline(cin, plaintext, '\n');

  // transform both plaintext and key to capital letters so that they are in same space of 26 capital letters
  transform(plaintext.begin(), plaintext.end(), plaintext.begin(), ::toupper);
  transform(key.begin(), key.end(), key.begin(), ::toupper);

  // getting size of key and plaintext for iteration
  int n = key.length();
  int m = plaintext.length();

  // making a empty string - cipher string to store cipher text
  string cipher = "";

  // j iterator to iterate through key
  int j = 0;

  // iterating through plaintext
  for (int i = 0; i < m; i++)
  {
    // checking if the character is an alphabet and ignoring special characters, spaces, etc
    if (isalpha(plaintext[i]))
    {
      // applying rules for vigenere cipher and encrypting the character
      char c = (plaintext[i] + key[j]) % 26;

      // So that ASCII value starts from 'A' till 'Z' instead of from 1 to 26
      c += 'A';

      // adding encrypted character to cipher
      cipher.push_back(c);

      // so that it is within bounds of key length
      j = (j + 1) % n;
    }
  }

  // output cipher text
  cout << "Ciphertext - " << cipher << endl;
  return 0;
}