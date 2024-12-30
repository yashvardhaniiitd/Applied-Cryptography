/*

Test Case for DES Encryption

Plaintext: 123456ABCD132536        Key: AABB09182736CCDD          CipherText: C0B7A8D05F3A829C

After initial permutation:14A7D67818CA18AD
After splitting: L0=14A7D678         R0=18CA18AD

Round       Left                  Right                  Round Key

1          18CA18AD        5A78E394         194CD072DE8C
........................................................................................
........................................................................................
.......................................................................................
16         19BA9212        CF26B472         181C5D75C66D

Ciphertext: C0B7A8D05F3A829C (after final permutation)

Example for Avalanche Property (as you can see there is a difference in only one bit of the plaintext)
Plaintext: 0000000000000000       Key: 22234512987ABB23    Ciphertext: 4789FD476E82A5F1

Plaintext: 0000000000000001       Key: 22234512987ABB23    Ciphertext: 0A4ED5C15A63FEA3


Then in this case you should observe the changes in the following number of bits in each round as described below:
Rounds:                 1    2    3     4    5    6    7    8    9    10    11    12    13    14    15    16
Bit differences:     1   6   20   29  30  33  32  29  32  39    33    28     30    31    30    29*/

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

// map for converting hexadecimal to binary and vice versa
map<char, vector<int>> hex_to_bin()
{
  map<char, vector<int>> hex_bin;

  hex_bin['0'] = {0, 0, 0, 0};
  hex_bin['1'] = {0, 0, 0, 1};
  hex_bin['2'] = {0, 0, 1, 0};
  hex_bin['3'] = {0, 0, 1, 1};
  hex_bin['4'] = {0, 1, 0, 0};
  hex_bin['5'] = {0, 1, 0, 1};
  hex_bin['6'] = {0, 1, 1, 0};
  hex_bin['7'] = {0, 1, 1, 1};
  hex_bin['8'] = {1, 0, 0, 0};
  hex_bin['9'] = {1, 0, 0, 1};
  hex_bin['A'] = {1, 0, 1, 0};
  hex_bin['B'] = {1, 0, 1, 1};
  hex_bin['C'] = {1, 1, 0, 0};
  hex_bin['D'] = {1, 1, 0, 1};
  hex_bin['E'] = {1, 1, 1, 0};
  hex_bin['F'] = {1, 1, 1, 1};

  return hex_bin;
}

map<char, vector<int>> hex_bin = hex_to_bin();
map<vector<int>, char> bin_hex;

string binary_to_hex(vector<int> bin)
{
  int n = bin.size();
  string res = "";

  for (int i = 0; i < n; i += 4)
  {
    vector<int> hex(bin.begin() + i, bin.begin() + i + 4);

    char c = bin_hex[hex];
    res.push_back(c);
  }

  return res;
}

vector<int> hex_to_binary(string hexi)
{
  vector<int> tmp;

  int n = hexi.size();
  for (int i = 0; i < n; i++)
  {
    vector<int> ans = hex_bin[hexi[i]];
    for (auto ele : ans)
    {
      tmp.push_back(ele);
    }
  }

  return tmp;
}

// class for des encyption
class DES
{
private:
  // variables for permutations
  vector<int> initial_perm;
  vector<int> exp_d;
  vector<int> per;
  vector<vector<vector<int>>> sbox;
  vector<int> final_perm;
  vector<int> keyp;
  vector<int> keyp2;
  vector<int> key_comp;

  string i_per;
  string f_per;

  // extra map for dec to bin and vice-versa
  map<int, vector<int>> dec_bin;
  map<vector<int>, int> bin_dec;

  // variables to store plain text , key, round ciphers and round keys
  string plaintext;
  string main_key;

  vector<string> left_Halfs;
  vector<string> right_Halfs;

  vector<string> roundKeys;

  //  2 bit conversion to decimal
  int bin_to_dec(int a, int b)
  {
    if (a == b)
    {
      if (a == 1)
        return 3;
      return 0;
    }
    if (a == 1)
      return 2;
    return 1;
  }

