#include <iostream>
#include <set>
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

    void removeAllTautologyClauses(NormalForm& f) {
        std::set<Literal> seenLiterals;
        for (auto it = f.begin(); it != f.end(); ) {
            seenLiterals.clear();
            bool tautology = false;
            for (const Literal& literal : *it) {
                if (seenLiterals.contains(-literal)) {
                    tautology = true;
                    break;
                }
                seenLiterals.insert(literal);
            }

            if (tautology) it = f.erase(it);
            else ++it;
        }
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
                    if (newClause.size() == 1) {
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
            if (it->size() == 1) {
                Literal unitLiteral = *it->begin();
                falseLiterals.insert(-unitLiteral);
                it = f.erase(it);
            }
            else ++it;
        }

        if (!falseLiterals.empty())
            removeFalseLiterals(f);
    }

    bool isPureLiteral(const Atom& atom, const NormalForm& f) {
        bool found_pos = false, found_neg = false;
        for (const Clause& clause : f)
            for (const Literal& literal : clause)
                if (std::abs(literal) == atom) {
                    if (literal > 0) found_pos = true;
                    else if (literal < 0) found_neg = true;
                    if (found_pos && found_neg) return false;
                }

        return found_pos ^ found_neg;
    }

    void removePureClauses(NormalForm& f) {
        for (const Literal& pureLiteral : literals)
            if (isPureLiteral(std::abs(pureLiteral), f))
                for (auto it = f.begin(); it != f.end(); )
                    if (it->contains(pureLiteral)) it = f.erase(it);
                    else ++it;

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
                if (literal != target) result.insert(literal);

            for (const Literal& literal : second)
                if (literal != -target) result.insert(literal);
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

    bool solve(NormalForm& f) {
        // izbegavanje rekurzije
        while(true) {
            print(f);

            std::cout << "Pre uklanjanja klauza: " << f.size() << std::endl;
            removeAllTautologyClauses(f);
            std::cout << "Nakon uklanjanja tautologicnih klauza: " << f.size() << std::endl;
            removeUnitClauses(f);
            std::cout << "Nakon uklanjanja jedinicnih klauza: " << f.size() << std::endl;
            removePureClauses(f);
            std::cout << "Nakon uklanjanja cistih klauza: " << f.size() << std::endl;

            if (f.empty()) return true;
            if (f.size() == 1 && f.begin()->empty()) return false;

            bool foundNewClause = false;
            // biramo literal po kome ce se rezolvirati
            for (const Literal& literal : literals) {
                auto clausesWith = allClausesWithGivenLiteral(f, literal);
                auto clausesWithout = allClausesWithGivenLiteral(f, -literal);

                if (clausesWith.empty() || clausesWithout.empty()) continue;
                foundNewClause = true;

                // dodajemo rezolvente u skup klauza
                for (const Clause& clause1 : clausesWith)
                    for (const Clause& clause2 : clausesWithout) {
                        Clause resolved = resolve(clause1, clause2, literal);
                        if (resolved.empty()) return false;
                        if (resolved.size() == 1) falseLiterals.insert(-(*resolved.begin()));
                        else f.insert(resolved);
                    }

                // uklanjamo klauze pomocu kojih smo vrsili rezoluciju
                for (const Clause& clause : clausesWith) f.erase(clause);
                for (const Clause& clause : clausesWithout) f.erase(clause);

                std::cout << "Nakon rezolviranja: " << f.size() << std::endl;
            }
            std::cout << "#############################" << std::endl;

            // ako ne postoje nove rezolvente koje mozemo izvesti prijavljujemo zadovoljivost
            if (!foundNewClause) return true;
        }
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