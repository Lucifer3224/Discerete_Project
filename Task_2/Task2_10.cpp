#include <iostream>
#include <sstream>
#include <string>
#include <stack>
#include <vector>
using namespace std;

/*The evaluateExpression function parses and evaluates a logical expression containing T, F, AND, OR, NOT, and parentheses.
It uses two stacks, one for boolean values (T/F) and another for operators, to process the expression. Tokens are read sequentially,
and operators are applied following precedence rules, with parentheses handled explicitly.
After all tokens are processed and remaining operators applied, the function returns the final boolean result of the evaluated expression.*/


// Function to evaluate the logical expression
bool evaluateExpression(const string& expr) {
    istringstream iss(expr);
    string token;
    stack<bool> values;  // Stack to hold boolean values (T/F)
    stack<string> ops;   // Stack to hold operators (AND, OR, NOT)

    auto applyOperator = [&]() {
        if (ops.top() == "NOT") {  // Handle NOT operator
            ops.pop();
            bool value = values.top();
            values.pop();
            values.push(!value);
        }
        else {  // Handle AND/OR operators
            string op = ops.top();
            ops.pop();
            bool right = values.top();
            values.pop();
            bool left = values.top();
            values.pop();

            if (op == "AND") {
                values.push(left && right);
            }
            else if (op == "OR") {
                values.push(left || right);
            }
        }
        };
    while (iss >> token) {  // Tokenize and evaluate the expression
        if (token == "T" || token == "F") {    
            values.push(token == "T"); //push 1 ->"T"   0->"F"
        }
        else if (token == "AND" || token == "OR") { 
            while (!ops.empty() && ops.top() != "(" &&
                (ops.top() == "NOT" || ops.top() == "AND" || ops.top() == "OR")) {
                applyOperator();
            }
            ops.push(token);
        }
        else if (token == "NOT") {
            ops.push(token);
        }
        else if (token == "(") { // Handle opening parenthesis
            ops.push(token);
        }
        else if (token == ")") { // Handle closing parenthesis
            while (!ops.empty() && ops.top() != "(") {
                applyOperator();
            }
            ops.pop(); // Remove the '('
        }
    }
    while (!ops.empty()) {
        applyOperator();
    }
    return values.top();
}



/*
The printTruthTable function loops through all combinations and prints the input variables along with their corresponding outputs.
*/

void printTruthTable(vector<char> A, vector<char> B, vector<char> C, vector<bool> RE, string expression) {
    // Print truth table
    cout << "A  B  C  " << expression << "\n";
    for (int i = 0; i < 8; i++) {   
        cout << A[i] << "  " << B[i] << "  " << C[i] << "  " << (RE[i] ? "T" : "F") << "\n";
    }
}

/*
The calc_truth_table function loops through all combinations. In each loop,
it replaces the values of the inputs in the string, calls the evaluateExpression function,
and saves the output in an array named results. After processing all combinations, it returns the results array.
*/

// Iterate through all combinations of truth values
vector<bool> calc_truth_table(vector<char> A, vector<char> B, vector<char> C, vector<char> T, vector<char> F, string expression) {
    string modifiedExpression;
    vector<bool> results = { 0,0,0,0,0,0,0,0 };
    for (int i = 0; i < 8; i++) {
        modifiedExpression = expression;
        for (size_t x = 0; x < modifiedExpression.size(); x++) {
            if (modifiedExpression[x] == 'A') {
                if (modifiedExpression[x + 1] == ' ')
                    modifiedExpression[x] = A[i];
            }
            else if (modifiedExpression[x] == 'T') {
                modifiedExpression[x] = T[i];
            }
            else if (modifiedExpression[x] == 'F') {
                modifiedExpression[x] = F[i];
            }
            else if (modifiedExpression[x] == 'B') {
                modifiedExpression[x] = B[i];
            }
            else if (modifiedExpression[x] == 'C') {
                modifiedExpression[x] = C[i];
            }
        }
        // Print the modified expression
        results[i] = evaluateExpression(modifiedExpression);
    }
    return results;
}



// Function to check if two truth tables are equivalent
bool areEquivalent(const vector<bool>& table1, const vector<bool>& table2) {
    return table1 == table2; // Verify whether all elements in two arrays are identical.
}



// Function to find satisfiable inputs
bool findSatisfiableInputs(const vector<bool>& results1, const vector<bool>& results2, const vector<char>& A, const vector<char>& B, const vector<char>& C) {
    int flag = 0;    //change if find satisfiable 
    for (int i = 0; i < 8; i++) {
        if (results1[i] && results2[i]) {
            flag = 1;
            cout << "Satisfiable inputs: A = " << A[i] << ", B = " << B[i] << ", C = " << C[i] << "\n";
           
        }
    }if (flag == 1) {
        cout << "2 Expressions are satisfiable\n";
        return 1;
    }else
    return 0;
}


