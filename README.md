# Implementation of Davis-Putnam algorithm
Automated Reasoning course project at Faculty of Mathematics, University of Belgrade. <br>
The Davis-Putnam algorithm is a prominent method for verifying the satisfiability of formulas in conjunctive normal form in propositional logic. <br>
This algorithm introduced the use of the resolution principle, which became a cornerstone in the field of automated theorem proving. <br>
The Davis-Putnam algorithm systematically eliminates variables from the formula through a series of resolution steps, simplifying the problem until a solution is found or unsatisfiability is proven.

## Removing Tautological Clauses
The first step of the algorithm is to remove all tautological clauses from the formula. <br>
A tautological clause is a clause that contains both a literal and its negation, making it automatically true. <br>
By removing these clauses, the formula is simplified.

## Unit Clause Propagation
After removing tautological clauses, the algorithm searches for unit clauses, which contain only one literal. <br>
These literals are set to true, and all clauses containing them are removed from the formula. <br>
Clauses containing the negation of these literals are updated by removing that literal.

## Elimination of "Pure" Literals
A pure literal is a literal that appears only in one form (positive or negative) throughout the entire formula. <br>
The algorithm finds and removes clauses containing pure literals, as their presence does not affect the satisfiability of the formula.

## Variable Elimination
Variable elimination is a crucial part of the Davis-Putnam procedure. The algorithm selects a variable to apply the elimination rule and combines clauses containing that literal and its negation to form new clauses. This process is repeated until an empty clause is found (indicating unsatisfiability) or the set of clauses is empty (indicating satisfiability).

## Heuristics for Variable Selection

In the implementation of the Davis-Putnam algorithm, the choice of the variable for resolution can significantly affect efficiency. <br>
The heuristics used include:

- **Maximum Frequency Heuristic**: Selecting the variable that appears most frequently in the formula to reduce the number of clauses as quickly as possible.
- **Random Heuristic**: Selecting the variable randomly to avoid potential bias and enable exploration of different paths to the solution.


# Cloning the Repository and Running the Algorithm

## On Linux
1. Open a terminal and navigate to the desired directory.
2. Clone the repository:
```sh
git clone git@github.com:StefanJevtic63/ar.git
```
3. Update your package list and install g++:
```sh
sudo apt-get update
sudo apt-get install g++
```
4. Run the script to perform tests:
```sh
./perform-tests.sh
```

## On Windows
1. Open Command Prompt / Windows PowerShell / Git Bash and navigate to the desired repository.
2. Clone the repository:
```sh
git clone git@github.com:StefanJevtic63/ar.git
```
3. Choose between IDEs like CLion, Microsoft Visual Studio, Visual Studio Code or install WSL (Windows Subsystem for Linux) by opening your preferred terminal as an administrator and running the following command:
```sh
wsl --install
```

4. Run the bash script to perform tests:
```sh
./perform-tests.sh
```

# References
[1] Armin Biere, Marijn Heule, and Hans van Maaren, eds. Handbook of
satisfiability. Vol. 185. IOS press, 2009.

[2] Martin Davis and Hilary Putnam, A computing procedure for quantification
theory. Vol. 7, pp. 201-215. Journal of the Association for Computing
Machinery, 1960.

[3] Martin Davis, George Logemann and Donald Loveland, A machine program
for theorem proving. Vol. 5, 394–397. Communications of the Association
for Computing Machinery, 1962.

[4] Hantao Zhang and Mark. E. Stickel, Implementing the Davis-Putnam
Method. Kluwer Academic Publishers, 2000.

[5] Stephen A. Cook, The complexity of theorem-proving procedures. Communications
of the Association for Computing Machinery, 1971.
