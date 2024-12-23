#include<bits/stdc++.h>
using namespace std;

int modInverse(int k) {
    for(int i = 0; i < 27; i++) { 
        int flag = (i * k) % 27;    
        if(flag == 1) {
            return i;
        }
    }
    return -1;
}

string decrypt(string cipher_text, int a, int b) {
    string plain_text = "";
    int a_inverse = modInverse(a);
    if(a_inverse == -1) {
        return "Not exist";
    }
    cout << "a Inverse: " << a_inverse << endl;
    
    for(int i = 0; i < cipher_text.size(); i++) {
        if(cipher_text[i] == ' ') {
            // cipher_text[i]-64: to convert the range 1-26
            // - b: reverses the addition of b during encryption
            /* % 27: If the number is positive and less than 27, % 27 has no effect.
            If the number is larger than 27, % 27 wraps it back to the range 1-26.
            If the number is negative, % 26 can result in a negative remainder in C++ */
            // + 27: ensures that the mod operation doesn’t return a negative result in case a negative remainder
            // %27 again: if the remainder was already negative this step has no effect but if the number was positive when we added 27 it may get out of 1-26 range so this ensures the number stays within the range
            // * a_inverse: Multiply by a^-1
            // %27 for the third time: wrap within 1–26
            // +64: convert the letter back to its ASCII representation
            // Space is at index 0
            int val = ((((0 - b) % 27 + 27) % 27) * a_inverse) % 27;
            plain_text += (val == 0) ? ' ' : (val <= 26 ? char(val + 64) : ' ');
        }
        else if(isupper(cipher_text[i])) {
            // Shift the alphabet indices up by 1 (A=1, B=2, ..., Z=26)
            int val = ((((cipher_text[i] - 64 - b) % 27 + 27) % 27) * a_inverse) % 27;
            plain_text += (val == 0) ? ' ' : char(val + 64);
        }
        else if(islower(cipher_text[i])) {
            // Similar adjustment for lowercase letters
            int val = ((((cipher_text[i] - 96 - b) % 27 + 27) % 27) * a_inverse) % 27;
            plain_text += (val == 0) ? ' ' : char(val + 96);
        }
    }
    return plain_text;
}

int main() {
    string cipher_text;
    cout << "Enter the ciphered text: ";   // "NEZKLYECIDJYCIDNQ "
    cin >> cipher_text;
    int a, b;
    cout << "Enter first key: ";
    cin >> a;
    cout << "Enter second key: ";
    cin >> b;
    
    string plain_text = decrypt(cipher_text, a, b);
    cout << "Plain Text: " << plain_text << endl;
    
    return 0;
}
