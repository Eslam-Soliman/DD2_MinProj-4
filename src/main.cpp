#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#define WIN32
#define TMPBUF "_temp_buf.txt"
#include "lp_lib.h"

using namespace std;

struct hard{
    string name;
    int w, h;
};

struct soft{
    string name;
    int area, AR;
    double w_min, w_max, d, c;
};

bool sort_pads(hard a, hard b){
    return a.w <= b.w;
}

int MAX = 0, eq = 0;
int hardN, softN;
double max_d;
double W = 0;
vector<hard> hardBlocks, pads;
vector<soft> softBlocks;
vector<string> equations, init, declr;

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

int to_closest(double x){
    if(x - (int)x > 0.5) return (int(x) + 1);
    else return int(x);
}

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
            if(tmp[0] == "endhard" || tmp[0] == "endpads") ended = 1;
            else if(tmp.size() != 3){
                cout << "Invalid number of parameters for the block " << tmp[0] << endl;
                exit(1);
            }
            else{
                block.name = tmp[0];
                block.h = atoi(tmp[1].c_str());
                block.w = atoi(tmp[2].c_str());
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
            else if(tmp.size() != 3){
                cout << "Invalid number of parameters for the block " << tmp[0] << endl;
                exit(1);
            }
            else{
                block.name = tmp[0];
                block.area = atoi(tmp[1].c_str());
                block.AR = atoi(tmp[2].c_str());
                block.w_min = sqrt(1.0 * block.area / block.AR);
                block.w_max = sqrt(1.0 * block.area * block.AR);
                block.c = (2 * block.area / (sqrt(1.0 * block.area * block.AR)));
                block.d = -1.0 / block.AR;
                soft_blocks.push_back(block);
            }
        }
        getline(file, buf);
    }
    return soft_blocks;
}

void write_single(int i){
    if(i < hardN){
        equations.push_back("c" + to_string(eq++) + ": x" + to_string(i) + " >= 0;\n");
        equations.push_back("c" + to_string(eq++) + ": y" + to_string(i) + " >= 0;\n");
        equations.push_back("c" + to_string(eq++) + ": x" + to_string(i) + " + " + to_string(hardBlocks[i].h) + " r" + to_string(i) + " + " + to_string(hardBlocks[i].w) + " - " + to_string(hardBlocks[i].w) + " r" + to_string(i) + " <= " + to_string(W) + ";\n");
        equations.push_back("c" + to_string(eq++) + ": y" + to_string(i) + " + " + to_string(hardBlocks[i].w) + " r" + to_string(i) + " + " + to_string(hardBlocks[i].h) + " - " + to_string(hardBlocks[i].h) + " r" + to_string(i) + " <= Y;\n");
        init.push_back("r" + to_string(i) + " <= 1;\n");
        declr.push_back(" r" + to_string(i) + ",");
    }
    else{
        equations.push_back("c" + to_string(eq++) + ": w" + to_string(i) + " <= " + to_string(softBlocks[i - hardN].w_max) + ";\n");
        equations.push_back("c" + to_string(eq++) + ": w" + to_string(i) + " >= " + to_string(softBlocks[i - hardN].w_min) + ";\n");
        equations.push_back("c" + to_string(eq++) + ": x" + to_string(i) + " + w" + to_string(i) + " <= " + to_string(W) + ";\n");
        equations.push_back("c" + to_string(eq++) + ": y" + to_string(i) + " + " + to_string(softBlocks[i - hardN].d) + " w" + to_string(i) + " + " + to_string(softBlocks[i - hardN].c) + " <= Y;\n");
    }
}

