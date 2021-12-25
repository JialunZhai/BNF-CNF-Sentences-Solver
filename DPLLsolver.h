#ifndef _DPLL_SOLVER_H_
#define _DPLL_SOLVER_H_

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <utility>
#include <unordered_map>
#include <unordered_set>
#include <vector>
using namespace std;
//-------------------------- Clause ---------------------------------
class Clause {
public:
    friend bool operator<(const Clause& clause1, const Clause& clause2);
    friend bool operator==(const Clause& clause1, const Clause& clause2);
    //------------------------------------------------------------------
    Clause();
    Clause(const string& raw_clause);
    Clause(const initializer_list<pair<const string, bool>>& init_lst);//290
    Clause(const unordered_map<string, bool>& _atoms);
    Clause(const Clause& clause);
    //------------------------------------------------------------------
    Clause& insert(const string& atom, const bool& sign);
    bool propagate(const string& atom, const bool& value);
    bool empty()const;
    size_t size()const;
    bool isAtom()const;
    pair<string, bool> calAtom()const;
    set<string> getAtomLabels()const;
    string toString()const;
    unordered_map<string, bool> getAtoms()const;
private:
    unordered_map<string, bool> atoms;
};
bool operator<(const Clause& clause1, const Clause& clause2) {
    const auto& atoms1 = clause1.atoms;
    const auto& atoms2 = clause2.atoms;
    if (atoms1.size() != atoms2.size()) return atoms1.size() < atoms2.size();
    auto iter1 = atoms1.begin();
    auto iter2 = atoms2.begin();
    while (iter1 != atoms1.end() && iter1->first == iter2->first) {
        ++iter1;
        ++iter2;
    }
    if (iter1 != atoms1.end()) return iter1->first < iter2->first;

    iter1 = atoms1.begin();
    iter2 = atoms2.begin();
    while (iter1 != atoms1.end() && iter1->second == iter2->second) {
        ++iter1;
        ++iter2;
    }
    if (iter1 != atoms1.end() && iter1->second != iter2->second) return iter1->second < iter2->second;
    return false;
}
bool operator==(const Clause& clause1, const Clause& clause2) {
    return clause1.atoms == clause2.atoms;
}
Clause::Clause() {}
Clause::Clause(const string& raw_clause) {
    string atom;
    bool sign = false;
    for (const auto& c : raw_clause) {
        if (isblank(c)) {
            if (atom.empty()) continue;
            else {
                auto iter = atoms.find(atom);
                if (iter != atoms.end() && iter->second != sign) {
                    atoms.clear();
                    return;
                }
                insert(atom, sign);
                atom.clear();
                sign = false;
            }
        }
        else if (c == '!') {
            sign = !sign;
        }
        else {
            atom.push_back(c);
        }
    }
    if (!atom.empty()) {
        auto iter = atoms.find(atom);
        if (iter != atoms.end() && iter->second != sign) {
            atoms.clear();
            return;
        }
        insert(atom, sign);
    }
}
Clause::Clause(const initializer_list<pair<const string, bool>>& init_lst) : atoms(init_lst) {}
Clause::Clause(const unordered_map<string, bool>& _atoms) : atoms(_atoms) {}
Clause::Clause(const Clause& clause) : atoms(clause.atoms) {}
Clause& Clause::insert(const string& atom, const bool& sign) {
    atoms.insert({ atom,sign });
    return *this;
}
bool Clause::propagate(const string& atom, const bool& value) {
    auto iter = atoms.find(atom);
    if (iter != atoms.end()) {
        if (iter->second ^ value) atoms.clear();
        else {
            atoms.erase(iter);
            if (atoms.empty()) return false;
        }
    }
    return true;
}
bool Clause::empty()const {
    return atoms.empty();
}
size_t Clause::size()const {
    return atoms.size();
}
bool Clause::isAtom()const {
    return atoms.size() == 1;
}
pair<string, bool> Clause::calAtom()const {
    if (atoms.size() != 1) return { "",false };
    return { atoms.begin()->first,!atoms.begin()->second };
}
set<string> Clause::getAtomLabels()const {
    set<string> atom_labels;
    for (const auto& a2v : atoms) atom_labels.insert({ a2v.first });
    return atom_labels;
}
string Clause::toString()const {
    string s;
    for (const auto& atom2v : atoms) {
        if (atom2v.second) s += "!";
        s += atom2v.first;
        s.push_back(' ');
    }
    if (!s.empty()) s.pop_back();
    return s;
}
unordered_map<string, bool> Clause::getAtoms()const {
    return atoms;
}

//-------------------------- DPLLsolver ---------------------------------    

