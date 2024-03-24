#include <iostream>
#include "fm.h"

using namespace std;

int main(int argc,char** argv ) {
    if(argc == 3){
        string in_file = argv[1];
        string out_file = argv[2];

        Fm fm(in_file,out_file);

        fm.initialize();
        fm.writeResult();
        return 0;

        fm.Solve();

    }else{
        cerr << "parameter error" << endl;
    }

    return 0;
}