void write_double(int i, int j){
    if(i < hardN && j < hardN){
        equations.push_back("c" + to_string(eq++) + ": " + "x" + to_string(i) + " + " + to_string(hardBlocks[i].h) + " r" + to_string(i) + " + " + to_string(hardBlocks[i].w) + " - " + to_string(hardBlocks[i].w) + " r" + to_string(i) + " <= x" + to_string(j) + " + " + to_string(MAX) + " p" + to_string(i) + to_string(j) + " + " + to_string(MAX) + " q" + to_string(i) + to_string(j) + ";\n");
        equations.push_back("c" + to_string(eq++) + ": " + "y" + to_string(i) + " + " + to_string(hardBlocks[i].w) + " r" + to_string(i) + " + " + to_string(hardBlocks[i].h) + " - " + to_string(hardBlocks[i].h) + " r" + to_string(i) + " <= y" + to_string(j) + " + " + to_string(MAX) + " + " + to_string(MAX) + " p" + to_string(i) + to_string(j) + " - " + to_string(MAX) + " q" + to_string(i) + to_string(j) + ";\n");
        equations.push_back("c" + to_string(eq++) + ": " + "x" + to_string(i) + " - " + to_string(hardBlocks[j].h) + " r" + to_string(j) + " - " + to_string(hardBlocks[j].w) + " + " + to_string(hardBlocks[j].w) + " r" + to_string(j) + " >= x" + to_string(j) + " - " + to_string(MAX) + " + " + to_string(MAX) + " p" + to_string(i) + to_string(j) + " - " + to_string(MAX) + " q" + to_string(i) + to_string(j) + ";\n");
        equations.push_back("c" + to_string(eq++) + ": " + "y" + to_string(i) + " - " + to_string(hardBlocks[j].w) + " r" + to_string(j) + " - " + to_string(hardBlocks[j].h) + " + " + to_string(hardBlocks[j].h) + " r" + to_string(j) + " >= y" + to_string(j) + " - " + to_string(2 * MAX) + " + " + to_string(MAX) + " p" + to_string(i) + to_string(j) + " + " + to_string(MAX) + " q" + to_string(i) + to_string(j) + ";\n\n");
        init.push_back("p" + to_string(i) + to_string(j) + " <= 1;\n");
        init.push_back("q" + to_string(i) + to_string(j) + " <= 1;\n");
        declr.push_back(" p" + to_string(i) + to_string(j) + ", q" + to_string(i) + to_string(j) + ",");
    }
    else if(i < hardN && j >= hardN){
        equations.push_back("c" + to_string(eq++) + ": " + "x" + to_string(i) + " + " + to_string(hardBlocks[i].h) + " r" + to_string(i) + " + " + to_string(hardBlocks[i].w) + " - " + to_string(hardBlocks[i].w) + " r" + to_string(i) + " <= x" + to_string(j) + " + " + to_string(MAX) + " p" + to_string(i) + to_string(j) + " + " + to_string(MAX) + " q" + to_string(i) + to_string(j) + ";\n");
        equations.push_back("c" + to_string(eq++) + ": " + "y" + to_string(i) + " + " + to_string(hardBlocks[i].w) + " r" + to_string(i) + " + " + to_string(hardBlocks[i].h) + " - " + to_string(hardBlocks[i].h) + " r" + to_string(i) + " <= y" + to_string(j) + " + " + to_string(MAX) + " + " + to_string(MAX) + " p" + to_string(i) + to_string(j) + " - " + to_string(MAX) + " q" + to_string(i) + to_string(j) + ";\n");
        equations.push_back("c" + to_string(eq++) + ": " + "x" + to_string(i) + " - w" + to_string(j) + " >= x" + to_string(j) + " - " + to_string(MAX) + " + " + to_string(MAX) + " p" + to_string(i) + to_string(j) + " - " + to_string(MAX) + " q" + to_string(i) + to_string(j) + ";\n");
        equations.push_back("c" + to_string(eq++) + ": " + "y" + to_string(i) + " - " + to_string(softBlocks[j - hardN].d) + " w" + to_string(j) + " - " + to_string(softBlocks[j - hardN].c) + " >= y" + to_string(j) + " - " + to_string(2 * MAX) + " + " + to_string(MAX) + " p" + to_string(i) + to_string(j) + " + " + to_string(MAX) + " q" + to_string(i) + to_string(j) + ";\n");
        init.push_back("p" + to_string(i) + to_string(j) + " <= 1;\n");
        init.push_back("q" + to_string(i) + to_string(j) + " <= 1;\n");
        declr.push_back(" p" + to_string(i) + to_string(j) + ", q" + to_string(i) + to_string(j) + ",");
    }
    else{
        equations.push_back("c" + to_string(eq++) + ": x" + to_string(i) + " + w" + to_string(i) + " <= x" + to_string(j) + " + " + to_string(MAX) + " p" + to_string(i) + to_string(j) + " + " + to_string(MAX) + " q" + to_string(i) + to_string(j) + ";\n");
        equations.push_back("c" + to_string(eq++) + ": y" + to_string(i) + " + " + to_string(softBlocks[i - hardN].d) + " w" + to_string(i) + " + " + to_string(softBlocks[i - hardN].c) + " <= y" + to_string(j) + " + " + to_string(MAX) + " + " + to_string(MAX) + " p" + to_string(i) + to_string(j) + " - " + to_string(MAX) + " q" + to_string(i) + to_string(j) + ";\n");
        equations.push_back("c" + to_string(eq++) + ": " + "x" + to_string(i) + " - w" + to_string(j) + " >= x" + to_string(j) + " - " + to_string(MAX) + " + " + to_string(MAX) + " p" + to_string(i) + to_string(j) + " - " + to_string(MAX) + " q" + to_string(i) + to_string(j) + ";\n");
        equations.push_back("c" + to_string(eq++) + ": " + "y" + to_string(i) + " - " + to_string(softBlocks[j - hardN].d) + " w" + to_string(j) + " - " + to_string(softBlocks[j - hardN].c) + " >= y" + to_string(j) + " - " + to_string(2 * MAX) + " + " + to_string(MAX) + " p" + to_string(i) + to_string(j) + " + " + to_string(MAX) + " q" + to_string(i) + to_string(j) + ";\n");
        init.push_back("p" + to_string(i) + to_string(j) + " <= 1;\n");
        init.push_back("q" + to_string(i) + to_string(j) + " <= 1;\n");
        declr.push_back(" p" + to_string(i) + to_string(j) + ", q" + to_string(i) + to_string(j) + ",");
    }
}