class DPLLsolver {
public:
    DPLLsolver();
    DPLLsolver(const DPLLsolver& dpll_solver);
    DPLLsolver& insert(const string& raw_clause);
    bool assign(const string& atom, const bool& value);
    const DPLLsolver& printClauses()const;
    set<string> getAtomLabels()const;
    map<string, bool> driverDPLL();
    DPLLsolver& runDPLL();
    int isPureLiteral(const string& atom)const;
    DPLLsolver& setVerboseMode(const bool& mode);
private:
    bool recursiveDPLL(map<string, bool>& ans, set<string>& candidates);
private:
    set<Clause> clauses;
    bool is_verbose_mode;
};
DPLLsolver::DPLLsolver() :is_verbose_mode(false) {}
DPLLsolver::DPLLsolver(const DPLLsolver& dpll_solver) : clauses(dpll_solver.clauses), is_verbose_mode(dpll_solver.is_verbose_mode) {}
DPLLsolver& DPLLsolver::setVerboseMode(const bool& mode) {
    is_verbose_mode = mode;
    return *this;
}
DPLLsolver& DPLLsolver::insert(const string& raw_clause) {
    Clause clause(raw_clause);
    if (!clause.empty()) clauses.insert({ clause });// do not insert the clause which both contains A and !A
    return *this;
}
bool DPLLsolver::assign(const string& atom, const bool& value) {
    auto clauses_cp = clauses;
    clauses.clear();
    for (auto clause : clauses_cp) {
        if (!clause.propagate(atom, value)) return false;
        if (!clause.empty()) clauses.insert({ clause });
    }
    return true;
}
set<string> DPLLsolver::getAtomLabels()const {
    set<string> labels;
    for (const auto& clause : clauses) {
        auto part_of_labels = clause.getAtomLabels();
        labels.insert(part_of_labels.begin(), part_of_labels.end());
    }
    return labels;
}
const DPLLsolver& DPLLsolver::printClauses()const {
    for (const auto& clause : clauses) {
        cout << clause.toString() << endl;
    }
    return *this;
}
int DPLLsolver::isPureLiteral(const string& atom)const {
    int sign = -1;
    for (const auto& clause : clauses) {
        auto atoms = clause.getAtoms();
        auto iter = atoms.find(atom);
        if (iter == atoms.end()) continue;
        if (sign == -1) sign = iter->second;
        else {
            if (sign != iter->second) return -1;
        }
    }
    return sign;
}
map<string, bool> DPLLsolver::driverDPLL() {
    map<string, bool> ans;
    set<string> candidates(getAtomLabels());
    if (!recursiveDPLL(ans, candidates)) return {};
    for (const auto& candidate : candidates) {
        if (is_verbose_mode) cout << "unbound " << candidate << "=false" << endl;
        ans.insert({ candidate,false });
    }
    return ans;
}
bool DPLLsolver::recursiveDPLL(map<string, bool>& ans, set<string>& candidates) {
    set<string> assigned_atoms;
    while (!clauses.empty() && clauses.begin()->isAtom()) {
        auto a2v = clauses.begin()->calAtom();
        ans.insert(a2v);
        candidates.erase(a2v.first);
        assigned_atoms.insert(a2v.first);
        if (!assign(a2v.first, a2v.second)) {
            if (is_verbose_mode) cout << (a2v.second ? "!" : "") << a2v.first << " contradiction" << endl;
            for (const auto& a : assigned_atoms) {
                candidates.insert({ a });
                ans.erase(a);
            }
            return false;
        }
        if (is_verbose_mode) cout << "easyCase " << a2v.first << " = " << (a2v.second ? "true" : "false") << endl;
        if (is_verbose_mode) printClauses();
    }
    if (clauses.empty()) return true;
    //--------------------pure literal----------------------------
    string pure_literal;
    int sign = -1;
    for (const auto& cndt : candidates) {
        sign = isPureLiteral(cndt);
        if (sign != -1) {
            pure_literal = cndt;
            break;
        }
    }
    if (sign != -1) {
        candidates.erase(pure_literal);
        ans.insert({ pure_literal,!sign });
        DPLLsolver solver(*this);
        if (is_verbose_mode) cout << "easyCase " << pure_literal << "=" << (!sign ? "true" : "false") << endl;
        if (solver.assign(pure_literal, !sign)) {
            if (is_verbose_mode) solver.printClauses();
            if (solver.recursiveDPLL(ans, candidates)) return true;
        }
        ans.erase(pure_literal);
        candidates.insert({ pure_literal });

        for (const auto& a : assigned_atoms) {
            candidates.insert({ a });
            ans.erase(a);
        }
        return false;
    }
    //----------------------hard case------------------------------
    auto candidate = *candidates.begin();
    candidates.erase(candidate);
    for (const bool& guess : { true,false }) {
        ans.insert({ candidate,guess });
        DPLLsolver solver(*this);
        if (is_verbose_mode) cout << "hard case, guess: " << candidate << "=" << (guess ? "true" : "false") << endl;
        if (solver.assign(candidate, guess)) {
            if (is_verbose_mode) solver.printClauses();
            if (solver.recursiveDPLL(ans, candidates)) return true;
        }
        else {
            if (is_verbose_mode) cout << (guess ? "!" : "") << candidate << " contradiction" << endl;
        }
        if (is_verbose_mode) cout << "fail|hard case, try: " << candidate << "=" << (guess ? "true" : "false") << endl;
        ans.erase(candidate);
    }
    candidates.insert({ candidate });

    for (const auto& a : assigned_atoms) {
        candidates.insert({ a });
        ans.erase(a);
    }
    return false;
}

DPLLsolver& DPLLsolver::runDPLL() {
    if (is_verbose_mode) printClauses();
    auto a2v_lst = driverDPLL();
    if (a2v_lst.empty()) {
        cout << "NO VALID ASSIGNMENT" << endl;
        return *this;
    }
    for (const auto& a2v : a2v_lst) {
        cout << a2v.first << " = " << (a2v.second ? "true" : "false") << endl;
    }
    return *this;
}

#endif