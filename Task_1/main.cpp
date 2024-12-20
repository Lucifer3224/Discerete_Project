/**
 * @file main.cpp
 * @brief logical expression analyzer that generates truth tables and analyzes logical validity and satisfiability.
 * 
 * Checking for valid and satisfiable logical expressions at line 415 :D
 * 
 */

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <stack>
#include <bitset> 
#include <unordered_set> // For unique variables
#include <algorithm> // For std::sort
#include <iomanip> // For std::setw

// Constants for terminal colors
namespace Colors {
    const std::string RESET = "\033[0m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string BOLD = "\033[1m";
}

/**
 * @class ExpressionTokenizer
 * @brief Handles the tokenization of logical expressions into their component parts
 */
class ExpressionTokenizer {
public:
    /**
     * @brief Token types for logical expressions
     */
    enum class TokenType {
        VARIABLE,
        OPERATOR,
        PARENTHESIS,
        INVALID
    };

    /**
     * @brief Structure to represent a token in the expression
     */
    struct Token {
        TokenType type;
        char value;
        int precedence; // Operator precedence, -1 for non-operators
    };

private:
    // Operator precedence map for tokenization
    const std::map<char, int> operatorPrecedence = {
        {'~', 4},  // NOT
        {'&', 3},  // AND
        {'|', 2},  // OR
        {'-', 1},  // IMPLIES
        {'<', 0}   // BICONDITIONAL
    };

public:
    /**
     * @brief Tokenizes an input expression into a vector of tokens
     * @param expression The logical expression to tokenize
     * @return Vector of tokens
     */
    std::vector<Token> tokenize(const std::string& expression) {
        std::vector<Token> tokens;
        
        for (size_t i = 0; i < expression.length(); ++i) {
            char c = expression[i];
            
            // Skip whitespace
            if (std::isspace(c)) continue;

            // Handle special operators 
            if (c == '-' && i + 1 < expression.length() && expression[i + 1] == '>') {
                // This line adds a new Token to the 'tokens' vector. The Token is created using an initializer list
                // with three elements: the type of the token (TokenType::OPERATOR), the character representing the 
                // operator ('-'), and the precedence of the operator (retrieved from the 'operatorPrecedence' map).
                tokens.push_back({TokenType::OPERATOR, '-', operatorPrecedence.at('-')}); // Implication '->'
                ++i;
                continue;
            }
            if (c == '<' && i + 2 < expression.length() && expression[i + 1] == '-' && expression[i + 2] == '>') {
                // This line adds a new Token to the 'tokens' vector. The Token is created using an initializer list
                // with three elements: the type of the token (TokenType::OPERATOR), the character representing the
                // operator ('<'), and the precedence of the operator (retrieved from the 'operatorPrecedence' map).
                tokens.push_back({TokenType::OPERATOR, '<', operatorPrecedence.at('<')}); // Biconditional '<->'
                i += 2;
                continue;
            }

            // Categorize token
            Token token;
            if (std::isalpha(c)) {
                token = {TokenType::VARIABLE, c, -1};
            } else if (c == '(' || c == ')') {
                token = {TokenType::PARENTHESIS, c, -1};
            } else if (operatorPrecedence.count(c)) {
                token = {TokenType::OPERATOR, c, operatorPrecedence.at(c)};
            } else {
                token = {TokenType::INVALID, c, -1};
            }

            tokens.push_back(token);
        }

        return tokens;
    }
};

/**
 * @class LogicalEvaluator
 * @brief Handles the evaluation of logical expressions using tokens
 */
class LogicalEvaluator {
private:
    std::map<char, bool> variableValues; // Map to store variable values
    ExpressionTokenizer tokenizer; // Tokenizer to break down expressions

    /**
     * @brief Evaluates a single logical operation
     * @param op The operator character
     * @param a The first operand
     * @param b The second operand (optional for unary operators)
     * @return The result of the logical operation
     */
    bool evaluateOperation(char op, bool a, bool b = false) {
        switch (op) {
            case '~': return !a;       // NOT operation
            case '&': return a && b;   // AND operation
            case '|': return a || b;   // OR operation
            case '-': return !a || b;  // Implication (a -> b)
            case '<': return a == b;   // Biconditional (a <-> b)
            default: return false;     // Invalid operator
        }
    }

public:
    /**
     * @brief Sets the value of a variable
     * @param var The variable character
     * @param value The boolean value to set
     */
    void setVariable(char var, bool value) {
        variableValues[var] = value;
    }

