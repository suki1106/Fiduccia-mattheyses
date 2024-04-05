
#include "fm.h"
#include <iostream>


using namespace std;

int main(int argc,char** argv ) {
    if(argc == 3){
        string in_file = argv[1];
        string out_file = argv[2];

        Fm fm(in_file,out_file);

        clock_t start,end;
        
        start = clock();

        fm.Solve();

        end = clock();

        double time = double(end-start) / double(CLOCKS_PER_SEC);

        cout << "Time taken: " << time << endl;

        fm.writeResult();

    }else{
        cerr << "parameter error" << endl;
    }

    return 0;
}