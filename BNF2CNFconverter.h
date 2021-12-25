#ifndef _BNF2CNF_CONVERTER_H_
#define _BNF2CNF_CONVERTER_H_

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
//--------------------------------------------------------------
class node {
public:
    node() :is_not(false), val(), left(nullptr), right(nullptr) {}
    bool is_not;
    string val;
    node* left;
    node* right;
};
//--------------------------------------------------------------
void replaceBrackets(string& str);
bool isMeaningful(const char& c);
void strip(string& str);
void simplify(string& str);
vector<string> str2Expr(string str);
bool isOperator(const string& str);
vector<string> calIff(vector<string> expr);
vector<string> calImply(vector<string> expr);
void printExpr(const vector<string>& expr);
//---------------------------------------------------
void printTree(node* tree);
node* deepCopy(node* tree);
void delTree(node** ptr_tree);
node* initTree(vector<string> expr);
void calNot(node* tree, bool not_again);
bool isFullyDistributed(node* tree);
void distributeOr(node* tree);
void tree2CNF(node* tree, vector<string>& lines, string& line);
vector<string> tree2CNFDriver(node* tree);
vector<string> runBNF2CNF(const string& bnf_line);
//----------------------- BNF to CNF ------------------------------
void replaceBrackets(string& str) {
    string s;
    for (const auto& c : str) {
        if (c == '[') s.push_back('(');
        else if (c == ']') s.push_back(')');
        else s.push_back(c);
    }
    swap(str, s);
}
bool isMeaningful(const char& c) {
    if (isalnum(c)) return true;
    unordered_set<char> st = { '_','!','|','&','<','>','(',')' };
    if (st.find(c) != st.end()) return true;
    else return false;
}
void strip(string& str) {
    string s;
    for (const auto& c : str) {
        if (isMeaningful(c)) s.push_back(c);
    }
    swap(str, s);
}
void simplify(string& str) { //replace "=>" by ">", replace "<=>" by ""
    string s;
    int i = 0;
    while (i < str.size()) {
        if (str[i] == '<') {
            s.push_back('=');
            while (str[i] != '>') ++i;
            ++i;
        }
        else if (str[i] == '=') {
            s.push_back('>');
            while (str[i] != '>') ++i;
            ++i;
        }
        else {
            s.push_back(str[i]);
            i += 1;
        }
    }
    swap(str, s);
}