    /**
     * @brief Evaluates a logical expression using infix notation and operator precedence.
     * 
     * This function tokenizes the input logical expression and evaluates it using a stack-based
     * approach to handle operator precedence and parentheses. It supports variables, logical
     * operators (NOT, AND, OR, IMPLIES, BICONDITIONAL), and parentheses for grouping.
     * 
     * The evaluation process involves:
     * - Tokenizing the expression into variables, operators, and parentheses.
     * - Using a value stack to store boolean values of variables.
     * - Using an operator stack to manage operators and parentheses.
     * - Processing operators based on their precedence and evaluating sub-expressions.
     * - Handling parentheses by processing until matching '(' is found.
     * - Returning the final result of the logical expression.
     * - Throwing a runtime error for invalid tokens or malformed expressions.
     * 
     * @param expression The logical expression to evaluate.
     * @return The boolean result of the evaluation.
     * @throws std::runtime_error If an invalid token is encountered or the expression is malformed.
     */
    bool evaluate(const std::string& expression) {
        // Tokenize the expression into a vector of tokens
        auto tokens = tokenizer.tokenize(expression);
        
        // Stack to store operand values (true/false)
        std::stack<bool> valueStack;
        
        // Stack to store operators and parentheses
        std::stack<ExpressionTokenizer::Token> operatorStack;

        /**
         * @brief Iterate through each token in the tokenized expression.
         * - If the token is a variable, push its value onto the value stack.
         * - If the token is an operator, process operators with higher or equal precedence
         *   from the operator stack and then push the current operator onto the stack.
         * - If the token is a parenthesis, handle '(' by pushing it onto the operator stack,
         *   and handle ')' by processing until the matching '(' is found and then removing it.
         * - If an invalid token is encountered, throw a runtime error.
         */
        for (const auto& token : tokens) {
            switch (token.type) {
            case ExpressionTokenizer::TokenType::VARIABLE:
                // Push the value of the variable onto the value stack
                valueStack.push(variableValues[token.value]);
                break;

            case ExpressionTokenizer::TokenType::OPERATOR:
                // Process operators with higher or equal precedence
                while (!operatorStack.empty() && 
                   operatorStack.top().type == ExpressionTokenizer::TokenType::OPERATOR &&
                   operatorStack.top().precedence >= token.precedence) {
                // Evaluate the top operator in the stack
                processTopOperator(valueStack, operatorStack);
                }
                // Push the current operator onto the operator stack
                operatorStack.push(token);
                break;

            case ExpressionTokenizer::TokenType::PARENTHESIS:
                if (token.value == '(') {
                // Push '(' onto the operator stack
                operatorStack.push(token);
                } else {
                // Process until matching '(' is found
                while (!operatorStack.empty() && operatorStack.top().value != '(') {
                    // Evaluate the top operator in the stack
                    processTopOperator(valueStack, operatorStack);
                }
                if (!operatorStack.empty()) {
                    // Remove '(' from the stack
                    operatorStack.pop();
                }
                }
                break;

            default:
                // Throw an error if an invalid token is encountered
                throw std::runtime_error("Invalid token encountered");
            }
        }

        // Process any remaining operators in the stack
        while (!operatorStack.empty()) {
            // Evaluate the top operator in the stack
            processTopOperator(valueStack, operatorStack);
        }

        // Return the final result from the value stack
        return valueStack.empty() ? false : valueStack.top();
    }

private:
    /**
     * @brief Processes the top operator from the operator stack and applies it to the values in the value stack.
     *
     * This function pops the top operator from the operator stack and applies it to the values in the value stack.
     * If the operator is a unary operator (e.g., '~' for NOT), it pops one value from the value stack, applies the
     * operator, and pushes the result back onto the value stack.
     * If the operator is a binary operator (e.g., '&' for AND, '|' for OR), it pops two values from the value stack,
     * applies the operator, and pushes the result back onto the value stack.
     *
     * @param valueStack A stack of boolean values representing the operands.
     * @param operatorStack A stack of tokens representing the operators.
     */
    void processTopOperator(std::stack<bool>& valueStack,
                           std::stack<ExpressionTokenizer::Token>& operatorStack) {
        auto op = operatorStack.top();
        operatorStack.pop();

        if (op.value == '~') {
            if (valueStack.empty()) throw std::runtime_error("Invalid expression");
            bool val = valueStack.top();
            valueStack.pop();
            valueStack.push(evaluateOperation(op.value, val)); // Evaluate NOT operation
        } else {
            if (valueStack.size() < 2) throw std::runtime_error("Invalid expression");
            bool val2 = valueStack.top();
            valueStack.pop();
            bool val1 = valueStack.top();
            valueStack.pop();
            valueStack.push(evaluateOperation(op.value, val1, val2)); // Evaluate binary operation
        }
    }
};;

/**
 * @class TruthTableGenerator
 * @brief Generates and analyzes truth tables for logical expressions.
 * 
 * This class is responsible for generating truth tables for given logical expressions (premises and conclusion)
 * and analyzing their validity and satisfiability. It uses the LogicalEvaluator class to evaluate the expressions
 * for all possible combinations of variable values.
 */
class TruthTableGenerator {
private:
    LogicalEvaluator evaluator; // Object to evaluate logical expressions
    std::vector<char> variables; // List of unique variables in the expressions
    std::vector<std::string> premises; // List of premises (logical expressions)
    std::string conclusion; // Conclusion (logical expression)

