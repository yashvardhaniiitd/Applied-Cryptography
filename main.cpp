#include "spn_encrypt.cpp"
#include "linear_attack.cpp"

#include <cstdint>
#include <random>
#include <iostream>
#include <set>
#include <string>

using namespace std;

uint16_t generate_random_plaintext()
{
    // Create a random device to seed the generator
    random_device rd;

    // Use Mersenne Twister engine for generating random numbers
    mt19937 generator(rd());

    // Define the range for a 16-bit unsigned integer (0 to 65535)
    uniform_int_distribution<uint16_t> distribution(0, UINT16_MAX);

    // Generate and return a random uint16_t value
    return distribution(generator);
}

char to_hex(unsigned char nibble)
{
    return nibble < 10 ? '0' + nibble : 'A' + (nibble - 10);
}

bool check(const string &key, const vector<vector<uint16_t>> &p_c_pair)
{
    for (int i = 0; i < 10; ++i)
    { // Check only the first 10 pairs for speed
        uint16_t plain = p_c_pair[i][0];
        uint16_t cipher = p_c_pair[i][1];
        uint16_t res = spn(plain, key, true);

        if (res != cipher)
            return false;
    }
    return true;
}

string exhaustive(pair<unsigned char, unsigned char> k5_2_4, vector<vector<uint16_t>> plain_cipher_pair)
{
    // Extract known nibbles (18th and 20th hex characters of the key)
    unsigned char k18 = k5_2_4.first;  // 18th hex character
    unsigned char k20 = k5_2_4.second; // 20th hex character

    // Loop over all 18 unknown hex characters (0x0 to 0xF)
    for (unsigned char k1 = 0x0; k1 <= 0xF; ++k1)
    {
        for (unsigned char k2 = 0x0; k2 <= 0xF; ++k2)
        {
            for (unsigned char k3 = 0x0; k3 <= 0xF; ++k3)
            {
                for (unsigned char k4 = 0x0; k4 <= 0xF; ++k4)
                {
                    for (unsigned char k5 = 0x0; k5 <= 0xF; ++k5)
                    {
                        for (unsigned char k6 = 0x0; k6 <= 0xF; ++k6)
                        {
                            for (unsigned char k7 = 0x0; k7 <= 0xF; ++k7)
                            {
                                for (unsigned char k8 = 0x0; k8 <= 0xF; ++k8)
                                {
                                    for (unsigned char k9 = 0x0; k9 <= 0xF; ++k9)
                                    {
                                        for (unsigned char k10 = 0x0; k10 <= 0xF; ++k10)
                                        {
                                            for (unsigned char k11 = 0x0; k11 <= 0xF; ++k11)
                                            {
                                                for (unsigned char k12 = 0x0; k12 <= 0xF; ++k12)
                                                {
                                                    for (unsigned char k13 = 0x0; k13 <= 0xF; ++k13)
                                                    {
                                                        for (unsigned char k14 = 0x0; k14 <= 0xF; ++k14)
                                                        {
                                                            for (unsigned char k15 = 0x0; k15 <= 0xF; ++k15)
                                                            {
                                                                for (unsigned char k16 = 0x0; k16 <= 0xF; ++k16)
                                                                {
                                                                    for (unsigned char k17 = 0x0; k17 <= 0xF; ++k17)
                                                                    {
                                                                        for (unsigned char k19 = 0x0; k19 <= 0xF; ++k19)
                                                                        {

                                                                            // Construct the key as a hex string
                                                                            stringstream keyStream;
                                                                            keyStream << to_hex(k1) << to_hex(k2)
                                                                                      << to_hex(k3) << to_hex(k4)
                                                                                      << to_hex(k5) << to_hex(k6)
                                                                                      << to_hex(k7) << to_hex(k8)
                                                                                      << to_hex(k9) << to_hex(k10)
                                                                                      << to_hex(k11) << to_hex(k12)
                                                                                      << to_hex(k13) << to_hex(k14)
                                                                                      << to_hex(k15) << to_hex(k16)
                                                                                      << to_hex(k17) << to_hex(k18)  // Fixed 18th character
                                                                                      << to_hex(k19) << to_hex(k20); // Fixed 20th character

                                                                            string key = keyStream.str();

                                                                            cout << "checking key : " << key << endl;
                                                                            // Check if the resulting ciphertext matches the expected ciphertext
                                                                            if (check(key, plain_cipher_pair))
                                                                            {
                                                                                // Key found that produces the correct ciphertext
                                                                                cout << "Found matching key: " << key << endl;
                                                                                return key; // Return the found key as string
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // If no key matches, return an empty string
    return "";
}

int main()
{
    spn_encryption();

    cout << "Doing Linear attack on it !!!" << endl;
    cout << "-------------------------------" << endl;
    cout << "generating 10000 random plaintexts" << endl;

    set<uint16_t> plaintexts;

    while (plaintexts.size() < 10000)
    {
        plaintexts.insert(generate_random_plaintext());
    }

    cout << "saving the pairs of plaintext and ciphertext in a csv file" << endl;

    ofstream dataFile("data.csv");
    if (!dataFile)
    {
        cerr << "Error: Unable to open data.csv for writing.\n";
        return 1;
    }

    for (auto plaintext : plaintexts)
    {
        uint16_t ciphertext = spn(plaintext);
        dataFile << hex << setw(4) << setfill('0') << plaintext << "," << ciphertext << endl;
    }

    dataFile.close();

    cout << "successfully made random pairs of plain text and cipher text " << endl;

    string fileName = "data.csv";

    vector<vector<uint16_t>> plain_cipher_pair = readCSV(fileName);
    pair<unsigned char, unsigned char> k5_2_4 = linear_attack(plain_cipher_pair);

    int x;
    cout << "want to recover key (0/1) ? ";
    cin >> x;

    if (x == 1)
    {
        cout << " \n doing exhaustive attack on other bits of key" << endl;
        string masterkey = exhaustive(k5_2_4, plain_cipher_pair);
    }
    return 0;
}