#include "fixedkeyspn_encrypt.cpp"
#include "linear_attack.cpp"

#include <cstdint>
#include <random>
#include <iostream>
#include <set>
#include <string>
#include <sstream>
#include <fstream>
#include <omp.h>  // For parallelization

using namespace std;

uint16_t generate_random_plaintext() {
    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<uint16_t> distribution(0, UINT16_MAX);
    return distribution(generator);
}

char to_hex(unsigned char nibble) {
    return nibble < 10 ? '0' + nibble : 'A' + (nibble - 10);
}

bool check(const string &key, const vector<vector<uint16_t>> &p_c_pair) {
    for (int i = 0; i < 10; ++i) {  // Check only the first 10 pairs for speed
        uint16_t plain = p_c_pair[i][0];
        uint16_t cipher = p_c_pair[i][1];
        uint16_t res = spn(plain, key, true);

        if (res != cipher) return false;
    }
    return true;
}

string exhaustive(const pair<unsigned char, unsigned char> &k5_2_4, const vector<vector<uint16_t>> &plain_cipher_pair) {
    unsigned char k18 = k5_2_4.first;  
    unsigned char k20 = k5_2_4.second; 

    unsigned char k1 = 0xF;
    unsigned char k2 = 0xF;
    unsigned char k3 = 0xF;
    unsigned char k4 = 0xF;
    unsigned char k5 = 0xF;
    unsigned char k6 = 0xF;
    unsigned char k7 = 0xF;
    unsigned char k8 = 0xF;
    unsigned char k9 = 0xF;
    unsigned char k10 = 0xF;
    unsigned char k11 = 0xF;
    unsigned char k12 = 0xF;

    string found_key;
    bool key_found = false;

    cout << "searching ... "<< endl;

    #pragma omp parallel for collapse(18) shared(found_key, key_found) // Parallelize loops
    for (unsigned char k13 = 0x0; k13 <= 0xF; ++k13) {
        if (key_found) continue;
        for (unsigned char k14 = 0x0; k14 <= 0xF; ++k14) {
            if (key_found) continue;
            for (unsigned char k15 = 0x0; k15 <= 0xF; ++k15) {
                if (key_found) continue;
                for (unsigned char k16 = 0x0; k16 <= 0xF; ++k16) {
                    if (key_found) continue;
                    for (unsigned char k17 = 0x0; k17 <= 0xF; ++k17) {
                        if (key_found) continue;
                        for (unsigned char k19 = 0x0; k19 <= 0xF; ++k19) {
                            if (key_found) continue;

                            // Construct the key as a hex string
                            stringstream keyStream;
                            keyStream << to_hex(k1) << to_hex(k2)<< to_hex(k3) << to_hex(k4)<< to_hex(k5) << to_hex(k6)                                                                                      << to_hex(k7) << to_hex(k8)
<< to_hex(k9) << to_hex(k10)
<< to_hex(k11) << to_hex(k12)
<< to_hex(k13) << to_hex(k14)
<< to_hex(k15) << to_hex(k16)
<< to_hex(k17) << to_hex(k18)  // Fixed linear 18th character
<< to_hex(k19) << to_hex(k20); // Fixed linear 20th character

                            string key = keyStream.str();
                            // cout << "checking key : "<< key <<endl;

                            if (check(key, plain_cipher_pair)) {
                            #pragma omp critical
                            {
                                if (!key_found) {
                                found_key = key;
                                key_found = true;
                                cout << "Found matching key: " << key << endl;
                                }
                            }
                            }
                        }
                    }
                }
            }
        }
    }

    return found_key.empty() ? "" : found_key;
}

int main() {
    spn_encryption();

    cout << "Doing Linear attack on it !!!" << endl;
    cout << "-------------------------------" << endl;
    cout << "Generating 10,000 random plaintexts" << endl;

    set<uint16_t> plaintexts;
    while (plaintexts.size() < 10000) {
        plaintexts.insert(generate_random_plaintext());
    }

    cout << "Saving plaintext-ciphertext pairs in a CSV file" << endl;

    ofstream dataFile("data.csv");
    if (!dataFile) {
        cerr << "Error: Unable to open data.csv for writing.\n";
        return 1;
    }

    for (auto plaintext : plaintexts) {
        uint16_t ciphertext = spn(plaintext);
        dataFile << hex << setw(4) << setfill('0') << plaintext << "," << ciphertext << endl;
    }

    dataFile.close();
    cout << "Successfully created random plaintext-ciphertext pairs." << endl;

    string fileName = "data.csv";
    vector<vector<uint16_t>> plain_cipher_pair = readCSV(fileName);
    pair<unsigned char, unsigned char> k5_2_4 = linear_attack(plain_cipher_pair);

    int x; 

    cout << "want to recover key (0/1) ? "; cin>> x;

    if (x == 1){

    cout << "\nDoing exhaustive attack on other bits of the key..." << endl;
    string masterkey = exhaustive(k5_2_4, plain_cipher_pair);

    // if (!masterkey.empty()) {
    //     cout << "Master key found: " << masterkey << endl;
    // } else {
    //     cout << "Failed to find the key." << endl;
    // }
    
    }
    return 0;
}
