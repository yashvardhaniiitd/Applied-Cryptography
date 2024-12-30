# Assignment 2 - Trivium Cipher (FSR based Stream Cipher)

## In this assignment you need to implement the Trivium Cipher.

The formal description of Trivium Cipher has been taught in the class and can be found here - https://www.ecrypt.eu.org/stream/p3ciphers/trivium/trivium_p3.pdf

In this implementation you need to generate 512 bits of keystream while taking the key and IV as the input.

The test vectors are (the values are in hex):
Key = 0x0000 0000 0000 0000 0000
IV = 0x0000 0000 0000 0000 0000 
keystream = 0xFBE0 BF26 5859 051B 517A 2E4E 239F C97F
                          5632 0316 1907 CF2D E7A8 790F A1B2 E9CD
                          F752 9203 0268 B738 2B4C 1A75 9AA2 599A
                          2855 4998 6E74 8059 0380 1A4C B5A5 D4F2
Key = 0x8000 0000 0000 0000 0000 
IV = 0x0000 0000 0000 0000 0000 
keystream = 0x38EB 86FF 730D 7A9C AF8D F13A 4420 540D
                           BB7B 6514 64C8 7501 5520 41C2 49F2 9A64
                           D2FB F515 6109 21EB E06C 8F92 CECF 7F80
                           98FF 20CC CC6A 62B9 7BE8 EF74 54FC 80F9