vector<string> str2Expr(string str) {
    replaceBrackets(str);
    strip(str);
    simplify(str);

    unordered_set<char> optrs = { '!','|','&','<','>','(',')','=' };

    vector<string> expr;
    string item;
    for (const auto& c : str) {
        if (optrs.find(c) != optrs.end()) {
            if (!item.empty()) {
                expr.push_back(item);
                item.clear();
            }
            expr.push_back(string(1, c));
        }
        else {
            item.push_back(c);
        }
    }
    if (!item.empty()) expr.push_back(item);
    return expr;
}
bool isOperator(const string& str) {
    if (str.size() != 1) return false;
    unordered_set<char> optrs = { '!','|','&','<','>','(',')' };
    return optrs.find(str[0]) != optrs.end();
}
vector<string> calIff(vector<string> expr) {
    while (true) {
        int i = expr.size() - 1;
        while (i >= 0) {
            if (expr[i] == "=") break;
            --i;
        }
        if (i < 0) return expr;
        //find left end
        int cnt = 0;
        int bg = i - 1;
        while (bg >= 0) {
            if (expr[bg] == ")") ++cnt;
            else if (expr[bg] == "(") --cnt;
            if (cnt < 0) break;
            --bg;
        }
        ++bg;

        //find right end
        cnt = 0;
        int ed = i + 1;
        while (ed < expr.size()) {
            if (expr[ed] == "(") ++cnt;
            else if (expr[ed] == ")") --cnt;
            if (cnt < 0) break;
            ++ed;
        }
        --ed;

        //calculate <=>
        vector<string> ans(expr.begin(), expr.begin() + bg);// left(
        ans.push_back("(");// left(,(
        ans.push_back("(");// left(,(,(
        ans.insert(ans.end(), expr.begin() + bg, expr.begin() + i);// left(,(,(,expr1
        ans.push_back(")");// left(,(,(,expr1,)
        ans.push_back(">");// left(,(,(,expr1,),=>
        ans.push_back("(");// left(,(,(,expr1,),=>,(
        ans.insert(ans.end(), expr.begin() + i + 1, expr.begin() + ed + 1);// left(,(,(,expr1,),=>,(,expr2
        ans.push_back(")");// left(,(,(,expr1,),=>,(,expr2,)
        ans.push_back(")");// left(,(,(,expr1,),=>,(,expr2,),)
        ans.push_back("&");// left(,(,(,expr1,),=>,(,expr2,),),&
        ans.push_back("(");// left(,(,(,expr1,),=>,(,expr2,),),&,(
        ans.push_back("(");// left(,(,(,expr1,),=>,(,expr2,),),&,(,(
        ans.insert(ans.end(), expr.begin() + i + 1, expr.begin() + ed + 1);// left(,(,(,expr1,),=>,(,expr2,),),&,(,(,expr2
        ans.push_back(")");// left(,(,(,expr1,),=>,(,expr2,),),&,(,(,expr2,)
        ans.push_back(">");// left(,(,(,expr1,),=>,(,expr2,),),&,(,expr2,),=>
        ans.push_back("(");// left(,(,(,expr1,),=>,(,expr2,),),&,(,expr2,),=>,(
        ans.insert(ans.end(), expr.begin() + bg, expr.begin() + i);// left(,(,(,expr1,),=>,(,expr2,),),&,(,(,expr2,),=>,(,expr1
        ans.push_back(")");// left(,(,(,expr1,),=>,(,expr2,),),&,(,(,expr2,),=>,(,expr1,)
        ans.push_back(")");// left(,(,(,expr1,),=>,(,expr2,),),&,(,(,expr2,),=>,(,expr1,),)
        ans.insert(ans.end(), expr.begin() + ed + 1, expr.end());// left(,(,(,expr1,),=>,(,expr2,),),&,(,(,expr2,),=>,(,expr1,),),)right
        swap(expr, ans);
        //cout << "In calIff: ";//////////////////////////////
        //printExpr(expr);/////////////////////////////////////
    }

}
vector<string> calImply(vector<string> expr) {
    while (true) {
        int i = expr.size() - 1;
        while (i >= 0) {
            if (expr[i] == ">") break;
            --i;
        }
        if (i < 0) return expr;
        //find left end
        int cnt = 0;
        int bg = i - 1;
        while (bg >= 0) {
            if (expr[bg] == ")") ++cnt;
            else if (expr[bg] == "(") --cnt;
            if (cnt < 0) break;
            --bg;
        }
        ++bg;

        //find right end
        cnt = 0;
        int ed = i + 1;
        while (ed < expr.size()) {
            if (expr[ed] == "(") ++cnt;
            else if (expr[ed] == ")") --cnt;
            if (cnt < 0) break;
            ++ed;
        }
        --ed;

        //calculate =>
        vector<string> ans(expr.begin(), expr.begin() + bg);// left(
        ans.push_back("(");// left(,(
        ans.push_back("!");// left(,(,!,(,expr1
        ans.push_back("(");// left(,(,!,(,expr1
        ans.insert(ans.end(), expr.begin() + bg, expr.begin() + i);// left(,(,!,(,expr1
        ans.push_back(")");// left(,(,!,(,expr1,)
        ans.push_back("|");// left(,(,!,(,expr1,),|
        ans.push_back("(");// left(,(,!,(,expr1,),|,(
        ans.insert(ans.end(), expr.begin() + i + 1, expr.begin() + ed + 1);// left(,(,!,(,expr1,),|,(,expr2
        ans.push_back(")");// left(,(,!,(,expr1,),|,(,expr2,)
        ans.push_back(")");// left(,(,!,(,expr1,),|,(,expr2,),)
        ans.insert(ans.end(), expr.begin() + ed + 1, expr.end());// left(,(,!,(,expr1,),|,(,expr2,),),)right
        swap(expr, ans);
        //cout << "In calImply: ";//////////////////////////////
        //printExpr(expr);/////////////////////////////////////
    }
}

void printExpr(const vector<string>& expr) {
    for (const auto& item : expr) cout << item;
    cout << endl;
}
//-------------------------- AND-OR-NOT tree ---------------------------------

