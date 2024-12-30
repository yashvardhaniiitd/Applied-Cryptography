

// test cases

/*Key = 0x0000 0000 0000 0000 0000
IV = 0x0000 0000 0000 0000 0000
keystream = 0xFBE0 BF26 5859 051B 517A 2E4E 239F C97F 5632 0316 1907 CF2D E7A8 790F A1B2 E9CD F752 9203 0268 B738 2B4C 1A75 9AA2 599A 2855 4998 6E74 8059 0380 1A4C B5A5 D4F2

Key = 0x8000 0000 0000 0000 0000
IV = 0x0000 0000 0000 0000 0000
keystream = 0x38EB 86FF 730D 7A9C AF8D F13A 4420 540D BB7B 6514 64C8 7501 5520 41C2 49F2 9A64 D2FB F515 6109 21EB E06C 8F92 CECF 7F80 98FF 20CC CC6A 62B9 7BE8 EF74 54FC 80F9
*/

/*
00000000000000000000000000000000000000000000000000000000000000000000000010000000

keystream = 0x38EB 86FF 730D 7A9C AF8D F13A 4420 540D BB7B 6514 64C8 7501 5520 41C2 49F2 9A64 D2FB F515 6109 21EB E06C 8F92 CECF 7F80 98FF 20CC CC6A 62B9 7BE8 EF74 54FC 80F9
*/

/*key = 00400000000000000000
  IV = 00000000
  keystream = 6120 8D28 6BC1 DC43 1171 EDA5 CAF7 9D95 60B1 8ACE F264 8441 7B65 1A47 A3F7 A803 53F7 9AF8 656D A430 1A5E 5A02 E042 65B1 82C6 7F58 9122 0349 F8CD 1CD0 6597 B77E

  key = 00002000000000000000
  IV = 00000000000000000000
  stream[0..63] = C8F9031DABF8DB03FF120D05512B5F24
                  EAEA1BAB43201A5E93BF17F628E5B216
                  D58577112F581A67DD5F962484ED4AC5
                  9202BA3509A73E119680B562F86DF0DC
*/

#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <algorithm>
#include <map>

using namespace std;

class trivium
{
private:
  // 3 trivium shift registers
  vector<int> A;
  vector<int> B;
  vector<int> C;

public:
  // initialization stage of trivium
  trivium(vector<int> key, vector<int> iv)
  {
    //  initialize the shift registers and assign them 0 for all bits
    A.resize(93, 0);
    B.resize(84, 0);
    C.resize(111, 0);

    // filling key and initialization vector in shift registers
    for (int i = 0; i < 80; i++)
    {
      A[i] = key[i];
      B[i] = iv[i];
    }

    C[108] = C[109] = C[110] = 1;

    // warm-up stage
    for (int i = 0; i < 4 * 288; i++)
    {
      round();
    }
  }

  // generating output stream
  int round()
  {
    int t1 = A[65] ^ A[92];
    int t2 = B[68] ^ B[83];
    int t3 = C[65] ^ C[110];

    int output = t1 ^ t2 ^ t3;

    int fB = t1 ^ (A[90] & A[91]) ^ B[77];
    int fC = t2 ^ (B[81] & B[82]) ^ C[86];
    int fA = t3 ^ (C[108] & C[109]) ^ A[68];

    // filling A shift register
    for (int i = 92; i >= 0; i--)
    {
      A[i] = A[i - 1];
    }
    A[0] = fA;

    //  filling B shift register
    for (int i = 83; i >= 0; i--)
    {
      B[i] = B[i - 1];
    }
    B[0] = fB;

    // filling C shift register
    for (int i = 110; i >= 0; i--)
    {
      C[i] = C[i - 1];
    }
    C[0] = fC;

    return output;
  }
};

// map for converting hexadecimal to binary
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

int main()
{
  // input key and iv
  string key;
  string iv;

  cout << "Key = 0x";
  getline(cin, key, '\n');

  cout << "IV = 0x";
  getline(cin, iv, '\n');

  // removing spaces from key and iv
  key.erase(remove(key.begin(), key.end(), ' '), key.end());
  // cout << key << " " << key.size() << endl;
  iv.erase(remove(iv.begin(), iv.end(), ' '), iv.end());
  // cout << iv << " " << iv.size() << endl;

  // checking size for key and iv
  if (key.size() > 20 || iv.size() > 20)
  {
    cout << "Size of key and IV must be 80 bits / 10 bytes" << endl;
    return 1;
  }

  // managing that key and iv are both 80 bits if inserted less
  if (key.size() < 20)
    key.insert(0, 20 - key.size(), '0');

  if (iv.size() < 20)
    iv.insert(0, 20 - iv.size(), '0');

  // map to change from hexadecimal to binary
  map<char, vector<int>> hex_bin = hex_to_bin();

  // map to change from 4 bits binary to hexadecimal
  map<vector<int>, char> bin_hex;
  for (auto pair : hex_bin)
  {
    bin_hex[pair.second] = pair.first;
  }

  // input handling of hexadecimal
  reverse(key.begin(), key.end());
  for (int i = 0; i < key.size() - 1; i += 2)
  {
    char temp = key[i];
    key[i] = key[i + 1];
    key[i + 1] = temp;
  }

  // making a key and iv vector for trivium initialization
  vector<int> k(80, 0);
  vector<int> IV(80, 0);

  // filling up the vectors in binary
  for (int i = 0; i < key.size(); i++)
  {
    // checking for wrong input such as given alphas apart from used in hexadecimals
    if (hex_bin.find(key[i]) == hex_bin.end() || hex_bin.find(iv[i]) == hex_bin.end())
    {
      cout << "invalid input !" << endl;
      return 1;
    }

    // input binary in key and initialization vector
    vector<int> b = hex_bin[key[i]];
    vector<int> c = hex_bin[iv[i]];

    for (int j = 0; j < 4; j++)
    {
      k[4 * i + j] = b[j];
      IV[4 * i + j] = c[j];
    }
  }

  // cout << endl;
  // for (int i = 0; i < 80; i++)
  // {
  //   cout << k[i];
  // }
  // cout << endl;

  // a trivium object and ouput for it
  trivium t(k, IV);

  // initialize output vector
  vector<int> output(512);

  // collect output stream from trivium for 512 bits
  for (int i = 0; i < 512; i++)
  {
    output[i] = t.round();
  }

  // generating 512 bits keystream
  string keystream;

  // for output space handling
  bool space = false;

  // converting binary to hexadecimal
  for (int i = 0; i < 512; i += 8)
  {
    vector<int> bits(output.begin() + i, output.begin() + i + 8);

    // output handling reversing bytes and then adding in output
    reverse(bits.begin(), bits.end());

    vector<int> bits1(bits.begin(), bits.begin() + 4);
    vector<int> bits2(bits.begin() + 4, bits.end());

    keystream += bin_hex[bits1];
    keystream += bin_hex[bits2];

    // adding space after every 2 bytes
    if (space)
    {
      keystream += " ";
    }

    space = !space;
  }

  // output keystream
  cout << "\nKeystream = 0x" << keystream << endl;

  return 0;
}