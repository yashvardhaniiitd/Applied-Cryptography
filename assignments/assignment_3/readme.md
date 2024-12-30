# Assignment 3 - DES Encryption and Avalanche Property

## In this assignment you need to implement the DES Encryption and show the Avalanche Property for change in plaintext as well as key.

Test Case for DES Encryption

- Plaintext: 123456ABCD132536        
- Key: AABB09182736CCDD        
- CipherText: C0B7A8D05F3A829C


- After initial permutation:14A7D67818CA18AD
- After splitting: L0=14A7D678         R0=18CA18AD

- Round       Left                  Right                  Round Key

- 1          18CA18AD        5A78E394         194CD072DE8C
........................................................................................
........................................................................................
.......................................................................................
- 16         19BA9212        CF26B472         181C5D75C66D

- Ciphertext: C0B7A8D05F3A829C (after final permutation)
---------------------------------------------------------------------------------------------------------------------------------------------------



- Example for Avalanche Property (as you can see there is a difference in only one bit of the plaintext)
- Plaintext: 0000000000000000       Key: 22234512987ABB23    Ciphertext: 4789FD476E82A5F1

- Plaintext: 0000000000000001       Key: 22234512987ABB23    Ciphertext: 0A4ED5C15A63FEA3 


- Then in this case you should observe the changes in the following number of bits in each round as described below:
- Rounds:                 1    2    3     4    5    6    7    8    9    10    11    12    13    14    15    16
- Bit differences:     1   6   20   29  30  33  32  29  32  39    33    28     30    31    30    29