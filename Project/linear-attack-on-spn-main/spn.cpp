#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <stdexcept>

// S-box for substitution
int sbox[16] = {0xE, 0x4, 0xD, 0x1,
                0x2, 0xF, 0xB, 0x8,
                0x3, 0xA, 0x6, 0xC,
                0x5, 0x9, 0x0, 0x7};

// Permutation table (1-based index)
const int permutation[16] = {1, 5, 9, 13,
                              2, 6, 10, 14,
                              3, 7, 11, 15,
                              4, 8, 12, 16};

// Function to apply S-box substitution to a 16-bit state
unsigned short applySbox(unsigned short state) {
    unsigned short substitutedState = 0;

    // Apply substitution nibble by nibble (4 bits at a time)
    for (int i = 3; i >= 0; i--) {
        unsigned short nibble = (state >> (i * 4)) & 0xF;        
        unsigned short substitutedNibble = sbox[nibble];
        substitutedState |= (substitutedNibble << (i * 4));
    }

    return substitutedState;
}

// Function to apply permutation to a 16-bit state
unsigned short applyPermutation(unsigned short state) {
    unsigned short permutedState = 0;

    for (int i = 0; i < 16; ++i) {
        int bit = (state >> (15 - i)) & 1;
        permutedState |= (bit << (15 - (permutation[i] - 1)));
    }

    return permutedState;
}

// Function to mix key into state using XOR operation
unsigned short mixKey(unsigned short state, unsigned short key) {
    return state ^ key; // XOR operation for key mixing
}

// Function to generate keys from an given master key (80 bits)
std::vector<unsigned short> generateKeys(const std::string& masterKey) {
    std::vector<unsigned short> keys;

    // Ensure the master key is exactly 20 hex digits long (80 bits)
    if (masterKey.length() != 20) {
        throw std::invalid_argument("Master key must be exactly 20 hex digits long.");
    }

    // Generate five subkeys from the master key
    for (int i = 0; i < 5; ++i) {
        std::string subKeyHex = masterKey.substr(i * 4, 4); // Get each subkey of length four
        
        // Convert hex string to unsigned short
        unsigned short subKey = static_cast<unsigned short>(std::stoi(subKeyHex, nullptr, 16));
        keys.push_back(subKey);
    }
    
    return keys;
}

// Main encryption function that integrates all parts
unsigned short encrypt(unsigned short plaintext, const std::vector<unsigned short>& keys) {
    unsigned short state = plaintext;

    // Perform encryption over multiple rounds
    for (size_t round = 0; round < keys.size() - 2; ++round) { // Last key is applied after final round
        // std::cout << "Round: " << round << std::endl;
        
        // Key mixing
        state = mixKey(state, keys[round]);

        // Substitution
        state = applySbox(state);

        // Permutation
        state = applyPermutation(state);
    }

    // Last round 
    state = mixKey(state, keys[keys.size() - 2]);
    state = applySbox(state);

    // Final key mixing after last round without permutation
    state = mixKey(state, keys[keys.size() - 1]);
    
    return state;
}

// int main() {
//     unsigned short plaintext = 0x0000; // Example plaintext input

//     try {
//         // Define a master key as a string of hexadecimal digits (80 bits /20 hex digits)
//         std::string masterKey;
        
//         std::cout << "Enter a master key (20 hex digits): ";
//         std::cin >> masterKey;

//         // Generate subkeys from the master key
//         std::vector<unsigned short> keys = generateKeys(masterKey);

//         // Perform encryption
//         unsigned short ciphertext = encrypt(plaintext, keys);

//         // Output results
//         std::cout << "Original plaintext: 0x" << std::hex << plaintext << std::endl;
//         std::cout << "Ciphertext:       0x" << std::hex << ciphertext << std::endl;
        
//     } catch(const std::invalid_argument& e) {
//         std::cerr << "Error: " << e.what() << '\n';
//     }

//     return 0;
// }


