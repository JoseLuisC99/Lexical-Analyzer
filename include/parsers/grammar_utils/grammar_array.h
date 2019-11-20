/*!
 * @file grammar_array.h
 * @brief Class for storing parsed grammars
 * @details Class GrammarArray stores parsed grammars from GrammarParser
 */

#ifndef STRUCT_GRAMMARARRAY_H
#define STRUCT_GRAMMARARRAY_H

#include <string>
#include <map>
#include <set>
#include <iostream>
#include <vector>

namespace compiler::grammar {
/*!
 * @brief Class which stores a grammar.
 *
 * @details Class structure which represents grammars parsed by GrammarParser.
 * Used in @ref include/compiler/parser_algorithms "parser_algorithms" directory.
 */
    class GrammarArray {
    private:
        struct Bimap {
            std::map<std::pair<std::string, std::vector<std::string>>, int> index_getter;
            std::map<int, std::pair<std::string, std::vector<std::string>>> rule_getter;

            void insert(const std::pair<std::string, std::vector<std::string>> &rule_input, int index) {
                index_getter[rule_input] = index;
                rule_getter[index] = rule_input;
            }

            void insert(int index, const std::pair<std::string, std::vector<std::string>> &rule_input) {
                index_getter[rule_input] = index;
                rule_getter[index] = rule_input;
            }

            [[nodiscard]] int size() const { return rule_getter.size(); }

            std::pair<std::string, std::vector<std::string>> &operator[](int index) {
                return rule_getter[index];
            }

            int operator[](const std::pair<std::string, std::vector<std::string>> &rule) {
                return index_getter[rule];
            }

            const std::pair<std::string, std::vector<std::string>> &operator[](int index) const {
                return rule_getter.at(index);
            }

            int operator[](const std::pair<std::string, std::vector<std::string>> &rule) const {
                return index_getter.at(rule);
            }
        };

        std::string axiom_; //!< Axiom of the grammar stored.
        /*!<
         * Variable which generates every string accepted by the parsed grammar.
         */

        std::map<std::string, std::set<std::vector<std::string> > > rules_array_; //!< Stores every rule in the grammar.
        /*!< It uses a map to optimize the acquirement of rules generated by the variables,
         * and a set to obtain every rule associated with the variable which we want to get its rules.*/

        std::set<std::string> non_terminals_; //!< Set of every non_terminal found in the grammar.

        std::set<std::string> terminals_; //!< Set of every variable found in the grammar.

        Bimap index_rule_; //!< Rule indexer for #rules_array_.
        /*!< Saves every rule found in #rules_array_ and assigns them a unique index. Used in class LL1. */



        /*!
         * @brief Calculates the structure of the variable in @p expression.
         * @details Calculates only the follow of expressions found in #terminals or #non_terminals_, or the follow of the
         * special string "#" (epsilon).
         * @param variable Variable or terminal symbol to which we want to calculate its follow.
         * @param calculated Helper set that saves every expression already analyzed by follow, to ensure the method
         * doesn't enter in a infinite loop.
         * @return A set containing every non terminal symbol that is in the follow of @p expression.
         */
        std::set<std::string> Follow(const std::string &variable, std::set<std::string> &calculated);

        /*!
         * @brief Determines if @p variable expression can generate from its rules the special string "#" (epsilon).
         * @param variable String which we want to determine if it can generate epsilon.
         * @return True if @p variable can generate epsilon. Else, false.
         */
        bool canGenerateEpsilon(const std::string &variable);

        bool canGenerateEpsilon(const std::string &variable, std::set<std::string> &calculated);

        /*!
         * @brief Updates #terminals_ every time a new rule is inserted.
         * @details Searches for every string not in #non_terminals_ for every rule in #rules_array_. If a symbol
         * is not in #non_terminals_ and the symbol is not "#" (epsilon), the method stores it in #terminals_ for future uses.
         */
        void UpdateTerminals();

        /*!
         * @brief Gets every rule that contains @p variable in its right side.
         * @param variable Symbol which we want to find in every right side of the grammar rules.
         * @return A set containing every rule that contains @p variable in its right side.
         */
        std::set<std::pair<std::string, std::vector<std::string>>> GetRightSides(const std::string &variable);

    public:

        /*!
         * @brief Default constructor for the class GrammarArray.
         * @details Creates a new GrammarArray without any rules, terminal symbols or non terminal symbols.
         */
        GrammarArray();

        /*!
         * @brief Inserts a new rule in #rules_array_.
         * @attention This method will @b ALWAYS set #axiom_ to the first rule inserted in #rules_array_.
         * @param variable String that generates the expression string @p rule .
         * @param rule Succession of strings which define the rule generated by @p variable .
         */
        void InsertRule(const std::string &variable, const std::vector<std::string> &rule);

