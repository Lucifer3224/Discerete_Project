#include <iostream>
#include <string>
using namespace std;

// Function to get the position of a character in a string
int getposition(const string& Gaelic_alphabet, char c) {
    for (int i = 0; i < Gaelic_alphabet.size(); i++) {
        if (Gaelic_alphabet[i] == c)
            return i;
    }
    return -1; // Return -1 if character is not found
}

// Function to get a character from the Gaelic alphabet based on its index
char getchar(const string& Gaelic_alphabet, int index) {
    if (index >= 0 && index < Gaelic_alphabet.size()) {
        return Gaelic_alphabet[index];
    }
    return '?';
}

// Affine Cipher function
string affine_cipher(const string& message, int a, int b) {
    string Gaelic_alphabet = "ABCDEFGHILMNOPRSTU "; // Scottish Gaelic alphabet (uppercase only)
    int m = Gaelic_alphabet.length(); // Alphabet size = 18
    string cipheredMessage = "";
    string not_valid_alphabet = "JjKkQqVvWwXxYyZz"; // Unsupported characters

    for (char c : message) {
        /*  if (c == ' ')
           cipheredMessage = cipheredMessage + "*";*/

        if (not_valid_alphabet.find(c) != string::npos) { // Check if the character is invalid
            cout << "Invalid character in message: " << c << endl;
            return "none";
        }
        else {
            char base = c;
            c = toupper(c); // Convert to uppercase
            int pos = getposition(Gaelic_alphabet, c); // Find position in the alphabet
            if (pos == -1) { // Handle case where character is not in Gaelic alphabet
                cout << "Invalid character in message: " << c << endl;
                return "none";
            }

            int newPos = (a * pos + b) % m; // Apply Affine Cipher formula
            if (c == base)
                cipheredMessage += getchar(Gaelic_alphabet, newPos); // Append encrypted character
            else
                cipheredMessage = cipheredMessage + "-" + getchar(Gaelic_alphabet, newPos);

        }


    }
    return cipheredMessage;
}

int main() {
    string message;
    int a = 4, b = 5; 

    // Input the message
    cout << "Enter the message to cipher: ";
    getline(cin, message);

    // Encrypt the message
    string cipheredMessage = affine_cipher(message, a, b);

    // Output the ciphered message
    if (cipheredMessage != "none") {
        cout << "Ciphered message: " << cipheredMessage << endl;
    }

    return 0;
}
