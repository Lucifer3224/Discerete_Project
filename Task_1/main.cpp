#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <stack>

// ANSI Color Codes for Terminal Output
const std::string RESET = "\033[0m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string BOLD = "\033[1m";
const std::string CYAN = "\033[36m";

// Logical Operations Class
class LogicalOperations {
public:
    // Logical NOT operation
    static bool logicalNot(bool x) {
        return !x;
    }

    // Logical AND operation
    static bool logicalAnd(bool x, bool y) {
        return x && y;
    }

    // Logical OR operation
    static bool logicalOr(bool x, bool y) {
        return x || y;
    }

    // Logical Conditional operation
    static bool logicalImplies(bool x, bool y) {
        return !x || y;
    }

    // Logical BICONDITIONAL operation
    static bool logicalBiconditional(bool x, bool y) {
        return x == y;
    }
};

// Expression Parsing and Evaluation Class
class LogicalExpressionEvaluator {
private:
    std::map<char, bool> variableValues;

    // Check if character is an operator
    bool isOperator(char c) {
        return c == '&' || c == '|' || c == '~' || c == '-' || c == '<';
    }

    // Precedence of operators
    int getPrecedence(char op) {
        if (op == '~') return 4;  // Highest precedence for NOT
        if (op == '&') return 3;  // AND has higher precedence than OR
        if (op == '|') return 2;
        if (op == '-') return 1;  // Conditional
        if (op == '<') return 0;  // BICONDITIONAL
        return -1;
    }

    // Evaluate single operation
    bool applyOperator(char op, bool b1, bool b2 = false) {
        switch(op) {
            case '~': return LogicalOperations::logicalNot(b1);
            case '&': return LogicalOperations::logicalAnd(b1, b2);
            case '|': return LogicalOperations::logicalOr(b1, b2);
            case '-': return LogicalOperations::logicalImplies(b1, b2);
            case '<': return LogicalOperations::logicalBiconditional(b1, b2);
            default: return b1;
        }
    }

public:
    // Reset variable values
    void resetVariables() {
        variableValues.clear();
    }

    // Set variable values for evaluation
    void setVariable(char var, bool value) {
        variableValues[var] = value;
    }

    // Evaluate a logical expression
    bool evaluate(const std::string& expression) {
        std::stack<bool> values;
        std::stack<char> ops;
        
        for (size_t i = 0; i < expression.length(); i++) {
            // Skip whitespace
            if (expression[i] == ' ') continue;

            // Handle opening parenthesis
            if (expression[i] == '(') {
                ops.push(expression[i]);
                continue;
            }

            // Handle closing parenthesis
            if (expression[i] == ')') {
                while (!ops.empty() && ops.top() != '(') {
                    char op = ops.top();
                    ops.pop();

                    // Unary NOT
                    if (op == '~') {
                        bool val = values.top();
                        values.top() = applyOperator(op, val);
                    }
                    // Binary operators
                    else {
                        bool val2 = values.top();
                        values.pop();
                        bool val1 = values.top();
                        values.pop();
                        values.push(applyOperator(op, val1, val2));
                    }
                }
                ops.pop(); // Remove the opening parenthesis
                continue;
            }

            // Handle negation (unary operator)
            if (expression[i] == '~') {
                ops.push(expression[i]);
                continue;
            }

            // If it's a variable
            if (std::isalpha(expression[i])) {
                bool value = variableValues[expression[i]];
                values.push(value);

                // Apply any pending negation
                while (!ops.empty() && ops.top() == '~') {
                    value = applyOperator('~', value);
                    values.top() = value;
                    ops.pop();
                }
            }
            // If it's an operator
            else if (isOperator(expression[i])) {
                // Handle Conditional and biconditional
                if (expression[i] == '-' && i + 1 < expression.length() && expression[i + 1] == '>') {
                    ops.push('-');
                    i++;
                } else if (expression[i] == '<' && i + 2 < expression.length() && expression[i + 1] == '-' && expression[i + 2] == '>') {
                    ops.push('<');
                    i += 2;
                } else {
                    // Process operators of higher or equal precedence
                    while (!ops.empty() && getPrecedence(ops.top()) >= getPrecedence(expression[i])) {
                        char op = ops.top();
                        ops.pop();

                        // Unary NOT
                        if (op == '~') {
                            bool val = values.top();
                            values.top() = applyOperator(op, val);
                        }
                        // Binary operators
                        else {
                            bool val2 = values.top();
                            values.pop();
                            bool val1 = values.top();
                            values.pop();
                            values.push(applyOperator(op, val1, val2));
                        }
                    }
                    ops.push(expression[i]);
                }
            }
        }

        // Process remaining operators
        while (!ops.empty()) {
            char op = ops.top();
            ops.pop();

            // Unary NOT
            if (op == '~') {
                bool val = values.top();
                values.top() = applyOperator(op, val);
            }
            // Binary operators
            else {
                bool val2 = values.top();
                values.pop();
                bool val1 = values.top();
                values.pop();
                values.push(applyOperator(op, val1, val2));
            }
        }

        return values.top();
    }

    // Extract unique variables from expressions
    std::vector<char> extractVariables(const std::vector<std::string>& expressions) {
        std::vector<char> variables;
        for (const auto& expr : expressions) {
            for (char c : expr) {
                if (std::isalpha(c) && 
                    std::find(variables.begin(), variables.end(), c) == variables.end()) {
                    variables.push_back(c);
                }
            }
        }
        std::sort(variables.begin(), variables.end());
        return variables;
    }
};

// Truth Table Generator and Analyzer
class LogicalAnalyzer {
private:
    LogicalExpressionEvaluator evaluator;

    // Print table header
    void printHeader(const std::vector<char>& variables, 
                     const std::vector<std::string>& premises, 
                     const std::string& conclusion) {
        // Print variable columns
        for (char var : variables) {
            std::cout << BOLD << std::setw(6) << var << " " << RESET;
        }

        // Print premises columns
        for (size_t i = 0; i < premises.size(); ++i) {
            std::cout << BOLD << std::setw(12) << "Premise" + std::to_string(i+1) << " " << RESET;
        }

        // Print conclusion column
        std::cout << BOLD << std::setw(12) << "Conclusion" << RESET << std::endl;

        // Print separator
        for (size_t i = 0; i < variables.size() + premises.size() + 1; ++i) {
            std::cout << std::string(15, '-');
        }
        std::cout << std::endl;
    }

public:
    // Generate and analyze truth table
    void generateTruthTable(const std::vector<std::string>& premises, 
                             const std::string& conclusion) {
        // Extract variables
        std::vector<char> variables = evaluator.extractVariables(premises);
        
        // Add variables from conclusion if not already present
        std::vector<char> conclusionVars = evaluator.extractVariables({conclusion});
        for (char var : conclusionVars) {
            if (std::find(variables.begin(), variables.end(), var) == variables.end()) {
                variables.push_back(var);
            }
        }
        std::sort(variables.begin(), variables.end());

        // Print header
        printHeader(variables, premises, conclusion);

        // Track satisfiability and validity
        bool isSatisfiable = false;
        bool isValid = true;
        bool isFalsifiable = false;
        std::vector<int> satisfiableRows;
        std::vector<int> falsifiableRows;

        // Generate all possible truth value combinations
        int numCombinations = std::pow(2, variables.size());
        for (int i = 0; i < numCombinations; ++i) {
            // Reset variables for this iteration
            evaluator.resetVariables();

            // Set variable values based on binary representation
            for (size_t j = 0; j < variables.size(); ++j) {
                bool value = (i & (1 << j)) != 0;
                evaluator.setVariable(variables[j], value);
            }

            // Evaluate premises and conclusion
            bool allPremisesTrue = true;
            bool conclusionTrue = evaluator.evaluate(conclusion);

            std::vector<bool> premiseResults;
            for (const auto& premise : premises) {
                bool premiseResult = evaluator.evaluate(premise);
                premiseResults.push_back(premiseResult);
                
                if (!premiseResult) {
                    allPremisesTrue = false;
                }
            }

            // Print truth values
            for (char var : variables) {
                std::cout << BOLD << std::setw(6) << (evaluator.evaluate(std::string(1, var)) ? "T" : "F") << " " << RESET;
            }

            // Print premise values
            for (bool premiseResult : premiseResults) {
                std::cout << BOLD << std::setw(12) << (premiseResult ? "T" : "F") << " " << RESET;
            }

            // Print conclusion with color coding
            if (conclusionTrue) {
                std::cout << GREEN << BOLD << std::setw(12) << "T" << RESET;
            } else {
                std::cout << RED << BOLD << std::setw(12) << "F" << RESET;
            }

            // Update satisfiability and validity
            if (allPremisesTrue) {
                if (conclusionTrue) {
                    isSatisfiable = true;
                    satisfiableRows.push_back(i + 1);
                } else {
                    isFalsifiable = true;
                    isValid = false;
                    falsifiableRows.push_back(i + 1);
                }
                std::cout << CYAN << BOLD << " <-- Critical Row" << RESET;
            }
            std::cout << std::endl;
        }

        // Print analysis results
        std::cout << "\n" << YELLOW << BOLD << "Analysis Results:" << RESET << std::endl;
        std::cout << "Satisfiability: " << (isSatisfiable ? (GREEN + BOLD + "Satisfiable") : (RED + BOLD + "Not Satisfiable")) << RESET << std::endl;
        std::cout << "Validity: " << (isValid ? (GREEN + BOLD + "Valid") : (RED + BOLD + "Falsifiable")) << RESET << std::endl;

        // Print critical rows
        if (!satisfiableRows.empty()) {
            std::cout << "Rows causing satisfiability: ";
            for (int row : satisfiableRows) {
                std::cout << row << " ";
            }
            std::cout << std::endl;
        }

        if (!falsifiableRows.empty()) {
            std::cout << "Rows causing falsifiability: ";
            for (int row : falsifiableRows) {
                std::cout << row << " ";
            }
            std::cout << std::endl;
        }
    }
};

int main() {
    LogicalAnalyzer analyzer;
    int numPremises;
    
    std::cout << std::endl;
    std::cout << BLUE << BOLD << "Logical Expression Truth Table Analyzer" << RESET << std::endl;
    std::cout << MAGENTA << "Terms of Use: Use " << BOLD << "&" << RESET << MAGENTA << " for " << BOLD << "AND" << RESET << MAGENTA << ", " << BOLD << "|" << RESET << MAGENTA << " for " << BOLD << "OR" << RESET << MAGENTA << ", " << BOLD << "~" << RESET << MAGENTA << " for " << BOLD << "NOT" << RESET << MAGENTA << ", " << BOLD << "->" << RESET << MAGENTA << " for " << BOLD << "Conditional" << RESET << MAGENTA << ", " << BOLD << "<->" << RESET << MAGENTA << " for " << BOLD << "BICONDITIONAL" << RESET << MAGENTA << "." << RESET << std::endl;
    
    // Get number of premises
    std::cout << CYAN << BOLD << "Number of premises: " << RESET;
    std::cin >> numPremises;
    std::cin.ignore(); // Clear input buffer

    // Store premises
    std::vector<std::string> premises;
    for (int i = 0; i < numPremises; ++i) {
        std::string premise;
        std::cout << CYAN << BOLD << "Premise " << (i+1) << ": " << RESET;
        std::getline(std::cin, premise);
        premises.push_back(premise);
    }

    // Get conclusion
    std::string conclusion;
    std::cout << CYAN << BOLD << "Conclusion: " << RESET;
    std::getline(std::cin, conclusion);
    std::cout << std::endl;

    // Generate truth table
    analyzer.generateTruthTable(premises, conclusion);

    return 0;
}