        /*!
         * @brief Calculates the first of the expression string defined by @p expression_vector.
         * @param expression_vector Succession of strings defining a expression string which we want to get its first.
         * @return A set containing every non terminal symbol that is in the first of @p expression_vector.
         */
        std::set<std::string> First(const std::vector<std::string> &expression_vector);

        /*!
         * @brief Calculates the first of the expression string defined by @p expression.
         * @details It differs from First(const std::vector<std::string>& expression_vector) in where this method only
         * calculates the first of terminal and non terminal symbols + "#" (epsilon).
         * @param expression Symbol for which we want to calculate its first.
         * @return A set containing every non terminal symbol that is in the first of @p expression.
         */
        std::set<std::string> First(const std::string &expression, std::set<std::string> &calculated);

        /*!
         * @brief Calculates the follow of the variable defined by @p expression.
         * @warning Undefined behavior if @p variable is not in #non_terminals_.
         * @param variable Symbol for which we want to calculate its follow.
         * @return A set containing every non terminal symbol that is in the follow of @p expression.
         */
        std::set<std::string> Follow(const std::string &variable);

        /*!
         * @brief Gets the rules generated by @p variable.
         * @param variable Variable which we want to find its rules.
         * @return A set containing every rule generated by @p variable.
         */
        std::set<std::vector<std::string>> GetVariableRules(const std::string &variable);

        /*!
         * @brief Gets the index of the rule associated with @p variable and @p rule .
         * @param variable Variable that generates @p rule .
         * @param rule Rule generated by @p variable .
         * @return The index of the rule associated with @p variable and @p rule in #rules_index.
         */
        int GetRuleIndex(const std::string &variable, const std::vector<std::string> &rule);

        /*!
         * @brief Converts the grammar to an augmented grammar.
         * @details Adds a new rule @p new_axiom -> #axiom_ to convert the grammar into an augmented grammar.
         * @param new_axiom If @p new_axiom is empty, the method will set #axiom_ to axiom_P.
         * @attention If axiom_P is in #terminals_, the method will set #axiom_ to any "axiom_P____..." that is not in #terminals_,
         * appending a '_' char at the end of @p new_axiom until it's not in #terminals. The same applies if @p new_axiom is not empty.
         */
        void ToAugmentedGrammar(std::string new_axiom = "");

        /*!
         * @brief Constructs a grammar equivalent to the augmented grammar of this grammar.
         * @details Adds a new rule @p new_axiom -> #axiom_ to convert the grammar into an augmented grammar.
         * @attention This does @b NOT modify the GrammarArray from where is called.
         * @param new_axiom String to set #axiom_ to. If @p new_axiom is empty, the method will set #axiom_ to axiom_P.
         * @attention If axiom_P is in #terminals_, the method will set #axiom_ to any "axiom_P____..." that is not in #terminals_,
         * appending a '_' char at the end of @p new_axiom until it's not in #terminals. The same applies if @p new_axiom is not empty.
         * @return A new augmented grammar created from the grammar this method is called from.
         */
        GrammarArray GetAugmentedGrammar(std::string new_axiom = "");


        /*!
         * @brief Getter for #axiom_.
         * @return #axiom_
         */
        [[nodiscard]] const std::string &axiom() const;

        /*!
         * @brief Getter for #non_terminals_.
         * @return #non_terminals_
         */
        [[nodiscard]] const std::set<std::string> &non_terminals() const;

        /*!
         * @brief Getter for #terminals_.
         * @return #terminals
         */
        [[nodiscard]] const std::set<std::string> &terminals() const;

        /*!
         * @brief Setter for #axiom_.
         * @param new_axiom New value of #axiom_.
         */
        void set_axiom(const std::string &new_axiom);

        /*!
         * @brief Outputs a grammar to @p ostream .
         * @param ostream Stream output where we want to output the grammar.
         * @param obj GrammarArray we want to output.
         * @return @p ostream .
         */
        friend std::ostream &operator<<(std::ostream &ostream, const GrammarArray &obj);

        /*!
         * @brief Gets the rules generated by @p variable.
         * @details Overloads operator [] to make the search of the rules of a variable more readable with array syntax.
         * @param variable Variable which we want to find its rules.
         * @return A set containing every rule generated by @p variable.
         */
        std::set<std::vector<std::string>> operator[](const std::string &index);

        std::pair<std::string, std::vector<std::string>> GetRuleFromIndex(int index);

        [[nodiscard]] std::pair<std::string, std::vector<std::string>> GetRuleFromIndex(int index) const;

        int size() { return index_rule_.size(); }

        void InsertTerminal(const std::string &new_symbol);

    };

    std::ostream &operator<<(std::ostream &ostream, const GrammarArray &obj);

} // namespace compiler::grammar

#endif //STRUCT_GRAMMARARRAY_H