  vector<int> perm(vector<int> table, vector<int> input)
  {
    int n = table.size();
    vector<int> ans;

    for (int i = 0; i < n; i++)
    {
      ans.push_back(input[table[i] - 1]);
    }

    return ans;
  }

  // Key generation function
  void generate_Round_Keys()
  {
    // Convert key to binary and apply parity drop permutation 64 -> 56 bits
    vector<int> key_main = hex_to_binary(main_key);
    vector<int> p_key = perm(keyp, key_main);

    // Split the 56-bit permuted key into two 28-bit halves
    vector<int> leftHalf(p_key.begin(), p_key.begin() + 28);
    vector<int> rightHalf(p_key.begin() + 28, p_key.end());

    // Generate 16 round keys
    for (int round = 0; round < 16; ++round)
    {
      // Perform left circular shifts (1 or 2 depending on round)
      int shifts = (round == 0 || round == 1 || round == 8 || round == 15) ? 1 : 2;

      // applying left circular shifts
      rotate(leftHalf.begin(), leftHalf.begin() + shifts, leftHalf.end());
      rotate(rightHalf.begin(), rightHalf.begin() + shifts, rightHalf.end());

      // Combine the halves generate the 48-bit round key
      vector<int> combinedKey;
      combinedKey.insert(combinedKey.end(), leftHalf.begin(), leftHalf.end());
      combinedKey.insert(combinedKey.end(), rightHalf.begin(), rightHalf.end());

      // apply key compression permutation
      vector<int> roundKey = perm(key_comp, combinedKey);
      roundKeys.push_back(binary_to_hex(roundKey));
    }
  }

  // function for xor operation
  vector<int> do_xor(vector<int> i1, vector<int> i2)
  {
    int n = i1.size();

    vector<int> ans;
    for (int i = 0; i < n; i++)
    {
      // simply xor 2 bits of i1 and i2
      ans.push_back(i1[i] ^ i2[i]);
    }

    return ans;
  }

