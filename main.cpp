#include <iostream>
#include <set>
#include <map>
#include <vector>

using Atom = int;
using Literal = int;
using Clause = std::set<Literal>;
using NormalForm = std::set<Clause>;

struct DP {
    std::set<Literal> literals;
    std::set<Literal> falseLiterals;

    void print(const NormalForm& f) {
        for (const Clause& clause : f) {
            std::cout << "[ ";
            for (const Literal& literal : clause)
                std::cout << literal << " ";
            std::cout << "]";
        }
        std::cout << std::endl;
    }

    bool isTautologicClause(const Clause& clause) {
        std::set<Literal> seenLiterals;
        for (const Literal& literal : clause) {
            if (seenLiterals.contains(-literal)) return true;
            seenLiterals.insert(literal);
        }

        return false;
    }

    void removeAllTautologyClauses(NormalForm& f) {
        for (auto it = f.begin(); it != f.end(); )
            if (isTautologicClause(*it)) it = f.erase(it);
            else ++it;
    }

    bool isUnitClause(const Clause& clause) {
        return clause.size() == 1;
    }

    void removeFalseLiterals(NormalForm& f) {
        for (auto it = f.begin(); it != f.end(); ) {
            bool clauseModified = false;
            Clause newClause;
            for (const Literal& literal : *it)
                if (falseLiterals.contains(literal)) clauseModified = true;
                else newClause.insert(literal);

            if (clauseModified) {
                it = f.erase(it);
                if (!newClause.empty()) {
                    if (isUnitClause(newClause)) {
                        falseLiterals.insert(-(*newClause.begin()));
                        it = f.begin(); // mozda sada mozemo neki literal od ranije da uklonimo
                    }
                    else f.insert(newClause);
                }
            }
            else ++it;
        }
    }

    void removeUnitClauses(NormalForm& f) {
        for (auto it = f.begin(); it != f.end(); ) {
            if (isUnitClause(*it)) {
                Literal unitLiteral = *it->begin();
                falseLiterals.insert(-unitLiteral);
                it = f.erase(it);
            }
            else ++it;
        }

        if (!falseLiterals.empty())
            removeFalseLiterals(f);
    }

    bool isPureLiteral(const Literal& literal, const NormalForm& f) {
        for (const Clause& clause : f)
            if (clause.contains(-literal)) return false;

        return true;
    }

    void removePureClausesByLiteral(NormalForm& f, const Literal& pureLiteral) {
        for (auto it = f.begin(); it != f.end(); )
            if (it->contains(pureLiteral)) it = f.erase(it);
            else ++it;
    }

    void removePureClauses(NormalForm& f) {
        for (const Literal& literal : literals)
            if (isPureLiteral(literal, f))
                removePureClausesByLiteral(f, literal);
    }

    std::vector<Clause> allClausesWithGivenLiteral(const NormalForm& f, const Literal& target) {
        std::vector<Clause> result;
        for (const auto& clause : f)
            if (clause.contains(target)) result.push_back(clause);

        return result;
    }

    Clause resolve(const Clause& first, const Clause& second, const Literal& target) {
        Clause result;
        if (first.contains(target) && second.contains(-target)) {
            for (const Literal& literal : first)
                if (literal != target && literal != -target) result.insert(literal);

            for (const Literal& literal : second)
                if (literal != target && literal != -target) result.insert(literal);
        }

        return result;
    }

    NormalForm parse(std::istream& fin) {
        std::string buffer;
        do {
            fin >> buffer;
            if(buffer == "c") fin.ignore(10000, '\n');
        } while(buffer != "p");

        // za "cnf"
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

    std::map<Atom, unsigned> maximumOccurrence(NormalForm& f) {
        std::map<Atom, unsigned> occurrence;
        for (const Clause& clause : f)
            for (const Literal& literal : clause)
                occurrence[ std::abs(literal) ]++;

        return occurrence;
    }

    bool solveRec(NormalForm& f) {
        removePureClauses(f);
        std::cout << "Nakon uklanjanja cistih klauza: " << literals.size() << " " << f.size() << std::endl;

        if (f.empty()) return true;
        if (f.size() == 1 && f.begin()->empty()) return false;

        // biramo literal po kome ce se rezolvirati po heuristici Maximum occurrence
        bool foundNewClause = false;
        std::map<Atom, unsigned> occurrence = maximumOccurrence(f);
        for (auto it = occurrence.rbegin(); it != occurrence.rend(); ++it) {
            const Atom literal = it->first;

            auto clausesWith = allClausesWithGivenLiteral(f, literal);
            auto clausesWithout = allClausesWithGivenLiteral(f, -literal);

            if (clausesWith.empty() || clausesWithout.empty()) continue;
            foundNewClause = true;

            // dodajemo rezolvente u skup klauza
            for (const Clause& clause1 : clausesWith)
                for (const Clause& clause2 : clausesWithout) {
                    Clause resolved = resolve(clause1, clause2, literal);
                    if (resolved.empty()) return false;
                    if (isUnitClause(resolved)) {
                        falseLiterals.insert(-(*resolved.begin()));
                        removeFalseLiterals(f);
                        return solveRec(f);
                    }
                    if (!isTautologicClause(resolved)) f.insert(resolved);
                }

            // uklanjamo klauze pomocu kojih smo vrsili rezoluciju
            for (const Clause& clause : clausesWith) f.erase(clause);
            for (const Clause& clause : clausesWithout) f.erase(clause);

            // azuriramo listu literala
            literals.erase(literal);
            literals.erase(-literal);
            falseLiterals.erase(literal);
            falseLiterals.erase(-literal);

            std::cout << "Nakon rezolviranja: " << literals.size() << " " << f.size() << std::endl;
        }
        std::cout << "#############################" << std::endl;

        // ako ne postoje nove rezolvente koje mozemo izvesti prijavljujemo zadovoljivost
        if (!foundNewClause) return true;

        return solve(f);
    }

    bool solve(NormalForm& f) {
        std::cout << "Pre uklanjanja klauza: " << literals.size() << " " << f.size() << std::endl;
        removeAllTautologyClauses(f);
        std::cout << "Nakon uklanjanja tautologicnih klauza: " << literals.size() << " " << f.size() << std::endl;
        removeUnitClauses(f);
        std::cout << "Nakon uklanjanja jedinicnih klauza: " << literals.size() << " " << f.size() << std::endl;

        return solveRec(f);
    }
};

int main()
{
    DP solver;
    NormalForm formula = solver.parse( std::cin);

    std::cout << (solver.solve(formula) == true ? "true" : "false") << std::endl;

    return 0;
}

/*
p cnf 3 3
-1 -2 3 0
-1 2 0
1 -3 0
*/