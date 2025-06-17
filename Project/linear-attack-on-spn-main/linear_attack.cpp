#include "helper_functions.cpp"
#include "read_pairs.cpp"

#include <set>
#include <map>
#include <string>
#include <iomanip>
#include <cstdint>

using namespace std;

void print_bias(map<pair<unsigned char, unsigned char>, int> counts, int n){
    for (unsigned char L1 = 0x0; L1 <= 0xF; ++L1) {
        for (unsigned char L2 = 0x0; L2 <= 0xF; ++L2) {
            float bias = abs((float)counts[{L1, L2}] / n - 0.5);
            cout<<hex<<(int)L1<<" "<<(int)L2<<"\t"<<dec<<setprecision(4)<<bias<<endl;
        }
    }
}

map<pair<unsigned char, unsigned char>, int> get_sub_keys(){
    map<pair<unsigned char, unsigned char>, int> count_sub_key_k5_2_4;
    
    // Iterate over possible values of subkey bytes (L1 and L2 go from 0x0 to 0xF)
    for (unsigned char L1 = 0x0; L1 <= 0xF; ++L1) {
        for (unsigned char L2 = 0x0; L2 <= 0xF; ++L2) {
            
            // cout<<hex<<(int)L1<<" "<<(int)L2<<" "<<0<<endl;
            
            // Initialize the count for each subkey pair to 0
            count_sub_key_k5_2_4[{L1, L2}] = 0;
        }
    }

    return count_sub_key_k5_2_4;
}

string uint16ToHexString(uint16_t value) {
    stringstream stream;
    // Set the formatting to hex and fill with zeros to make it 4 characters long
    stream << hex << setw(4) << setfill('0') << value;
    return stream.str();
}

pair<unsigned char, unsigned char> linear_attack(vector<vector<uint16_t>> p_c_pairs) {
    map<pair<unsigned char, unsigned char>, int> count_sub_key_k5_2_4 = get_sub_keys();
    int n = p_c_pairs.size();

    // Iterate over each pair of plaintext-ciphertext
    for (int i = 0; i < n; i++) {
        string plaintext = uint16ToHexString(p_c_pairs[i][0]);
        string ciphertext = uint16ToHexString(p_c_pairs[i][1]);

        // Check lengths
        if (plaintext.length() < 4 || ciphertext.length() < 4) {
            cerr << "Error: Hex string is too short. Plaintext: " << plaintext << ", Ciphertext: " << ciphertext << endl;
            continue; // Skip this iteration
        }

        // Extract bits from plaintext and ciphertext
        unsigned char y2 = stoi(ciphertext.substr(1, 1), nullptr, 16) & 0xF; // Get the second hex digit
        unsigned char y4 = stoi(ciphertext.substr(3, 1), nullptr, 16) & 0xF; // Get the fourth hex digit
        unsigned char x2 = stoi(plaintext.substr(1, 1), nullptr, 16) & 0xF;  // Get the second hex digit of plaintext

        for (unsigned char L1 = 0x0; L1 <= 0xF; ++L1) {
            for (unsigned char L2 = 0x0; L2 <= 0xF; ++L2) {
                // Compute v values
                unsigned char v2_4 = L1 ^ y2; // Linear approximation for y2
                unsigned char v4_4 = L2 ^ y4; // Linear approximation for y4

                // Applying inverse S-boxes
                unsigned char u2_4 = apply_inverse_s_box(v2_4);
                unsigned char u4_4 = apply_inverse_s_box(v4_4);

                // Calculate bits for linear approximation
                unsigned char x5 = (x2 >> 1) & 1; // Bit 1
                unsigned char x7 = (x2 >> 3) & 1; // Bit 3
                unsigned char x8 = (x2 >> 4) & 1; // Bit 4

                // Calculate the linear approximation output
                unsigned char z = (x5 ^ x7 ^ x8 ^
                                  ((u2_4 >> 2) & 1) ^
                                  ((u2_4 >> 4) & 1) ^
                                  ((u4_4 >> 2) & 1) ^
                                  ((u4_4 >> 4) & 1));

                // Update counts based on the linear approximation result
                if (z == 0) {
                    count_sub_key_k5_2_4[{L1, L2}]++;
                }
            }
        }
    }

    // Print bias and find the maximum key
    print_bias(count_sub_key_k5_2_4, n);

    int maxi = -1;
    pair<unsigned char, unsigned char> maxKey;

    for (unsigned char L1 = 0x0; L1 <= 0xF; ++L1) {
        for (unsigned char L2 = 0x0; L2 <= 0xF; ++L2) {
            int count = abs(count_sub_key_k5_2_4[{L1, L2}] - n / 2);
            // Update max key if current count is greater
            if (count > maxi) {
                maxi = count;
                maxKey = {L1, L2};
            }
        }
    }

    cout << "Max key: " << hex << (int)maxKey.first << " " << (int)maxKey.second << endl;

    return maxKey;
}


// int main(){
//     string fileName = "data.csv";
//     cout<<"File name : "<<fileName<<endl;
//     vector<vector<uint16_t>> plain_cipher_pair = readCSV(fileName);
//     cout<<"collected pairs "<<endl;
//     pair<unsigned char, unsigned char> k5_2_4 = linear_attack(plain_cipher_pair);

//     // string key = full_key(k5_2_4, plain_cipher_pair[0][0], plain_cipher_pair[0][1]);
// }