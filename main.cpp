#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>

using Atom = int;
using Literal = int;
using Clause = std::set<Literal>;
using NormalForm = std::set<Clause>;

/**
* @struct DP
* Represents a data structure used for processing and manipulating logical formulas in conjunctive normal form (CNF).
*
* This struct maintains sets of literals, including the set of all literals and the set of false literals.
* It provides various methods for analyzing and modifying the normal form of the formula.
*/
struct DP {
    std::set<Literal> literals;
    std::set<Literal> falseLiterals;

    /**
    * @brief Prints the given normal form of the formula.
    *
    * @param f The normal form of the formula to be printed.
    */
    void print(const NormalForm& f) {
        for (const Clause& clause : f) {
            std::cout << "[ ";
            for (const Literal& literal : clause)
                std::cout << literal << " ";
            std::cout << "]";
        }
        std::cout << std::endl;
    }

    /**
    * @brief Checks if a given clause is tautological.
    *
    * Clause is tautological if it contains both a literal and its negation.
    *
    * @param clause The clause to be checked.
    * @return bool True if the clause is tautological, false otherwise.
    */
    bool isTautologicClause(const Clause& clause) {
        std::set<Literal> seenLiterals;
        for (const Literal& literal : clause) {
            if (seenLiterals.find(-literal) != seenLiterals.end()) return true;
            seenLiterals.insert(literal);
        }

        return false;
    }

    /**
    * @brief Removes all tautological clauses from the given normal form.
    *
    * @param f The normal form of the formula, which will be modified.
    */
    void removeAllTautologyClauses(NormalForm& f) {
        for (auto it = f.begin(); it != f.end(); )
            if (isTautologicClause(*it)) it = f.erase(it);
            else ++it;
    }

    /**
    * @brief Checks if a given clause is a unit clause, meaning it contains only one literal.
    *
    * @param clause The clause to be checked.
    * @return bool True if the clause is a unit clause, false otherwise.
    */
    bool isUnitClause(const Clause& clause) {
        return clause.size() == 1;
    }

    /**
     * @brief Removes all false literals from the clauses in the given normal form.
     *
     * @param f The normal form of the formula, which will be modified.
     * @param conflict Bool value that becomes yes if the clause becomes empty.
     */
    void removeFalseLiterals(NormalForm& f, bool& conflict) {
        for (auto it = f.begin(); it != f.end(); ) {
            bool clauseModified = false;
            Clause newClause;
            for (const Literal& literal : *it)
                if (falseLiterals.find(literal) != falseLiterals.end()) clauseModified = true;
                else newClause.insert(literal);

            if (clauseModified) {
                if (newClause.empty()) {
                    conflict = true;
                    return;  // UNSAT - empty clause
                }

                it = f.erase(it);
                if (!newClause.empty()) {
                    if (isUnitClause(newClause)) {
                        falseLiterals.insert(-(*newClause.begin()));
                        it = f.begin(); // potentially remove newly unlocked false literals
                    }
                    else f.insert(newClause);
                }
            }
            else ++it;
        }
    }

    /**
    * @brief Removes all unit clauses from the given normal form and updates the set of false literals accordingly.
    *
    * @param f The normal form of the formula, which will be modified.
    * @param conflict Bool value that becomes yes if there are conflict clauses.
    */
    void removeUnitClauses(NormalForm& f, bool& conflict) {
        for (auto it = f.begin(); it != f.end(); ) {
            if (isUnitClause(*it)) {
                Literal unitLiteral = *it->begin();
                if (falseLiterals.find(unitLiteral) != falseLiterals.end()) {
                    conflict = true;
                    return;  // UNSAT - conflict clauses
                }

                falseLiterals.insert(-unitLiteral);
                it = f.erase(it);
            }
            else ++it;
        }

        if (!falseLiterals.empty())
            removeFalseLiterals(f, conflict);
    }

    /**
    * @brief Checks if a given literal is a pure literal.
    *
    * A pure literal is a literal that appears only with one sign (either positive or negative) in the entire formula.
    * This function iterates through all clauses and checks if they contain pure literals.
    *
    * @param literal The literal to be checked.
    * @param f The normal form of the formula.
    * @return bool True if the literal is a pure literal, false otherwise.
    */
    bool isPureLiteral(const Literal& literal, const NormalForm& f) {
        for (const Clause& clause : f)
            if (clause.find(-literal) != clause.end()) return false;

        return true;
    }

