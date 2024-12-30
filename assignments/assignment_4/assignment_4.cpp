#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <map>

using namespace std;

map<char, int> hex_to_int;
// map<int, char> int_to_hex;


// function to convert decimal to hexadecimal
string int_to_hex_fun(int value, int set)
{
    stringstream ss;
    ss << hex << setfill('0') << setw(set) << value; // Set to two digits
    return ss.str();
}

// class for aes encryption
class AES
{
    // vectors to hold round keys, states, and intermediate results
    vector<vector<vector<string>>> round_keys;
    vector<vector<vector<string>>> states;
    vector<vector<vector<string>>> subbytes;
    vector<vector<vector<string>>> mixcols;
    vector<vector<vector<string>>> shiftrows;

    // S-box used for byte substitution
    vector<string> sbox_key;
    vector<vector<string>> sbox_subbytes;

    // Utility function to perform left shift
    vector<string> leftshift(const vector<string> &v)
    {
        vector<string> shifted(v.size());
        for (size_t i = 1; i < v.size(); i++)
        {
            shifted[i - 1] = v[i];
        }
        shifted[v.size() - 1] = v[0];
        return shifted;
    }

    // XOR two hex strings
    string hex_xor(const string &hex1, const string &hex2)
    {
        string result = "";
        for (int i = 0; i < 2; i++)
        {
            int val1 = hex_to_int[hex1[i]];// convert hex character to int
            int val2 = hex_to_int[hex2[i]];
            int sum = val1 ^ val2;// XOR operation
            result += int_to_hex_fun(sum, 1);// convert back to hex
        }
        return result;
    }

    // Key Expansion function to generate round keys from the original key
    void key_expansion(vector<vector<string>> &k0)
    {

        // Copy initial key to round 0 key
        for (int r = 0; r < 4; r++)
        {
            for (int c = 0; c < 4; c++)
            {
                round_keys[0][r][c] = k0[r][c];
            }
        }
        // round constants for key expansion
        vector<string> rcon = {
            "01", "02", "04", "08", "10", "20", "40", "80", "1b", "36"};

        // loop for making 10 round keys
        for (int i = 1; i <= 10; i++)
        {
            vector<string> temp(4);

            // Get the last column of the previous round key
            for (int j = 0; j < 4; j++)
            {
                temp[j] = round_keys[i - 1][j][3];
            }

            // Rotate and apply S-box
            temp = leftshift(temp);
            for (int j = 0; j < 4; j++)
            {
                int hexValue = hex_to_int[temp[j][0]] * 16 + hex_to_int[temp[j][1]];
                temp[j] = sbox_key[hexValue];// substitute using S-box
            }

            // XOR with round constant
            temp[0] = hex_xor(temp[0], rcon[i - 1]);

            // First column of the round key
            for (int j = 0; j < 4; j++)
            {
                round_keys[i][j][0] = hex_xor(round_keys[i - 1][j][0], temp[j]);
            }

            // rest of the round key
            for (int c = 1; c < 4; c++)
            {
                for (int j = 0; j < 4; j++)
                {
                    round_keys[i][j][c] = hex_xor(round_keys[i][j][c - 1], round_keys[i - 1][j][c]);
                }
            }
        }

        // cout<<"Made round keys !!"<<endl;
    }

