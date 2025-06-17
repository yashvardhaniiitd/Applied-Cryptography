# Linear Cryptanalysis on basic SPN cipher

Linear cryptanalysis is a statistical attack technique used against block ciphers like SPN (Substitution-Permutation Network). It exploits linear approximations to the behaviour of the cipher to recover information about the secret key. 

## Build 

Run command `make` which will make a fmain.o file and then run it using `./fmain.o` command. This is the better file to run for analysis and seeing how linear attack works. 

We can also build opmain.cpp or main.cpp which for exhaustive attacks will take a lot of time which is not feasible to run in normal computers.


## Code files 

- `spn.cpp` : is the library for SPN cipher and contains functions to generate round keys from 80bits(20 hex digits) master key and spn encryption function.
- `spn_encrypt.cpp` :contains codes for menu for inputs of master key (random generation and user input) and plaintext of 16bits (4 hex digits) and saving master key in key.txt file along with encrypted plaintext in cipher.txt file using spn.cpp.
- `helper_functions.cpp` : contains data structures and functions for sbox and permutations and also their reverse for spn cipher.
- `read_pairs.cpp` : it is a file used to read the plaintext and ciphertext pairs saved in data.csv file.
-  `linear_attack.cpp` : it is the file where the linear attack code is which takes in plaintext and cipher text pairs and does linear attack using them also printing all the bias.
-  `main.cpp` : it is the main program file for linear cryptanalysis it first runs the  spn_encrypt.cpp for encryption and saving of ciphertext and keys then makes 10,000 random pairs of plaintext and ciphertext and runs linear attack on them. Finally running an exhaustive search for the full master key.
- `opmain.cpp` : it is the same as main.cpp but we parallelized the for loops for the exhaustive search so that we can save time.
- `fixedkeyspn_encrypt.cpp` : it is the same as spn_encrypt.cpp but for keys we fixed first 48bits (12 hex digits) as "F"
- `fixedkeymain.cpp` : it is the same as opmain.cpp but here also we fixed the first 48 bits as F for exhaustive search and therefore it uses fixedkeyspn_encrypt.cpp for inputs and encryption.


## Reference 

- A Tutorial on Linear and Differential Cryptanalysis by Howard M. Heys
- Book - Cryptography theory and practice by Stinson and Paterson