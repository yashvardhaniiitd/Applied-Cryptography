#include "spn.cpp"  
#include <fstream>
#include <iostream>
#include <iomanip>   // For I/O formatting
#include <cstdint>   // For fixed-width integer types
#include <random>    // For random key generation
#include <vector>    

using namespace std;

// Function to generate and save a random 80-bit key to key.txt with the first 12 hex digits set to 'F'
void random_key_gen() {
    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<uint32_t> distribution(0, UINT32_MAX);  // 32-bit random value

    // Fixed first 12 hex digits as 'FFFFFFFFFFFF' (48 bits of F)
    string fixedPart = "FFFFFFFFFFFF";

    // Generate the remaining 32 bits (8 hex digits)
    uint32_t part2 = distribution(generator);  // Random 32 bits

    // Combine into a single 80-bit key as a hex string
    stringstream keyStream;
    keyStream << fixedPart 
              << hex << setw(8) << setfill('0') << part2;  // Remaining 32 bits = 8 hex digits

    string key = keyStream.str();  // Final 20 hex-digit string (80 bits)
    
    // Write the key to a file
    ofstream keyFile("key.txt");
    if (!keyFile) {
        cerr << "Error: Unable to open key.txt for writing.\n";
    }

    keyFile << key << endl;
    keyFile.close();
}

// Function to read the 80-bit key from key.txt (generates if missing/empty)
string get_key() {
    string key;
    ifstream keyFile("key.txt");

    if (!keyFile || keyFile.peek() == ifstream::traits_type::eof()) {
        cout << "Key not found or empty, generating a new random key...\n";
        random_key_gen();
        keyFile.open("key.txt");  // Reopen the file after generating the key
    }

    keyFile >> key;
    keyFile.close();
    return key;
}

// Function to perform SPN encryption on the plaintext
uint16_t spn(uint16_t plaintext, string key = "FFFFFFFFFFFF00000000", bool havekey = false) {
    if (!havekey)
        key = get_key();
    
    vector<unsigned short> keys = generateKeys(key);  // Assuming generateKeys() exists
    return encrypt(plaintext, keys);  // Assuming encrypt() exists
}

void withkey(){
    while(true){
        string masterKey = "FFFFFFFFFFFF";

        string rkey;
        cout << "Enter last 2 round key (8 hex digits): ";
        cin >> rkey;


        if (rkey.size() != 8){
            cout << "Key size is wrong; it should be 8 hex digits!!" << endl;
            continue;
        }

        // Save the key to key.txt
        ofstream keyFile("key.txt");
        if (!keyFile) {
            cerr << "Error: Unable to open key.txt for writing.\n";
            return;
        }
        keyFile << masterKey <<rkey<< endl;
        keyFile.close();
        cout << "Key saved to key.txt successfully.\n";
        return;
    }
}

void spn_encryption() {
    int x = 0;
    cout << "Choose encryption method: 1. given key or 2. Random key: ";
    cin >> x;

    switch(x){
        case 1: withkey(); break;
        case 2: random_key_gen(); break;
        default: cout << "Invalid choice!" << endl; return;
    }

    uint16_t plaintext;
    cout << "Plaintext (4 hex digits): ";
    cin >> hex >> plaintext;

    // Perform SPN encryption and display/save the result
    uint16_t cipher = spn(plaintext);
    cout << "Encrypted plaintext: 0x" << hex << setw(4) << setfill('0') << cipher << endl;

    ofstream cipherFile("cipher.txt");
    if (!cipherFile) {
        cerr << "Error: Unable to open cipher.txt for writing.\n";
        return;
    }
    cipherFile << hex << setw(4) << setfill('0') << cipher << endl;
    cipherFile.close();
    cout << "Encrypted message saved to cipher.txt successfully.\n";
}

// int main(){
//     spn_encryption();
//     return 0;
// }