    // xor round key and current state 
    vector<vector<string>> add_round_key(int r)
    {
        vector<vector<string>> curr_state;

        // get curr state based on round number
        if (r == 0)
        {
            curr_state = states[r];
        }
        else if (r == 10)
        {
            curr_state = shiftrows[r];
        }
        else
        {
            curr_state = mixcols[r];
        }

        vector<vector<string>> curr_rkey = round_keys[r];

        // loop to xor round key and current state
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                curr_state[i][j] = hex_xor(curr_rkey[i][j], curr_state[i][j]);
            }
        }

        return curr_state;
    }

    // get row and col and use sbox for substitution of bytes
    string sbox_use(string hexi)
    {
        int row = hex_to_int[hexi[0]];
        int col = hex_to_int[hexi[1]];

        return sbox_subbytes[row][col];// return substituted byte from S-box
    }

    // substitution of bytes function
    vector<vector<string>> sub_bytes(int r)
    {
        vector<vector<string>> curr_state = states[r];

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                curr_state[i][j] = sbox_use(curr_state[i][j]);// substitute each byte
            }
        }

        // cout<<" --- used subbytes!"<<endl;

        return curr_state;
    }

    // shift rows function
    vector<vector<string>> shift_rows(int r)
    {
        vector<vector<string>> curr_state = subbytes[r];

        for (int i = 0; i < 4; i++)
        {
            int j = 0;
            // shift rows as per number of row
            while (j < i)
            {
                curr_state[i] = leftshift(curr_state[i]);
                j++;
            }
        }
        // cout<<" --- used shiftrows!"<<endl;

        return curr_state;
    }

    // Galois field multiplication used in Mix Columns step
    int galois_multiply(int a, int b)
    {
        int result = 0;
        while (b > 0)
        {
            // If the least significant bit is 1
            if (b & 1)
            {                
                result ^= a; // XOR with the current value of a
            }
            a <<= 1; // Multiply a by 2

            // If a overflows 8 bits
            if (a & 0x100)
            {              
                a ^= 0x1B; // Reduce by the AES polynomial
            }
            b >>= 1; // Divide b by 2
        }
        return result;
    }

    // matrix multiplication function
    vector<vector<string>> multiply_matrices(const vector<vector<string>> &mat1, const vector<vector<string>> &mat2)
    {
        int n = mat1.size();
        vector<vector<int>> result(n, vector<int>(n, 0)); // Use int for calculations

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                for (int k = 0; k < n; k++)
                {
                    // Convert hex strings to integers
                    int val1 = (hex_to_int[mat1[i][k][0]] * 16) + hex_to_int[mat1[i][k][1]];
                    int val2 = (hex_to_int[mat2[k][j][0]] * 16) + hex_to_int[mat2[k][j][1]];
                    
                    result[i][j] ^= galois_multiply(val1, val2); // Used Galois field multiplication
                }
                result[i][j] = result[i][j] % 256; // Reduce to 8 bits
            }
        }

        // Convert result back to hex strings
        vector<vector<string>> hex_result(n, vector<string>(n));
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                hex_result[i][j] = int_to_hex_fun(result[i][j], 2); // Convert to hex
            }
        }

        return hex_result;
    }

    // function to mix columns 
    vector<vector<string>> mix_cols(int r)
    {
        // aes column matrix initialization
        vector<vector<string>> col_mat = {
            {"02", "03", "01", "01"},
            {"01", "02", "03", "01"},
            {"01", "01", "02", "03"},
            {"03", "01", "01", "02"},
        };

        vector<vector<string>> curr_state = shiftrows[r];

        // mix columns using multiplication of matrix
        curr_state = multiply_matrices(col_mat, curr_state);

        return curr_state;
    }

    // encryption function
    void encrypt(vector<vector<string>> s0)
    {   
        // preprocessing of plaintext
        int round_no = 0;
        states[0] = s0;
        states[1] = add_round_key(round_no);

        // cout<<"Pre processing before rounds dones !"<<endl;

        // 9 rounds of aes encryption 
        round_no++;
        while (round_no < 10)
        {
            // cout<<"Processing round "<< round_no <<endl;
            subbytes[round_no] = sub_bytes(round_no);
            shiftrows[round_no] = shift_rows(round_no);
            mixcols[round_no] = mix_cols(round_no);
            states[round_no + 1] = add_round_key(round_no);
            round_no++;
        }

        // 10th round of aes without mix columns
        subbytes[round_no] = sub_bytes(round_no);
        shiftrows[round_no] = shift_rows(round_no);
        states[round_no + 1] = add_round_key(round_no);
    }