//A tautology occurs when all elements in the array are 1.

bool IsTautology(const vector<bool>& table1) {
    int tautology = 0;
    for (int i = 0; i < 8;i++) {
        if (table1[i] == 1) {
            tautology++;
        }
        else
            return 0;  // Return 0 and consider it not a tautology if any element is not 1.
    }return 1;         // Return 1 after finishing the loop if all elements are 1.
}
bool IsUnsatisfiable(const vector<bool>& table1) {
    int Unsatisfiable = 0;
    for (int i = 0; i < 8;i++) {
        if (table1[i] == 1) {
            return 0;    // Return 0 and consider it not a Unsatisfiable if any element is 1.
        }

    }return 1;          // Return 1 after finishing the loop if all elements are 0.

}

/*Modify one gate to make the expression not a Tautology and satisfiable.
Iterate through the expression. If you encounter 'AND', replace it with 'OR' and recalculate the truth table. If the modified expression becomes not a Tautology and satisfiable, return it.
Repeat the same process for 'OR'. If the change does not result in the desired outcome, revert it and proceed with further modifications.
Once all 'AND' and 'OR' combinations are processed, iterate through the expression again. If you encounter 'NOT', remove it and recalculate the truth table. If the modified expression meets the criteria, return it.
If not, undo the change and continue removing additional 'NOT' gates.
*/

// Modify the logical expression to make it satisfiable
string modifyExpression(vector<bool>& results1, const vector<char>& A, const vector<char>& B, const vector<char>& C, const vector<char>& T, const vector<char>& F, string expr) {
    string mod = expr;
    string OR = "OR";
    string AND = "AND";
    int flag ;
    int no_gate_and_or = 0;
    int no_not_gate = 0;
    // Count the number of AND and OR gates in the expression and track the number of transitions between AND and OR gates.
    //cout number of NOT gate in the expression and track the number of transitions between NOT and BUFFER gates.
    for (int i = 0; i < expr.size();i++) {
        if ((expr[i] == 'A' && expr[i + 1] == 'N') || (expr[i] == 'O' && expr[i + 1] == 'R'))
            no_gate_and_or++;
        else if (expr[i] == 'N' && expr[i + 1] == 'O' && expr[i+2] == 'T') {
            no_not_gate++;
        }
    }

    int i = 0;
    flag = 0;
    for (int y = 0;y < no_gate_and_or;y++) {     //track the number of transitions between AND and OR gates
        for (i;i < mod.size();i++) {             // I avoided initializing i to 0 in the loop because the next iteration does not begin at the first index of the string.
            if (expr[i] == 'A' && expr[i + 1] == 'N')  // "Change 'AND' to 'OR'.
            {
                mod.replace(i, 3, OR);
                i = i + 1;
                flag = 1;     //update flag
            }
            else if (expr[i] == 'O' && expr[i + 1] == 'R' ) {  // "Change 'OR' to 'AND' .
                mod.replace(i, 2 , AND);
                i = i + 2;
                flag = 1;     //update flag
            }

            if (flag == 1) {  //If the flag is set, verify Tautology and Unsatisfiable conditions. If either is true, return.
                results1 = calc_truth_table(A, B, C, T, F, mod);
                if (IsTautology(results1) == 0 && IsUnsatisfiable(results1) == 0)
                    return mod;
            }
            else mod = expr;  //make the mod equal expr
        }
    }


    i = 0;
    flag = 0;
    for (int y = 0;y < no_not_gate;y++) {
        for (i;i < mod.size();i++) {
            if (expr[i] == 'N' && expr[i + 1] == 'O' && expr[i + 2] == 'T') {
                mod.erase(i, 4);   //remove NOT
                i = i + 3;
                flag = 1;
            }
            if (flag == 1) {
                results1 = calc_truth_table(A, B, C, T, F, mod);

                if (IsTautology(results1) == 0 && IsUnsatisfiable(results1) == 0) {
                    cout << "TO  Modify Expression ,Should Remove NOT gate : " << mod;
                    return mod;
                }
            }
            else {
                flag = 0;
                mod = expr;
            }
        }
    }

    return mod;
}

//Checking Tautology and Unsatisfiable for logical expression
string check(vector<bool> results1, const vector<char>& A, const vector<char>& B, const vector<char>& C, const vector<char>& T, const vector<char>& F, string OriginalExpr, string& modifiedExpression) {

    cout << "Checking Tautology and Unsatisfiable for logical expression\n";
    if (IsTautology(results1)) {
        cout << "Expression is tautology.\n";
        modifiedExpression = modifyExpression(results1, A, B, C, T, F, OriginalExpr);
      
    }
    else {
        cout << "Expression is not tautology.\n";
    }

    if (IsUnsatisfiable(results1))
    {
        cout << "Expression is Unsatisfiable.\n";
        modifiedExpression = modifyExpression(results1, A, B, C, T, F, OriginalExpr);

    }
    else cout << "Expression is satisfiable";

    return modifiedExpression;
}


