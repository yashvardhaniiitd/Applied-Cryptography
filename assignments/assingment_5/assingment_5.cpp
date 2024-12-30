#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>

using namespace std;

// Function to compute modular exponentiation
long long mod_exp(long long base, long long exp, long long mod)
{
    long long result = 1;
    while (exp > 0)
    {
        if (exp % 2 == 1)
        {
            result = (result * base) % mod;
        }
        base = (base * base) % mod;
        exp /= 2;
    }
    return result;
}

// Function to find gcd using Euclidean algorithm
long long gcd(long long a, long long b)
{
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

// Function to compute modular inverse using extended Euclidean algorithm
long long mod_inverse(long long a, long long mod)
{
    long long m0 = mod, t, q;
    long long x0 = 0, x1 = 1;
    if (mod == 1)
        return 0;

    while (a > 1)
    {
        q = a / mod;
        t = mod;
        mod = a % mod, a = t;
        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }

    if (x1 < 0)
        x1 += m0;
    return x1;
}

// Signature generation
void sign_message(long long p, long long alpha, long long a, long long m, long long k, long long &r, long long &s)
{
    // Validate k
    if (gcd(k, p - 1) != 1)
    {
        cout << "Invalid value of k. gcd(k, p-1) must be 1." << endl;
        exit(1);
    }

    r = mod_exp(alpha, k, p);                // r = g^k mod p
    long long k_inv = mod_inverse(k, p - 1); // k_inv = k^-1 mod (p-1)
    s = (k_inv * (m - a * r)) % (p - 1);     // s = k^-1 * (m - a*r) mod (p-1)

    if (s < 0)
        s += (p - 1); // Ensure s is positive
}

// Verify signature
bool verify_signature(long long p, long long alpha, long long y, long long m, long long r, long long s)
{
    if (r <= 0 || r >= p)
        return false;                                       // Check bounds of r
    long long v1 = mod_exp(y, r, p) * mod_exp(r, s, p) % p; // y^r * r^s mod p
    long long v2 = mod_exp(alpha, m, p);                    // g^m mod p

    return v1 == v2;
}

// key generation
long long key_gen(long long &p, long long &alpha, long long &a)
{
    // Get user inputs
    cout << "Enter a large prime number (p): ";
    cin >> p;

    cout << "Enter a primitive root modulo p (alpha): ";
    cin >> alpha;

    cout << "Enter your private key (a, 1 <= a <= p-2): ";
    cin >> a;

    // Calculate public key
    long long y = mod_exp(alpha, a, p); // y = alpha^a mod p
    cout << "Public Key (p, g, y): (" << p << ", " << alpha << ", " << y << ")" << endl;

    return y;
}

int main()
{
    // Public and private keys
    long long p, alpha, a;
    long long y = key_gen(p, alpha, a);

    long long message;
    cout << "Enter a message (as an integer): ";
    cin >> message;

    long long k;
    cout << "Enter a random value for k (1 <= k <= p-2, gcd(k, p-1) = 1): ";
    cin >> k;

    long long r, s;
    sign_message(p, alpha, a, message, k, r, s);
    cout << "Signature (r, s): (" << r << ", " << s << ")" << endl;

    bool is_valid = verify_signature(p, alpha, y, message, r, s);
    cout << "Signature verification: " << (is_valid ? "Valid" : "Invalid") << endl;

    return 0;
}