public:
    // constructor to initialize everything
    AES(const string &plaintext, const string &key) : states(12, vector<vector<string>>(4, vector<string>(4))),
                                                      subbytes(12, vector<vector<string>>(4, vector<string>(4))),
                                                      shiftrows(12, vector<vector<string>>(4, vector<string>(4))),
                                                      mixcols(12, vector<vector<string>>(4, vector<string>(4))),
                                                      round_keys(12, vector<vector<string>>(4, vector<string>(4)))
    {
        sbox_key = {
    "63", "7c", "77", "7b", "f2", "6b", "6f", "c5", "30", "01", "67", "2b", "fe", "d7", "ab", "76",
    "ca", "82", "c9", "7d", "fa", "59", "47", "f0", "ad", "d4", "a2", "af", "9c", "a4", "72", "c0",
    "b7", "fd", "93", "26", "36", "3f", "f7", "cc", "34", "a5", "e5", "f1", "71", "d8", "31", "15",
    "04", "c7", "23", "c3", "18", "96", "05", "9a", "07", "12", "80", "e2", "eb", "27", "b2", "75",
    "09", "83", "2c", "1a", "1b", "6e", "5a", "a0", "52", "3b", "d6", "b3", "29", "e3", "2f", "84",
    "53", "d1", "00", "ed", "20", "fc", "b1", "5b", "6a", "cb", "be", "39", "4a", "4c", "58", "cf",
    "d0", "ef", "aa", "fb", "43", "4d", "33", "85", "45", "f9", "02", "7f", "50", "3c", "9f", "a8",
    "51", "a3", "40", "8f", "92", "9d", "38", "f5", "bc", "b6", "da", "21", "10", "ff", "f3", "d2",
    "cd", "0c", "13", "ec", "5f", "97", "44", "17", "c4", "a7", "7e", "3d", "64", "5d", "19", "73",
    "60", "81", "4f", "dc", "22", "2a", "90", "88", "46", "ee", "b8", "14", "de", "5e", "0b", "db",
    "e0", "32", "3a", "0a", "49", "06", "24", "5c", "c2", "d3", "ac", "62", "91", "95", "e4", "79",
    "e7", "c8", "37", "6d", "8d", "d5", "4e", "a9", "6c", "56", "f4", "ea", "65", "7a", "ae", "08",
    "ba", "78", "25", "2e", "1c", "a6", "b4", "c6", "e8", "dd", "74", "1f", "4b", "bd", "8b", "8a",
    "70", "3e", "b5", "66", "48", "03", "f6", "0e", "61", "35", "57", "b9", "86", "c1", "1d", "9e",
    "e1", "f8", "98", "11", "69", "d9", "8e", "94", "9b", "1e", "87", "e9", "ce", "55", "28", "df",
    "8c", "a1", "89", "0d", "bf", "e6", "42", "68", "41", "99", "2d", "0f", "b0", "54", "bb", "16"
};


        sbox_subbytes = {
            {"63", "7c", "77", "7b", "f2", "6b", "6f", "c5", "30", "01", "67", "2b", "fe", "d7", "ab", "76"},
            {"ca", "82", "c9", "7d", "fa", "59", "47", "f0", "ad", "d4", "a2", "af", "9c", "a4", "72", "c0"},
            {"b7", "fd", "93", "26", "36", "3f", "f7", "cc", "34", "a5", "e5", "f1", "71", "d8", "31", "15"},
            {"04", "c7", "23", "c3", "18", "96", "05", "9a", "07", "12", "80", "e2", "eb", "27", "b2", "75"},
            {"09", "83", "2c", "1a", "1b", "6e", "5a", "a0", "52", "3b", "d6", "b3", "29", "e3", "2f", "84"},
            {"53", "d1", "00", "ed", "20", "fc", "b1", "5b", "6a", "cb", "be", "39", "4a", "4c", "58", "cf"},
            {"d0", "ef", "aa", "fb", "43", "4d", "33", "85", "45", "f9", "02", "7f", "50", "3c", "9f", "a8"},
            {"51", "a3", "40", "8f", "92", "9d", "38", "f5", "bc", "b6", "da", "21", "10", "ff", "f3", "d2"},
            {"cd", "0c", "13", "ec", "5f", "97", "44", "17", "c4", "a7", "7e", "3d", "64", "5d", "19", "73"},
            {"60", "81", "4f", "dc", "22", "2a", "90", "88", "46", "ee", "b8", "14", "de", "5e", "0b", "db"},
            {"e0", "32", "3a", "0a", "49", "06", "24", "5c", "c2", "d3", "ac", "62", "91", "95", "e4", "79"},
            {"e7", "c8", "37", "6d", "8d", "d5", "4e", "a9", "6c", "56", "f4", "ea", "65", "7a", "ae", "08"},
            {"ba", "78", "25", "2e", "1c", "a6", "b4", "c6", "e8", "dd", "74", "1f", "4b", "bd", "8b", "8a"},
            {"70", "3e", "b5", "66", "48", "03", "f6", "0e", "61", "35", "57", "b9", "86", "c1", "1d", "9e"},
            {"e1", "f8", "98", "11", "69", "d9", "8e", "94", "9b", "1e", "87", "e9", "ce", "55", "28", "df"},
            {"8c", "a1", "89", "0d", "bf", "e6", "42", "68", "41", "99", "2d", "0f", "b0", "54", "bb", "16"}};

        // Parse plaintext and key
        string p = plaintext;
        string k = key;

        // converting plaintext and key to matrix
        vector<vector<string>> s0(4, vector<string>(4));
        vector<vector<string>> k0(4, vector<string>(4));

        int index = 0;
        for (int col = 0; col < 4; col++)
        {
            for (int row = 0; row < 4; row++)
            {
                s0[row][col] = p.substr(index, 2);
                k0[row][col] = k.substr(index, 2);
                index += 2;
            }
        }

        // cout<<"Initialized everything!!"<<endl;

        // generating round keys
        key_expansion(k0);

        // encrypting plaintext
        encrypt(s0);
    }

    // getting last state as string
    string get_cipher()
    {
        string cipher = "";

        for (int col = 0; col < 4; col++)
        {
            for (int row = 0; row < 4; row++)
            {
                cipher += states[11][row][col] + " ";
            }
        }

        return cipher;
    }

    // returning all data 
    vector<vector<vector<vector<string>>>> get_data()
    {
        return {states, subbytes, shiftrows, mixcols, round_keys};
    }
};