  // des internal functions rounds
  vector<int> internal_round(string r, int round_no)
  {
    vector<int> curr_right = hex_to_binary(r);

    // expansion permutation from 32 to 48 bits
    curr_right = perm(exp_d, curr_right);

    // cout << binary_to_hex(curr_right) << endl;

    // xor with round key
    curr_right = do_xor(curr_right, hex_to_binary(roundKeys[round_no]));

    vector<int> res;

    // cout << "Calculating right : " << binary_to_hex(curr_right) << endl;

    // s box operation
    for (int i = 0; i < 8; i++)
    {
      // taking 6 bits
      vector<int> r_part(curr_right.begin() + i * 6, curr_right.begin() + i * 6 + 6);

      // calculation row and column for s-box
      int row = bin_to_dec(r_part[0], r_part[5]);
      vector<int> c(r_part.begin() + 1, r_part.end() - 1);

      int col = bin_dec[c];

      // cout << "ROW : " << row << endl;
      // cout << "COL : " << col << endl;

      // get value from s-box
      int tmp = sbox[i][row][col];
      // cout << "tmp: " << tmp << endl;

      // storing s-box value
      r_part = dec_bin[tmp];
      for (int i = 0; i < 4; i++)
      {
        // cout << r_part[i] << " ";
        res.push_back(r_part[i]);
      }
    }

    // cout << "size after sboxes : " << res.size() << endl;

    // internal last permutation
    curr_right = perm(per, res);
    return curr_right;
  }

public:
  DES(string p, string k)
  {
    plaintext = p;
    main_key = k;

    // dec to bin and vice-versa initialization

    dec_bin = {
        {10, {1, 0, 1, 0}},
        {11, {1, 0, 1, 1}},
        {12, {1, 1, 0, 0}},
        {13, {1, 1, 0, 1}},
        {14, {1, 1, 1, 0}},
        {15, {1, 1, 1, 1}}};

    for (int i = 0; i < 10; i++)
    {
      dec_bin[i] = hex_bin[i + '0'];
    }
    for (auto pair : dec_bin)
    {
      bin_dec[pair.second] = pair.first;
    }

    // key permutation
    keyp = {57, 49, 41, 33, 25, 17, 9,
            1, 58, 50, 42, 34, 26, 18,
            10, 2, 59, 51, 43, 35, 27,
            19, 11, 3, 60, 52, 44, 36,
            63, 55, 47, 39, 31, 23, 15,
            7, 62, 54, 46, 38, 30, 22,
            14, 6, 61, 53, 45, 37, 29,
            21, 13, 5, 28, 20, 12, 4};

    // key compression
    key_comp = {14, 17, 11, 24, 1, 5,
                3, 28, 15, 6, 21, 10,
                23, 19, 12, 4, 26, 8,
                16, 7, 27, 20, 13, 2,
                41, 52, 31, 37, 47, 55,
                30, 40, 51, 45, 33, 48,
                44, 49, 39, 56, 34, 53,
                46, 42, 50, 36, 29, 32};

    // initial permutation
    initial_perm = {58, 50, 42, 34, 26, 18, 10, 2,
                    60, 52, 44, 36, 28, 20, 12, 4,
                    62, 54, 46, 38, 30, 22, 14, 6,
                    64, 56, 48, 40, 32, 24, 16, 8,
                    57, 49, 41, 33, 25, 17, 9, 1,
                    59, 51, 43, 35, 27, 19, 11, 3,
                    61, 53, 45, 37, 29, 21, 13, 5,
                    63, 55, 47, 39, 31, 23, 15, 7};

    //  Expansion D-box Table
    exp_d = {32, 1, 2, 3, 4, 5, 4, 5,
             6, 7, 8, 9, 8, 9, 10, 11,
             12, 13, 12, 13, 14, 15, 16, 17,
             16, 17, 18, 19, 20, 21, 20, 21,
             22, 23, 24, 25, 24, 25, 26, 27,
             28, 29, 28, 29, 30, 31, 32, 1};

    // Permutation Table
    per = {16, 7, 20, 21,
           29, 12, 28, 17,
           1, 15, 23, 26,
           5, 18, 31, 10,
           2, 8, 24, 14,
           32, 27, 3, 9,
           19, 13, 30, 6,
           22, 11, 4, 25};

    // S-box Table
    sbox = {{{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
             {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
             {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
             {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}},

            {{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
             {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
             {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
             {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}},

            {{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
             {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
             {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
             {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}},

            {{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
             {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
             {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
             {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}},

            {{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
             {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
             {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
             {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}},

            {{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
             {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
             {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
             {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}},

            {{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
             {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
             {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
             {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}},

            {{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
             {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
             {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
             {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}}};

    // final permutation
    final_perm = {40, 8, 48, 16, 56, 24, 64, 32,
                  39, 7, 47, 15, 55, 23, 63, 31,
                  38, 6, 46, 14, 54, 22, 62, 30,
                  37, 5, 45, 13, 53, 21, 61, 29,
                  36, 4, 44, 12, 52, 20, 60, 28,
                  35, 3, 43, 11, 51, 19, 59, 27,
                  34, 2, 42, 10, 50, 18, 58, 26,
                  33, 1, 41, 9, 49, 17, 57, 25};

    // generate round keys
    generate_Round_Keys();
  }

  vector<vector<string>> getdata()
  {
    vector<string> permutations;
    permutations.push_back(i_per);
    permutations.push_back(f_per);

    vector<vector<string>> res;
    res.push_back(left_Halfs);
    res.push_back(right_Halfs);
    res.push_back(roundKeys);
    res.push_back(permutations);

    return res;
  }

  // DES encryption process
  void encrypt()
  {
    // Initial Permutation
    vector<int> bin_plaintext = hex_to_binary(plaintext);
    vector<int> permuted_plaintext = perm(initial_perm, bin_plaintext);

    i_per = binary_to_hex(permuted_plaintext);
    cout << "Initial Permutations : " << i_per << endl;

    // Split into left and right halves
    vector<int> left_half(permuted_plaintext.begin(), permuted_plaintext.begin() + 32);
    vector<int> right_half(permuted_plaintext.begin() + 32, permuted_plaintext.end());

    cout << "Round : " << "\t" << "LEFT" << "\t\t" << "RIGHT" << "\t\t" << "ROUND KEY" << endl;

    // Perform 16 rounds
    for (int round_no = 0; round_no < 16; ++round_no)
    {
      vector<int> new_right_half = internal_round(binary_to_hex(right_half), round_no);

      // XOR the new right half with the left half and swap halves
      vector<int> temp = right_half;
      right_half = do_xor(left_half, new_right_half);
      left_half = temp;

      // store left and right halfs
      left_Halfs.push_back(binary_to_hex(left_half));
      right_Halfs.push_back(binary_to_hex(right_half));

      // output the values
      cout << round_no + 1 << "\t\t" << left_Halfs[round_no] << "\t" << right_Halfs[round_no] << "\t" << roundKeys[round_no] << endl;
    }

    // Combine the halves a
    vector<int> combined;
    combined.insert(combined.end(), right_half.begin(), right_half.end());
    combined.insert(combined.end(), left_half.begin(), left_half.end());

    // apply final permutation
    vector<int> final_ciphertext = perm(final_perm, combined);
    f_per = binary_to_hex(final_ciphertext);
    cout << "Final Permutation : " << f_per << endl;
  }
};

// count bit difference
int difference(string i1, string i2)
{
  vector<int> o1 = hex_to_binary(i1);
  vector<int> o2 = hex_to_binary(i2);

  int cnt = 0;

  for (int i = 0; i < 64; i++)
  {
    // if bit not equal increase cnt
    if (o1[i] != o2[i])
    {
      cnt++;
    }
  }

  return cnt;
}

// check difference between des original and avalanche data
void check_difference(vector<vector<string>> des_data, vector<vector<string>> ava_data)
{
  for (int round = 0; round < 16; round++)
  {
    // extract round ciphers
    string des_curr = (des_data[0][round]) + (des_data[1][round]);
    string ava_curr = (ava_data[0][round]) + (ava_data[1][round]);

    // cout << des_curr << "\t" << ava_curr << endl;

    // count bit difference
    int bit_diff = difference(des_curr, ava_curr);
    cout << round + 1 << " --- " << bit_diff << endl;
  }
}
int main()
{
  // initializing map
  for (auto pair : hex_bin)
  {
    bin_hex[pair.second] = pair.first;
  }

  // initialize plaintext and key
  string Plaintext;
  string Key;

  cout << "input must be of 8 bytes / 16 char in hex" << endl;

  // input plaintext and key
  cout << "Plaintext : ";
  cin >> Plaintext;

  cout << "Key : ";
  cin >> Key;

  // checking input size
  if (Plaintext.size() != 16 || Key.size() != 16)
  {
    cout << "input must be of 8 bytes / 16 char in hex !!" << endl;
    return 1;
  }

  // making a des object
  DES des(Plaintext, Key);
  // encrypting plaintext
  des.encrypt();
  cout << endl;

  // fetching the data
  vector<vector<string>> des_data = des.getdata();

  // changing the last bit of plaintext
  vector<int> ava = hex_to_binary(Plaintext);
  ava[63] = !ava[63];
  string avalanche = binary_to_hex(ava);
  cout << "Avalanche : " << avalanche << endl;

  // using avalanche plaintext making des class and encrypting it
  DES aval(avalanche, Key);
  aval.encrypt();
  cout << endl;

  // fetching avalanche data
  vector<vector<string>> ava_data = aval.getdata();

  // checking bit difference
  check_difference(des_data, ava_data);
}