void printTree(node* tree) {
    if (!tree) return;
    if (tree->is_not) cout << "!";
    cout << "{";
    printTree(tree->left);
    cout << tree->val;
    printTree(tree->right);
    cout << "}";
}
node* deepCopy(node* tree) {
    if (!tree) return nullptr;
    node* tree_cp = new node();
    tree_cp->is_not = tree->is_not;
    tree_cp->val = tree->val;
    tree_cp->left = deepCopy(tree->left);
    tree_cp->right = deepCopy(tree->right);
    return tree_cp;
}
void delTree(node** ptr_tree) {
    node* tree = *ptr_tree;
    if (tree->left) delTree(&(tree->left));
    if (tree->right) delTree(&(tree->right));
    delete tree;
    ptr_tree = nullptr;
}
node* initTree(vector<string> expr) {
    node* root = new node();
    while (true) {
        // can "|" be the root?
        int i = expr.size() - 1;
        int cnt = 0;
        while (i >= 0) {
            if (expr[i] == ")") ++cnt;
            else if (expr[i] == "(") --cnt;
            else if (expr[i] == "|" && cnt == 0) break;
            --i;
        }
        if (i >= 0) {
            root->val = "|";
            root->left = initTree(vector<string>(expr.begin(), expr.begin() + i));
            root->right = initTree(vector<string>(expr.begin() + i + 1, expr.end()));
            return root;
        }
        //can "&" be the root?
        i = expr.size() - 1;
        cnt = 0;
        while (i >= 0) {
            if (expr[i] == ")") ++cnt;
            else if (expr[i] == "(") --cnt;
            else if (expr[i] == "&" && cnt == 0) break;
            --i;
        }
        if (i >= 0) {
            root->val = "&";
            root->left = initTree(vector<string>(expr.begin(), expr.begin() + i));
            root->right = initTree(vector<string>(expr.begin() + i + 1, expr.end()));
            return root;
        }
        //need to remove "!"?
        if (expr[0] == "!") {
            expr = vector<string>(expr.begin() + 1, expr.end());
            root->is_not = !root->is_not;
            continue;
        }
        //need to remove "(" and ")"?
        if (expr[0] == "(") {
            if (expr.back() != ")") {
                cout << "Error: bracket not match: " << endl;
                printExpr(expr);
                cout << "----------------------------------" << endl;
                exit(0);
            }
            expr = vector<string>(expr.begin() + 1, expr.end() - 1);
            continue;
        }
        //just as an atom
        if (expr.size() != 1) {
            cout << "Error: not an atom: " << endl;
            printExpr(expr);
            cout << "----------------------------------" << endl;
            exit(0);
        }
        root->val = expr[0];
        return root;
    }
}

void calNot(node* tree, bool not_again) {
    if (!tree) return;
    if (not_again == tree->is_not) not_again = false;
    else not_again = true;
    if (!tree->left && !tree->right) tree->is_not = not_again;
    else {
        tree->is_not = false;
        if (not_again) {
            if (tree->val == "|") tree->val = "&";
            else tree->val = "|";
        }
    }
    calNot(tree->left, not_again);
    calNot(tree->right, not_again);
}

bool isFullyDistributed(node* tree) {
    if (!tree || !isOperator(tree->val)) return true;
    if (!isFullyDistributed(tree->left)) return false;
    if (!isFullyDistributed(tree->right)) return false;
    if (tree->val == "&") return true;
    if (tree->left->val == "&" || tree->right->val == "&") return false;
    return true;
}

void distributeOr(node* tree) {
    while (!isFullyDistributed(tree)) {
        if (!tree || !isOperator(tree->val)) return;
        if (!isOperator(tree->left->val) && !isOperator(tree->right->val)) return;
        distributeOr(tree->left);
        distributeOr(tree->right);
        if (tree->left->val == "&" && tree->val == "|") {
            node* pb = tree->left->right;
            node* pc = tree->right;
            tree->right = new node();
            tree->right->is_not = tree->left->is_not;
            tree->right->val = "|";
            tree->right->left = pb;
            tree->right->right = pc;
            tree->left->right = deepCopy(pc);
            swap(tree->val, tree->left->val);
        }
        if (tree->right->val == "&" && tree->val == "|") {
            node* pb = tree->right->left;
            node* pc = tree->left;
            tree->left = new node();
            tree->left->is_not = tree->right->is_not;
            tree->left->val = "|";
            tree->left->right = pb;
            tree->left->left = pc;
            tree->right->left = deepCopy(pc);
            swap(tree->val, tree->right->val);
        }
    }
}

void tree2CNF(node* tree, vector<string>& lines, string& line) {
    if (!tree) return;

    tree2CNF(tree->left, lines, line);

    if (tree->val == "|") line.push_back(' ');
    else if (tree->val == "&") {
        if (!line.empty()) lines.push_back(line);
        line.clear();
    }
    else {
        if (tree->is_not) line.push_back('!');
        line += tree->val;
    }

    tree2CNF(tree->right, lines, line);
}

vector<string> tree2CNFDriver(node* tree) {
    vector<string> lines;
    string line;
    tree2CNF(tree, lines, line);
    if (!line.empty())lines.push_back(line);
    return lines;
}

vector<string> runBNF2CNF(const string& bnf_line) {
    vector<string> expr = str2Expr(bnf_line);
    expr = calIff(expr);
    expr = calImply(expr);
    node* tree = initTree(expr);
    calNot(tree, false);
    distributeOr(tree);
    vector<string> lines = tree2CNFDriver(tree);
    delTree(&tree);
    return lines;
}

#endif