// Initialize hex mappings
void ini_maps()
{
    for (int i = 0; i < 10; i++)
    {
        hex_to_int[i + '0'] = i;
    }

    hex_to_int['a'] = 10;
    hex_to_int['b'] = 11;
    hex_to_int['c'] = 12;
    hex_to_int['d'] = 13;
    hex_to_int['e'] = 14;
    hex_to_int['f'] = 15;

    // for (auto p : hex_to_int)
    // {
    //     int_to_hex[p.second] = p.first;
    // }
}

// display matrix function
void disp_mat(vector<vector<string>> mat)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            cout << mat[i][j] << " ";
        }
        cout << endl;
    }
}

// display function for intermediate steps as per user input
void display(vector<vector<vector<vector<string>>>> data)
{

    auto states = data[0];
    auto subbytes = data[1];
    auto shiftrows = data[2];
    auto mixcols = data[3];
    auto roundkeys = data[4];

    while (true)
    {
        int r;
        cout << "------------------" << endl;
        cout << "Do you want to see data for any round ? (0 - 11) any other number is exit! " << endl;
        cout << "Choice : ";
        cin >> r;

        if (r < 0 || r > 11)
            return;

        cout << "\nShowing data for round " << r << endl;

        cout << "\nInitial state : " << endl;
        disp_mat(states[r]);

        if (r != 0 && r != 11)
        {
            cout << "\nAfter subbytes : " << endl;
            disp_mat(subbytes[r]);

            cout << "\nAfter shift rows : " << endl;
            disp_mat(shiftrows[r]);

            if (r != 10)
            {
                cout << "\nAfter mix columns : " << endl;
                disp_mat(mixcols[r]);
            }
        }

        if (r != 11)
        {
            cout << "\nRound key : " << endl;
            disp_mat(roundkeys[r]);
        }
    }
}

int main()
{
    // initializing hex maps
    ini_maps();

    // string plaintext = "01 23 45 67 89 ab cd ef fe dc ba 98 76 54 32 10";
    // string key = "0f 15 71 c9 47 d9 e8 59 0c b7 ad d6 af 7f 67 98";

    string plaintext, key;

    cout << "Plaintext: ";
    getline(cin, plaintext);

    cout << "Key: ";
    getline(cin, key);


    // removing spaces from plaintext and key
    plaintext.erase(remove(plaintext.begin(), plaintext.end(), ' '), plaintext.end());
    key.erase(remove(key.begin(), key.end(), ' '), key.end());


    // checking size if more tha 32*4 (128) bits then wrong input
    if (plaintext.size() > 32 || key.size() > 32){
        cout<<"plaintext and key size must be 128 bits !!"<<endl;
        return 1;
    }

    // adding padding if size less than 128 bits
    while (plaintext.size()<32)
    {
        plaintext+='0';
    }

    while(key.size()<32){
        key +='0';
    }

    // initiallizing aes object with plaintext and key
    AES aes(plaintext, key);

    // getting encrypted text
    string cipher = aes.get_cipher();
    cout << "Ciphertext: " << cipher << endl;

    // getting and showing intermediate data
    auto data = aes.get_data();
    display(data);
    
    return 0;
}

/*Plaintext: 01 23 45 67 89 ab cd ef fe dc ba 98 76 54 32 10
Key: 0f 15 71 c9 47 d9 e8 59 0c b7 ad d6 af 7f 67 98
Ciphertext: ff 0b 84 4a 08 53 bf 7c 69 34 ab 43 64 14 8f b9*/