int main() {

    // Define truth values for P, Q, S
    vector<char> A = { 'F', 'T', 'F', 'T', 'F', 'T', 'F', 'T' };
    vector<char> B = { 'F', 'F', 'T', 'T', 'F', 'F', 'T', 'T' };
    vector<char> C = { 'F', 'F', 'F', 'F', 'T', 'T', 'T', 'T' };
    vector<char> T = { 'T', 'T', 'T', 'T', 'T', 'T', 'T', 'T' };
    vector<char> F = { 'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F' };
    vector<bool> results1 = { 0, 0, 0, 0, 0, 0, 0, 0 };
    vector<bool> results2 = { 0, 0, 0, 0, 0, 0, 0, 0 };
    string OriginalExpr;
    string simplifiedExpr;
    string modifiedExpression = "";
    string simplifiedModifiedexpr;
    // Get the expression input
    cout << "Hello, when entering an expression,\nuse capital letters {A,B,C} and ensure spaces between characters and brackets.\nMake sure to add a space between any of these elements: 'A', 'B', 'C', '(', ')', 'AND', 'OR', 'NOT'.\nUse 'T' if all combinations are true  \nUse 'F' if all combinations are false.\nFor example : '( ( A AND B ) OR NOT C )' ";
    cout << "\n----------------------------------------" << "\n";
    cout << "Enter the Original logical expression : ";
    getline(cin, OriginalExpr);
    cout << "Enter the simplified logical expression : ";
    getline(cin, simplifiedExpr);
    OriginalExpr = OriginalExpr.append(" ");   //If the last character of variable A in this code is 'A ', I handle it as such to distinguish it from 'AND' 
    simplifiedExpr = simplifiedExpr.append(" ");

    // Evaluate truth tables
    results1 = calc_truth_table(A, B, C, T, F, OriginalExpr);
    results2 = calc_truth_table(A, B, C, T, F, simplifiedExpr);

    // Print truth tables
    printTruthTable(A, B, C, results1, OriginalExpr);
    cout << "----------------------------------------" << "\n";
    printTruthTable(A, B, C, results2, simplifiedExpr);

    // Check equivalence
    if (areEquivalent(results1, results2))
        cout << "Two expressions are Equivalent \n";
    else
        cout << "Two expressions are not Equivalent \n";


    cout << "----------------------------------------" << "\n";

    cout << "Checking satisfiability for both expressions\n";
    if (!findSatisfiableInputs(results1, results2, A, B, C)) {
        cout << "2 Expression is unsatisfiable. \n";
    }

    cout << "----------------------------------------\n";
    cout << "Original Expression : " << OriginalExpr << "\n";
    modifiedExpression = check(results1, A, B, C, T, F, OriginalExpr, modifiedExpression);

    if (modifiedExpression != "") {
        cout << "\n-----------------------------------------\n";
        cout << "modified Expression : " << modifiedExpression << "\n";
        results1 = calc_truth_table(A, B, C, T, F, modifiedExpression);
        check(results1, A, B, C, T, F, modifiedExpression, modifiedExpression);
        cout << "\n----------------------------------------" << "\n";
        cout << "Enter the simplified modified logical expression : ";
        getline(cin, simplifiedModifiedexpr);
        simplifiedModifiedexpr = simplifiedModifiedexpr.append(" ");  //If the last character of variable A in this code is 'A ', I handle it as such to distinguish it from 'AND'
        // Evaluate truth tables
        results2 = calc_truth_table(A, B, C, T, F, simplifiedModifiedexpr);

        // Print truth tables
        printTruthTable(A, B, C, results1, modifiedExpression);
        cout << "----------------------------------------" << "\n";
        printTruthTable(A, B, C, results2, simplifiedModifiedexpr);

        // Check equivalence
        if (areEquivalent(results1, results2))
            cout << "Two expressions are Equivalent \n";
        else
            cout << "Two expressions are not Equivalent \n";
        cout << "----------------------------------------" << "\n";

        cout << "Checking satisfiability for both expressions\n";
        if (!findSatisfiableInputs(results1, results2, A, B, C)) {
            cout << "2 Expression is unsatisfiable. \n";
    }

    
    }
  

    // OriginalExpr: NOT ( ( A AND ( B AND C ) ) AND ( NOT C AND ( A AND B ) ) )
    //simplifiedExpr: T
    //simplifiedModified: NOT A OR NOT B OR C 

    return 0;
}