    /**
    * @brief Removes all clauses containing the given pure literal from the normal form.
    *
    * @param f The normal form of the formula, which will be modified.
    * @param pureLiteral The pure literal to be removed.
    */
    void removePureClausesByLiteral(NormalForm& f, const Literal& pureLiteral) {
        for (auto it = f.begin(); it != f.end(); )
            if (it->find(pureLiteral) != it->end()) it = f.erase(it);
            else ++it;
    }

    /**
     * @brief Removes pure clauses from the given normal form.
     *
     * This function iterates through all literals and removes the clauses containing the pure literals.
     *
     * @param f The normal form from which the pure clauses should be removed.
     */
    void removePureClauses(NormalForm& f) {
        for (const Literal& literal : literals)
            if (isPureLiteral(literal, f))
                removePureClausesByLiteral(f, literal);
    }

    /**
    * @brief Retrieves all clauses in the given normal form that contain the specified literal.
    *
    * This function iterates through all clauses in the normal form and adds the clauses that contain the target literal to the result vector.
    *
    * @param f The normal form to search through.
    * @param target The literal to search for in the clauses.
    * @return std::vector<Clause> A vector containing all clauses that include the target literal.
    */
    std::vector<Clause> allClausesWithGivenLiteral(const NormalForm& f, const Literal& target) {
        std::vector<Clause> result;
        for (const auto& clause : f)
            if (clause.find(target) != clause.end()) result.push_back(clause);

        return result;
    }

    /**
    * @brief Resolves two clauses on a given literal, producing a new clause.
    *
    * The resolution operation is a fundamental inference rule in propositional logic.
    *  It takes two clauses that contain complementary literals (i.e., a literal and its negation)
    *  and produces a new clause that is the union of the two clauses, excluding the complementary literals.
    *
    * @param first The first clause to be resolved.
    * @param second The second clause to be resolved.
    * @param target The literal on which the resolution should be performed.
    * @return Clause The new clause resulting from the resolution operation.
    */
    Clause resolve(const Clause& first, const Clause& second, const Literal& target) {
        Clause result;
        if (first.find(target) != first.end() && second.find(-target) != second.end()) {
            for (const Literal& literal : first)
                if (literal != target && literal != -target) result.insert(literal);

            for (const Literal& literal : second)
                if (literal != target && literal != -target) result.insert(literal);
        }

        return result;
    }

    /**
    * @brief Parses a normal form representation from the given input stream.
    *
    * This function reads the normal form from the input stream, which should be in the DIMACS format.
    * It skips any comment lines (starting with 'c') until it reaches the 'p' line,
    *  which specifies the number of atoms and clauses in the formula.
    *  It then reads the clauses and constructs the normal form.
    *
    * @param fin The input stream from which the normal form should be read.
    * @return NormalForm The parsed normal form.
    */
    NormalForm parse(std::istream& fin) {
        std::string buffer;
        do {
            fin >> buffer;
            if(buffer == "c") fin.ignore(10000, '\n');
        } while(buffer != "p");

        // for "cnf"
        fin >> buffer;

        int atomCount, clauseCount;
        fin >> atomCount >> clauseCount;

        NormalForm formula;
        for(int i = 0; i < clauseCount; i++) {
            Clause c;
            Literal l; fin >> l;

            while(l != 0) {
                literals.insert(l);
                c.insert(l);
                fin >> l;
            }

            formula.insert(c);
        }

        return formula;
    }

    /**
     * @brief Computes the maximum occurrence count of each atom in the given normal form.
     *
     * This function iterates through all clauses in the normal form and counts the number of occurrences of each atom.
     * The result is a map that associates each atom with its maximum occurrence count in the formula.
     *
     * @param f The normal form for which the maximum occurrence counts should be computed.
     * @return std::map<Atom, unsigned> A map that associates each atom with its maximum occurrence count.
     */
    std::map<Atom, unsigned> maximumOccurrence(NormalForm& f) {
        std::map<Atom, unsigned> occurrence;
        for (const Clause& clause : f)
            for (const Literal& literal : clause)
                occurrence[ std::abs(literal) ]++;

        return occurrence;
    }

