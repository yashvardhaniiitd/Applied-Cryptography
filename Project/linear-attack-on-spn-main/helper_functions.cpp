#include <iostream>
#include <string>
#include <array>

using namespace std;

// Define the S-box as an array (16 elements as per the image)
const unsigned char s_box[16] = { 
    0xE, 0x4, 0xD, 0x1, 0x2, 0xF, 0xB, 0x8, 0x3, 0xA, 0x6, 0xC, 0x5, 0x9, 0x0, 0x7
};

// inverse S-box
array<unsigned char, 16> generate_inverse_s_box(const unsigned char s_box[16]) {
    array<unsigned char, 16> inverse_s_box;
    for (int i = 0; i < 16; ++i) {
        inverse_s_box[s_box[i]] = i;
    }
    return inverse_s_box;
}

array<unsigned char, 16> inverse_s_box = generate_inverse_s_box(s_box);

// Define the p_box table (16-bit input and output positions)
const int p_box[16] = { 
    1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15, 4, 8, 12, 16
};

//  inverse p_box
array<int, 16> generate_inverse_p_box(const int p_box[16]) {
    array<int, 16> inverse_p_box;
    for (int i = 0; i < 16; ++i) {
        inverse_p_box[p_box[i] - 1] = i + 1;
    }
    return inverse_p_box;
}

array<int, 16> inverse_p_box = generate_inverse_p_box(p_box);


// Function to apply S-box on a 4-bit hex value
unsigned char apply_s_box(unsigned char input) {
    // Masking with 0xF to ensure input is within 4-bit range
    return s_box[input & 0xF];
}

// Function to apply the inverse S-box
unsigned char apply_inverse_s_box(unsigned char input) {
    return inverse_s_box[input & 0xF];
}

// Function to apply p_box on a 16-bit input
unsigned short apply_p_box(unsigned short input) {
    unsigned short output = 0;

    for (int i = 0; i < 16; i++) {
        // Get the bit value from the input at position (16-i), then place it at the permuted position
        unsigned short bit = (input >> (16 - p_box[i])) & 1;
        output |= (bit << (16 - i - 1));
    }

    return output;
}

// Function to apply the inverse p_box on a 16-bit input
unsigned short apply_inverse_p_box(unsigned short input) {
    unsigned short output = 0;
    for (int i = 0; i < 16; ++i) {
        unsigned short bit = (input >> (16 - i - 1)) & 1;
        output |= (bit << (16 - inverse_p_box[i]));
    }
    return output;
}