    /**
     * @brief Extracts unique variables from expressions.
     * 
     * This function scans through the premises and conclusion to find all unique variables.
     * It stores these variables in a sorted vector for consistent processing.
     */
    void extractVariables() {
        std::unordered_set<char> variableSet; // Temporary set to store unique variables
        
        // Process premises to extract variables
        for (const auto& premise : premises) {
            for (char c : premise) {
                if (std::isalpha(c)) { // Check if the character is a letter
                    variableSet.insert(c); // Insert the variable into the set
                }
            }
        }
        
        // Process conclusion to extract variables
        for (char c : conclusion) {
            if (std::isalpha(c)) { // Check if the character is a letter
                variableSet.insert(c); // Insert the variable into the set
            }
        }

        // Convert the set to a sorted vector
        variables = std::vector<char>(variableSet.begin(), variableSet.end()); // Copy set to vector for sorting and indexing purposes
        std::sort(variables.begin(), variables.end()); // Sort the variables for consistent order and indexing
    }

    /**
     * @brief Prints the truth table header.
     * 
     * This function prints the header of the truth table, including the variable names,
     * premises, and conclusion. It formats the header for readability.
     * 
     */
    void printHeader() const {
        // Print variable columns
        for (char var : variables) {
            std::cout << Colors::BOLD << std::setw(6) << var << " " << Colors::RESET;
        }

        // Print premises and conclusion
        for (size_t i = 0; i < premises.size(); ++i) {
            std::cout << Colors::BOLD << std::setw(12) 
                     << "P" + std::to_string(i+1) << " " << Colors::RESET;
        }
        std::cout << Colors::BOLD << std::setw(12) << "Conclusion" << Colors::RESET << "\n";

        // Print separator line
        // Separator line size calculation based on columns and widths of variables, premises, and conclusion
        std::cout << std::string(12 * (variables.size() + premises.size() + 1), '-') << "\n";
    }

public:
    /**
     * @brief Constructor initializing with premises and conclusion.
     * 
     * This constructor initializes the premises and conclusion, and then extracts the variables.
     * 
     * @param p The list of premises (logical expressions).
     * @param c The conclusion (logical expression).
     */
    TruthTableGenerator(const std::vector<std::string>& p, const std::string& c)
        : premises(p), conclusion(c) {
        extractVariables(); // Extract unique variables from premises and conclusion
    }

