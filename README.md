# BNF-CNF-Sentences-Solver

## A Propositional Logic Calculator

## Background

Propositional logic, a.k.a. sentential logic and statement logic, deals with propositions and the relations between propositions. Propositional logic is a topic of Artificial Intelligence, based on which logical agents can make inference to get new knowledge.

Natural language can be by Backus–Naur Form (BNF) grammer of sentences in propositional logic, which includes the proposition relations like 'not', 'and', 'or', 'implies' and 'if and only if'.
However, BNF grammer of sentences are hard to solve. There is a grammer called Conjunctive Normal Form (CNF), which only cantains proposition relations like 'not', 'and' and 'or', is eaily to solved by many efficient algorithms such as Davis-Putnam-Logemann-Loveland (DPLL) algorithm. Fortunately, every sentence of propositional logic is logically equivalent to a conjunction of clauses. In other words, one can convert BNF grammer of sentences to CNF then applies efficient algorithms to solve.

This project provides the tool for converting BNF grammer of sentences to CNF, and the tools for solving BNF or CNF.

## Enviroment

This calculaor is built on **Linux** with **C++11** language.

## Compilation

Use the command

```bash
g++ -std=c++11 -o solver solver.cpp
```

to compile.
If you compiled successfully, you will see an executable file named **solver** in current directory.

## Usage

Use the command

```bash
./solver [-v] -mode $mode input-file
```

to run the program

### Options

- ```-v``` is an optional flag for verbose mode.
- ```-mode``` is followed by ```cnf```, ```dpll``` or ```solver```:
  - In mode ```cnf``` you should input a BNF file, which will be convert to CNF and print to console.
  - In mode ```dpll``` you should input a CNF file, which will be solved by DPLL algorithm and whose solution will be printed to console.
  - In mode ```solver``` you should input a BNF file, which will be convert to CNF, then as the new input solved by DPLL algorithm, and finally the solution will be printed to console;
- ```Input-file``` is a mode-dependent input file and must be put in the last position after any other arguments.

### Input-file format

- Should be a .txt file.
- Should be no space in the file name.
- For the file used for mode ```cnf``` or ```solver```:
  - Should only contain alphanumeric characters, ```_```, ```!``` and space.
- For the file used for mode ```dpll```:
  - Only contains alphanumeric characters, ```_```, ```!```, ```&```, ```|```, ```(```, ```)```, ```[```, ```]```, ```<```, ```>```, ```=``` and space.
  - Please make sure your logic-expressions are valid, for example, if there is a left parenthesis ```(```, there always should be a right parenthesis ```)``` in the expression. The program will not examine whether your expressions are valid or not.

## Acknowledgement

This project came from course **Artificial Intelligence (CSCI-GA.2560)** in **NYU**, offered by **Prof. Paul Bethe**.
