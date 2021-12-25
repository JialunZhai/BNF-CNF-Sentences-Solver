//g++ -std=c++11 -o solver solver.cpp
//./solver -v -mode solver eg1.txt
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
#include <getopt.h>
#include "BNF2CNFconverter.h"
#include "DPLLsolver.h"

using namespace std;
//----------------------------------------------------------------------
bool isCNFLine(const string& line);
bool isBNFLine(const string& line);
void parse_cmd(int argc, char* argv[], bool& verbose_mode, string& mode, string& infile);
void procDPLLOnCNFfile(ifstream& infile, const bool& mode);
vector<string> procBNF2CNF(ifstream& infile);
void procDPLLOnBNFfile(ifstream& infile, const bool& mode);

//-------------------------- main ---------------------------------
int main(int argc, char* argv[]) {
    bool verbose_mode = false;
    string mode, infile_name;
    parse_cmd(argc, argv, verbose_mode, mode, infile_name);
    ifstream infile(infile_name);
    if (!infile.is_open()) {
        cout << "Error: can't open input-file " << infile_name << endl;
        exit(0);
    }
    if (mode == "cnf") {
        vector<string> lines = procBNF2CNF(infile);
        for (const auto& line : lines) {
            cout << line << endl;
        }
    }
    else if (mode == "dpll") {
        procDPLLOnCNFfile(infile, verbose_mode);
    }
    else if (mode == "solver") {
        procDPLLOnBNFfile(infile, verbose_mode);
    }
    else {
        cout << "Error: invalid mode " << mode << ", please try cnf, dpll or solver" << endl;
        exit(0);
    }
}

//------------------------------------------------------------------
bool isCNFLine(const string& line) {
    for (const auto& c : line) {
        if (!isalnum(c) && c != '_' && c != '!' && c != ' ') {
            cout << "Error: an invalid character " << c << " in CNF line " << endl;
            return false;
        }
    }
    return true;
}
bool isBNFLine(const string& line) {
    unordered_set<char> valid_chars = { '_','!','&','|','(',')','[',']','<','>','=',' ' };
    for (const auto& c : line) {
        if (!isalnum(c) && valid_chars.find(c) == valid_chars.end()) {
            cout << "Error: an invalid character " << c << " in BNF line " << endl;
            return false;
        }
    }
    return true;
}
//----------------------------------------------------------------------
void parse_cmd(int argc, char* argv[], bool& verbose_mode, string& mode, string& infile_name) {
    struct option opts[] = { {"mode",required_argument,NULL,'m'},
                             {"v",no_argument,NULL,'v'} };
    char c;
    while ((c = getopt_long_only(argc, argv, "m:", opts, NULL)) != -1) {
        switch (c) {
        case 'm': mode = string(optarg); break;
        case 'v': verbose_mode = true; break;
        }
    }
    //infile_name is the last cmd argument
    infile_name = string(argv[argc - 1]);
}

void procDPLLOnCNFfile(ifstream& infile, const bool& mode) {
    vector<string> cnf_lines;
    string line;
    while (getline(infile, line)) {
        while (!line.empty() && (line.back() == '\r' || isblank(line.back()))) line.pop_back();
        if (line.empty()) continue;
        if (!isCNFLine(line)) {
            cout << line << endl;
            exit(0);
        }
        cnf_lines.push_back(line);
    }
    DPLLsolver solver;
    solver.setVerboseMode(mode);
    for (const auto& cnf_line : cnf_lines) {
        solver.insert(cnf_line);
    }
    solver.runDPLL();
}

vector<string> procBNF2CNF(ifstream& infile) {
    vector<string> cnf_lines;
    string line;
    while (getline(infile, line)) {
        while (!line.empty() && (line.back() == '\r' || isblank(line.back()))) line.pop_back();
        if (line.empty()) continue;
        if (!isBNFLine(line)) {
            cout << line << endl;
            exit(0);
        }
        vector<string> lines = runBNF2CNF(line);
        cnf_lines.insert(cnf_lines.end(), lines.begin(), lines.end());
    }
    return cnf_lines;
}

void procDPLLOnBNFfile(ifstream& infile, const bool& mode) {
    vector<string> cnf_lines = procBNF2CNF(infile);
    DPLLsolver solver;
    solver.setVerboseMode(mode);
    for (const auto& cnf_line : cnf_lines) {
        solver.insert(cnf_line);
        //    cout << "[" << cnf_line << "]" << endl;///////////////////////
    }
    //cout << "runDPLL:" << endl;//////////////////
    solver.runDPLL();
}