    /**
     * @brief Generates and analyzes the truth table.
     * 
     * This function generates the truth table by evaluating all possible combinations of variable values.
     * It also analyzes the results to determine the validity and satisfiability of the logical expressions.
     * 
     * The process involves:
     * - Printing the header of the truth table.
     * - Iterating through all possible combinations of variable values.
     * - Evaluating each premise and the conclusion for each combination.
     * - Printing the results in the truth table.
     * - Analyzing the results to determine validity and satisfiability.
     */
    void generateAndAnalyze() {
        printHeader(); // Print the header of the truth table

        bool isValid = true; // Flag to check if the logical expressions are valid
        bool isSatisfiable = false; // Flag to check if the logical expressions are satisfiable
        // 1 << n is equivalent to 2^n, which is the total number of combinations for n variables
        const size_t combinations = 1 << variables.size(); // Total number of combinations (2^n) 

        // For each possible combination of variable values
        for (size_t i = 0; i < combinations; ++i) {
            // Set variable values based on the current combination
            for (size_t j = 0; j < variables.size(); ++j) {
                // Set the value of the j-th variable based on the j-th bit of the combination number 'i'
                // TODO: Explanation:
                // - 'i' represents the current combination of variable values in binary form.
                // - '(1 << j)' creates a mask with only the j-th bit set to 1.
                // - '(i & (1 << j))' checks if the j-th bit of 'i' is set to 1.
                // - If the j-th bit of 'i' is 1, the variable is set to true; otherwise, it is set to false.
                // Example:
                // - For i = 5 (binary 101), the 0th and 2nd bits are set to 1.
                // - This means variables[0] and variables[2] will be set to true, and variables[1] will be set to false.
                evaluator.setVariable(variables[j], (i & (1 << j)) != 0);
            }

            // Evaluate premises and conclusion
            bool allPremisesTrue = true; // Flag to check if all premises are true
            std::vector<bool> premiseResults; // Store results of premise evaluations

            for (const auto& premise : premises) {
                bool result = evaluator.evaluate(premise); // Evaluate the premise
                premiseResults.push_back(result); // Store the result
                allPremisesTrue &= result; // Update the flag
            }

            bool conclusionResult = evaluator.evaluate(conclusion); // Evaluate the conclusion

            // Print the row values in the truth table
            printTruthTableRow(i, premiseResults, conclusionResult, allPremisesTrue);

            // Update analysis based on the results
            if (allPremisesTrue) {
                if (conclusionResult) {
                    isSatisfiable = true; // If all premises are true and conclusion is true, it's satisfiable
                } else {
                    isValid = false; // If all premises are true and conclusion is false, it's invalid
                }
            }
        }

        // Print the analysis results
        printAnalysis(isValid, isSatisfiable);
    }

private:
    /**
     * @brief Prints a single row of the truth table.
     * 
     * This function prints the values of variables, premises, and conclusion for a specific combination.
     * 
     * @param combination The current combination of variable values.
     * @param premiseResults The results of evaluating the premises.
     * @param conclusionResult The result of evaluating the conclusion.
     * @param isCriticalRow Flag indicating if this row is critical (all premises true).
     */
    void printTruthTableRow(size_t combination, 
                           const std::vector<bool>& premiseResults,
                           bool conclusionResult,
                           bool isCriticalRow) const {
        // Print variable values
        for (size_t j = 0; j < variables.size(); ++j) {
            std::cout << Colors::BOLD << std::setw(6) 
                     << ((combination & (1 << j)) ? "T" : "F") << " " << Colors::RESET;
        }

        // Print premise results
        for (bool result : premiseResults) {
            std::cout << Colors::BOLD << std::setw(12) 
                     << (result ? "T" : "F") << " " << Colors::RESET;
        }

        // Print conclusion with color
        std::cout << (conclusionResult ? Colors::GREEN : Colors::RED)
                 << Colors::BOLD << std::setw(12) 
                 << (conclusionResult ? "T" : "F") << Colors::RESET;

        // Mark critical rows
        if (isCriticalRow) {
            std::cout << Colors::BLUE << " <== Critical Row" << Colors::RESET;
        }
        
        std::cout << "\n";
    }

    /**
     * @brief Prints the analysis results.
     * 
     * This function prints whether the logical expressions are valid and satisfiable.
     * 
     * @param isValid Flag indicating if the logical expressions are valid.
     * @param isSatisfiable Flag indicating if the logical expressions are satisfiable.
     */
    void printAnalysis(bool isValid, bool isSatisfiable) const {
        std::cout << "\n" << Colors::YELLOW << Colors::BOLD 
                 << "Analysis Results:" << Colors::RESET << "\n";
        
        std::cout << "Validity: " 
                 << (isValid ? Colors::GREEN : Colors::RED)
                 << Colors::BOLD 
                 << (isValid ? "Valid" : "Invalid")
                 << Colors::RESET << "\n";
        
        std::cout << "Satisfiability: "
                 << (isSatisfiable ? Colors::GREEN : Colors::RED)
                 << Colors::BOLD
                 << (isSatisfiable ? "Satisfiable" : "Unsatisfiable")
                 << Colors::RESET << "\n";
    }
};

/**
 * @brief Main function implementing the user interface
 */
int main() {
    try {
        std::cout << Colors::BLUE << Colors::BOLD 
                 << "\nLogical Expression Truth Table Analyzer\n" << Colors::RESET;

        // Display terms of use
        std::cout << Colors::MAGENTA << "Terms of Use:\n"
                  << "1. Ensure logical expressions are correctly formatted.\n"
                  << "2. The program supports variables (letters), logical operators (~, &, |, ->, <->), and parentheses.\n"
                  << Colors::RESET << "\n";

        // Get premises
        std::cout << "Number of premises: ";
        int numPremises;
        std::cin >> numPremises;
        std::cin.ignore();

        std::vector<std::string> premises;
        for (int i = 0; i < numPremises; ++i) {
            std::cout << "Premise " << (i+1) << ": ";
            std::string premise;
            std::getline(std::cin, premise);
            premises.push_back(premise);
        }

        // Get conclusion
        std::cout << "Conclusion: ";
        std::string conclusion;
        std::getline(std::cin, conclusion);

        // Generate and analyze truth table
        TruthTableGenerator generator(premises, conclusion);
        generator.generateAndAnalyze();

    } catch (const std::exception& e) {
        std::cerr << Colors::RED << "Error: " << e.what() << Colors::RESET << "\n";
        return 1;
    }

    return 0;
}