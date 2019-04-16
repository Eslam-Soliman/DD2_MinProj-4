#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#define WIN32
#include "lp_lib.h"

using namespace std;

vector<string> Extract(string line){
    int l = 0,r;
    while(l < line.length() && line[l] == ' ' || line[l] == '\t')l++;
    r = l;
    vector<string> tokens;

    while(l < line.length()){
        while(r < line.length() && line[r] != ' ' && line[r] != '\t') r++;
        tokens.push_back(line.substr(l, r - l));
        while(r < line.length() && line[r] == ' ' || line[r] == '\t') r++;
        l = r;
    }
    return tokens;
}

struct hard{
    string name;
    int w, h;
};

struct soft{
    string name;
    int area;
};

vector<hard> read_hard(ifstream &file){
    string buf;
    getline(file, buf);
    bool ended = 0;
    vector<hard> hard_blocks;
    hard block;
    while(!ended){
        vector<string> tmp;
        tmp = Extract(buf);
        if(tmp.size()){
            if(tmp[0] == "endhard") ended = 1;
            else if(tmp.size() != 3){
                cout << "Invalid number of parameters for the block " << tmp[0] << endl;
                exit(1);
            }
            else{
                block.name = tmp[0];
                block.w = atoi(tmp[1].c_str());
                block.h = atoi(tmp[2].c_str());
                hard_blocks.push_back(block);
            }
        }
        getline(file, buf);
    }
    return hard_blocks;
}

vector<soft> read_soft(ifstream &file){
    string buf;
    getline(file, buf);
    bool ended = 0;
    vector<soft> soft_blocks;
    soft block;
    while(!ended){
        vector<string> tmp;
        tmp = Extract(buf);
        if(tmp.size()){
            if(tmp[0] == "endsoft") ended = 1;
            else if(tmp.size() != 2){
                cout << "Invalid number of parameters for the block " << tmp[0] << endl;
                exit(1);
            }
            else{
                block.name = tmp[0];
                block.area = atoi(tmp[1].c_str());
                soft_blocks.push_back(block);
            }
        }
        getline(file, buf);
    }
    return soft_blocks;
}

vector<hard> read_pads(ifstream &file){
    string buf;
    bool ended = 0;
    vector<hard> pads;
    hard pad;
    getline(file, buf);
    while(!ended){
        vector<string> tmp;
        tmp = Extract(buf);
        if(tmp.size()){
            if(tmp[0] == "endpads") ended = 1;
            else if(tmp.size() != 3){
                cout << "Invalid number of parameters for the block " << tmp[0] << endl;
                exit(1);
            }
            else{
                pad.name = tmp[0];
                pad.w = atoi(tmp[1].c_str());
                pad.h = atoi(tmp[2].c_str());
                pads.push_back(pad);
            }
        }
        getline(file, buf);
    }
    return pads;
}

int main(){
    ifstream input;
    input.open("test.txt");
    string buf;
    vector<hard> hardBlocks, pads;
    vector<soft> softBlocks;
    getline(input, buf);
    while(!input.eof()){
        vector<string> command;
        command = Extract(buf);
        if(command.size() == 1){
            if(command[0] == "hardbegin") hardBlocks = read_hard(input);
            else if(command[0] == "softbegin") softBlocks = read_soft(input);
            else if(command[0] == "padsbegin") pads = read_pads(input);
            else {
                cout << "Invalid token: " << command[0] << endl;
            }
        }
        else if(command.size() > 1){
            cout << "Invalid list of tokens, begins with: " << command[0] << endl;
        }
        getline(input, buf);
    }

    for(int i = 0; i < hardBlocks.size(); i++)cout << "hard block with the name \"" << hardBlocks[i].name << "\" has a width of " << hardBlocks[i].w << ", and a height of " << hardBlocks[i].h << endl;
    for(int i = 0; i < softBlocks.size(); i++)cout << "soft block with the name \"" << softBlocks[i].name << "\" has an area of " << softBlocks[i].area << hardBlocks[i].h << endl;
    for(int i = 0; i < pads.size(); i++)cout << "pad with the name \"" << pads[i].name << "\" has a width of " << pads[i].w << ", and a height of " << pads[i].h << endl;


    //Using lp_solve API to solve for the objective function
	/*
    lprec *lp;
    HINSTANCE lpsolve;
    delete_lp_func *delete_lp;
    read_LP_func *read_LP;
    solve_func *solve;
    get_Nrows_func *get_Nrows;
    get_Ncolumns_func *get_Ncolumns;
    get_objective_func *get_objective;
    get_col_name_func *get_col_name;
    get_var_primalresult_func *get_var_primalresult;

    lpsolve = LoadLibrary("lpsolve55.dll");

    if (lpsolve == NULL) {
        printf("Unable to load lpsolve shared library\n");
        return(-1);
    }
    delete_lp = (delete_lp_func *) GetProcAddress(lpsolve, "delete_lp");
    read_LP = (read_LP_func *) GetProcAddress(lpsolve, "read_LP");
    solve = (solve_func *) GetProcAddress(lpsolve, "solve");
    get_Nrows = (get_Nrows_func *) GetProcAddress(lpsolve, "get_Nrows");
    get_Ncolumns = (get_Ncolumns_func *) GetProcAddress(lpsolve, "get_Ncolumns");
    get_objective = (get_objective_func *) GetProcAddress(lpsolve, "get_objective");
    get_col_name = (get_col_name_func *) GetProcAddress(lpsolve, "get_col_name");
    get_var_primalresult = (get_var_primalresult_func *) GetProcAddress(lpsolve, "get_var_primalresult");


    char path[100] = "LP_Example.txt";
    char name[15] = "solve_module";
    double ans[35];
    lp = read_LP(path, 0, name);
    solve(lp);
    int Nrows = get_Nrows(lp);
    int Ncols = get_Ncolumns(lp);

    vector <pair<string, double> > Results(Ncols);

    for(int i = 0; i < Ncols; i++){
        Results[i] = make_pair(get_col_name(lp, i + 1), get_var_primalresult(lp, i + 1 + Nrows));
    }

    for(int i = 0; i < Results.size(); i++)cout << Results[i].first << " = " << Results[i].second << endl;

    delete_lp(lp);

    FreeLibrary(lpsolve);
	*/
}