    /**
     * @brief Returns a vector of currently present atoms in random order.
     *
     * This function iterates through currently present literals and extracts their atom representation.
     * The result is a vector of atoms in random order.
     *
     * @return std::vector<Atom> Random order of atoms currently present in the formula.
     */
    std::vector<Atom> atomsRandomOrder() {
        std::vector<bool> visited(literals.size() + 1, false);
        std::vector<Atom> result;
        for (const Literal& literal : literals) {
            if (literal < 0 && !visited[-literal]) {
                visited[-literal] = true;
                result.push_back(-literal);
            }
            else if (literal > 0 && !visited[literal]) {
                visited[literal] = true;
                result.push_back(literal);
            }
        }

        // Seed with a real random value, if available
        std::random_device rd;
        // Initialize a random number generator
        std::mt19937 g(rd());

        // Shuffle the vector
        std::shuffle(result.begin(), result.end(), g);

        return result;
    }

    /**
     * @brief Recursively solves a Boolean satisfiability problem represented in normal form.
     *
     * @param f The normal form of the Boolean satisfiability problem to be solved.
     * @return true if the problem is satisfiable, false otherwise.
     */
    bool solveRec(NormalForm& f) {
        // Remove pure clauses from the normal form
        removePureClauses(f);
        // std::cout << "After removing pure clauses: " << literals.size() << " " << f.size() << std::endl;

        // If the normal form is empty, the problem is satisfiable
        if (f.empty()) return true;
        // If the normal form has a single empty clause, the problem is unsatisfiable
        if (f.size() == 1 && f.begin()->empty()) return false;

        // Choose a literal to resolve on using the Maximum Occurrence heuristic
        bool foundNewClause = false;
        std::map<Atom, unsigned> occurrence = maximumOccurrence(f);
        for (auto it = occurrence.rbegin(); it != occurrence.rend(); ++it) {
            const Atom literal = it->first;

            // Get the clauses containing the literal and the clauses containing the negation of the literal
            auto clausesWith = allClausesWithGivenLiteral(f, literal);
            auto clausesWithout = allClausesWithGivenLiteral(f, -literal);

            // If any of the clause sets is empty, continue to the next literal
            if (clausesWith.empty() || clausesWithout.empty()) continue;
            foundNewClause = true;

            // Add the resolvent clauses to the normal form
            for (const Clause& clause1 : clausesWith)
                for (const Clause& clause2 : clausesWithout) {
                    Clause resolved = resolve(clause1, clause2, literal);
                    if (resolved.empty()) return false;
                    if (isUnitClause(resolved)) {
                        falseLiterals.insert(-(*resolved.begin()));
                        bool conflict;
                        removeFalseLiterals(f, conflict);
                        if (conflict) return false;   // UNSAT - empty clause
                        return solveRec(f);
                    }
                    if (!isTautologicClause(resolved)) f.insert(resolved);
                }

            // Remove the clauses used for resolution from the normal form
            for (const Clause& clause : clausesWith) f.erase(clause);
            for (const Clause& clause : clausesWithout) f.erase(clause);

            // Update the list of literals
            literals.erase(literal);
            literals.erase(-literal);
            falseLiterals.erase(literal);
            falseLiterals.erase(-literal);

            // std::cout << "After resolution: " << literals.size() << " " << f.size() << std::endl;
        }
        // std::cout << "#############################" << std::endl;

        // If no new resolvent clauses were found, the problem is satisfiable
        if (!foundNewClause) return true;

        return solve(f);
    }

    /**
     * @brief Solves a Boolean satisfiability problem represented in normal form.
     *
     * @param f The normal form of the Boolean satisfiability problem to be solved.
     * @return true if the problem is satisfiable, false otherwise.
     */
    bool solve(NormalForm& f) {
        // std::cout << "Before removing clauses: " << literals.size() << " " << f.size() << std::endl;
        removeAllTautologyClauses(f);
        // std::cout << "After removing tautology clauses: " << literals.size() << " " << f.size() << std::endl;
        bool conflict;
        removeUnitClauses(f, conflict);
        if (conflict) return false;  // UNSAT - empty clause
        // std::cout << "After removing unit clauses: " << literals.size() << " " << f.size() << std::endl;

        return solveRec(f);
    }
};

int main()
{
    DP solver;
    NormalForm formula = solver.parse( std::cin);

    std::cout << (solver.solve(formula) == true ? "true" : "false") << std::endl;
}