int main(){

    string path;
    char module_name[100];
    cout << "Enter a name for the solving module (100 characters max): ";
    //scanf("%s", &module_name);
    fgets(module_name, 100, stdin);
    string module_name_cpp = module_name;
    module_name_cpp = module_name_cpp.substr(0, module_name_cpp.length() - 1);
    cout << "Enter the path of the dimensions file (use '/' for separation): ";
    cin >> path;
    ifstream input;
    input.open(path);
    if(!input.is_open()){
        cout << "Error in opening the dimensions file.\n" ;
        exit(-1);
    }
    string buf;
    getline(input, buf);
    while(!input.eof()){
        vector<string> command;
        command = Extract(buf);
        if(command.size() == 1){
            if(command[0] == "hardbegin") hardBlocks = read_hard(input);
            else if(command[0] == "softbegin") softBlocks = read_soft(input);
            else if(command[0] == "padsbegin") pads = read_hard(input);
            else {
                cout << "Invalid token: " << command[0] << endl;
            }
        }
        else if(command.size() > 1){
            cout << "Invalid list of tokens, begins with: " << command[0] << endl;
        }
        getline(input, buf);
    }

    hardN = hardBlocks.size(), softN = softBlocks.size();


    for(int i = 0; i < hardBlocks.size(); i++){
        W += hardBlocks[i].h * hardBlocks[i].w;
        MAX += max(hardBlocks[i].h, hardBlocks[i].w);
    }
    for(int i = 0; i < softBlocks.size(); i++){
        W += softBlocks[i].area;
        MAX += softBlocks[i].w_max;
    }
    W = sqrt(W);
    W = to_closest(W);

    //Using lp_solve API to solve for the objective function
    map <string, double> Results;
    ofstream output;
    string output_file = "Floorplanning_" + module_name_cpp + "_Results.txt";
    output.open(output_file);
    if(!output.is_open()){
        cout << "Error: cannot open an output file." << endl;
        exit(-2);
    }
    for(int iter = 0; iter < 2; iter++){
        output.close();
        output.open(output_file);
        equations.clear(); init.clear(); declr.clear();
        if(iter > 0){
            for(int i = 0; i < softBlocks.size(); i++){
                softBlocks[i].area = ceil(1.0 * softBlocks[i].area * softBlocks[i].area / (Results["w" + to_string(i + hardN)] * (Results["w" + to_string(i + hardN)] * softBlocks[i].d + softBlocks[i].c)));
                softBlocks[i].w_min = sqrt(1.0 * softBlocks[i].area / softBlocks[i].AR);
                softBlocks[i].w_max = sqrt(1.0 * softBlocks[i].area * softBlocks[i].AR);
                softBlocks[i].c = (2 * softBlocks[i].area / (sqrt(1.0 * softBlocks[i].area * softBlocks[i].AR)));
                softBlocks[i].d = -1.0 / softBlocks[i].AR;
            }
        }
        equations.push_back("min: Y;\n\n");
        declr.push_back("int");
        for(int i = 0; i < hardN + softN; i++){
            write_single(i);
        }
        for(int i = 0; i < hardN + softN; i++){
            for(int j = i + 1; j < hardN + softN; j++){
                write_double(i, j);
            }
        }

        declr[declr.size() - 1][declr[declr.size() - 1].length() - 1] = ';';

        ofstream temp_buf;
        temp_buf.open(TMPBUF);
        if(temp_buf.is_open()){
            for(int i = 0; i < equations.size(); i++)temp_buf << equations[i];
            for(int i = 0; i < init.size(); i++)temp_buf << init[i];
            if(declr.size() > 1) for(int i = 0; i < declr.size(); i++)temp_buf << declr[i];
            temp_buf.close();
        }
        else{
            cout << "Error: run the program in a public directory." << endl;
            exit(2);
        }


        lprec *lp;
        HINSTANCE lpsolve;
        delete_lp_func *delete_lp;
        read_LP_func *read_LP;
        solve_func *solve;
        get_Nrows_func *get_Nrows;
        get_Ncolumns_func *get_Ncolumns;
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
        get_col_name = (get_col_name_func *) GetProcAddress(lpsolve, "get_col_name");
        get_var_primalresult = (get_var_primalresult_func *) GetProcAddress(lpsolve, "get_var_primalresult");


        char _temp_buf[100] = TMPBUF;
        lp = read_LP(_temp_buf, 5, module_name);
        //remove(TMPBUF);
        int result = solve(lp);
        if(result == 0 || result == 1){
            int Nrows = get_Nrows(lp);
            int Ncols = get_Ncolumns(lp);


            for(int i = 0; i < Ncols; i++) Results[get_col_name(lp, i + 1)] = get_var_primalresult(lp, i + 1 + Nrows);

            max_d = 0;
            double x_pos, y_pos, width, height;
            if(hardBlocks.size()) output << "HARD BLOCKS POSITIONING:\n\n";
            for(int i = 0; i < hardBlocks.size(); i++){
                output << "Block \"" << hardBlocks[i].name << "\"{\n";
                x_pos = Results["x" + to_string(i)];
                y_pos = Results["y" + to_string(i)];
                width = ((Results["r" + to_string(i)] == 0)? hardBlocks[i].w : hardBlocks[i].h);
                height = ((Results["r" + to_string(i)] == 1)? hardBlocks[i].w : hardBlocks[i].h);
                output << "x-coordinates = " << x_pos << '\n';
                output << "y-coordinates = " << y_pos << '\n';
                output << "width = " << width << '\n';
                output << "height = " << height << "\n}\n";
                max_d = max(max_d, x_pos + width);
                max_d = max(max_d, y_pos + height);
            }
            if(softBlocks.size()) output << "\nSOFT BLOCKS POSITIONING:\n\n";
            for(int i = 0; i < softBlocks.size(); i++){
                x_pos = Results["x" + to_string(i + hardN)];
                y_pos = Results["y" + to_string(i + hardN)];
                width = Results["w" + to_string(i + hardN)];
                height = Results["w" + to_string(i + hardN)] * softBlocks[i].d + softBlocks[i].c ;
                output << "Block \"" << softBlocks[i].name << "\"{\n";
                output << "x-coordinates = " << x_pos << '\n';
                output << "y-coordinates = " << y_pos << '\n';
                output << "width = " << width << '\n';
                output << "height = " << height << "\n}\n\n";
                max_d = max(max_d, x_pos + width);
                max_d = max(max_d, y_pos + height);
            }

        }
         else{
            cout << "Error: Couldn't find a suitable solution.\n";
            exit(1);
        }
        delete_lp(lp);
        FreeLibrary(lpsolve);
    }

    sort(pads.begin(), pads.end(), sort_pads);

    double pad_max = 0, pad_sum = 0;
    for(int i = 0; i < pads.size(); i++){
        pad_max = max(pad_max, 1.0 * pads[i].w);
        pad_sum += pads[i].w;
    }
    double core_side = max(max_d, 1.0 * max(1.0 * ceil((1.0 * pad_sum) / 4.0), pad_max));
    output << "The Chip is " << ((core_side > max_d)? "pad limited" : "core limited") << endl;
    output << ", with a core side length of " << core_side << endl;
    int pad_dist = (core_side * 4 - pad_sum) / (pads.size() - 1);
    int side = 4;
    double crnt_side;
    int index = 0;
    int order;
    if(pads.size()){
        output << "Pads are put on the core sides with a distance between pads on the same side equals to " << pad_dist << endl;
        output << "PADS POSITIONING:\n";
        for(int i = 0; i < side; i++){
            crnt_side = core_side;
            order = 1;
            while(index < pads.size() && crnt_side >= pads[index].w){
                crnt_side -= pads[index].w;
                output << pads[index++].name << " is number " << order++ << " in the side ";
                switch(i){
                case 0: output << "\"up\"\n"; break;
                case 1: output << "\"right\"\n"; break;
                case 2: output << "\"down\"\n"; break;
                case 3: output << "\"left\"\n"; break;
                }
                if(crnt_side >= pad_dist + pads[index].w) crnt_side -= pad_dist;
                else crnt_side = 0;
            }
        }
    }
    output << "Total Chip side = " << core_side + 2 * pad_max << endl;
    output.close();
}

