#include "spn.cpp"  
#include <fstream>
#include <iostream>
#include <iomanip>   // For I/O formatting
#include <cstdint>   // For fixed-width integer types
#include <random>    // For random key generation
#include <vector>    

using namespace std;

// Function to generate and save a random 32-bit key to key.txt
void random_key_gen() {
    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<uint64_t> distribution(0, UINT64_MAX);  // 64-bit random value

    // Generate 80 bits: A 64-bit part + a 16-bit part
    uint64_t part1 = distribution(generator);  // First 64 bits
    uint16_t part2 = distribution(generator) & 0xFFFF;  // Last 16 bits (remaining bits for 80 total)

    // Combine into a single 80-bit key as a hex string
    stringstream keyStream;
    keyStream << hex << setw(16) << setfill('0') << part1  // 64 bits = 16 hex digits
              << setw(4) << setfill('0') << part2;  // 16 bits = 4 hex digits

    string key = keyStream.str();  // Final 20 hex-digit string (80 bits)
    
    // Write the key to a file
    ofstream keyFile("key.txt");
    if (!keyFile) {
        cerr << "Error: Unable to open key.txt for writing.\n";
    }

    keyFile << key << endl;
    keyFile.close();
}

// Function to read the 32-bit key from key.txt (generates if missing/empty)
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
uint16_t spn(uint16_t plaintext, string key = "00000000000000000000" , bool havekey = false ) {
    if (!havekey)
        key = get_key();
    
        // cout <<"Using key : "<< hex << key <<endl;
    
    vector<unsigned short> keys = generateKeys(key);  // Assuming generateKeys() exists
    return encrypt(plaintext, keys);  // Assuming encrypt() exists
}

void withkey(){
    while(true){
        string masterKey;

        cout << "Enter a master key (20 hex digits): ";
        cin >> masterKey;

        if (masterKey.size() != 20){
            cout << "keysize is wrong it should be 20 hex digits !!"<<endl;
            continue;
        }   

        // Save the key to key.txt
        ofstream keyFile("key.txt");
        if (!keyFile) {
            cerr << "Error: Unable to open key.txt for writing.\n";
            return;
        }
        keyFile << masterKey << endl;
        keyFile.close();
        cout << "Key saved to key.txt successfully.\n";
        return;
    }
}

void spn_encryption() {
    int x = 0;
    cout <<" with 1. key or 2. random key encryption choose : ";
    cin>>x;

    switch(x){
        case 1: withkey(); break;
        case 2: random_key_gen(); break;
        default: cout << "invalid choice !!"<<endl;return;
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

    return;
}

// int main(){
//     spn_encryption();

//     return 0;
// }
