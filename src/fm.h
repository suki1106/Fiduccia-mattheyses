#ifndef FM_H
#define FM_H

#include <vector>
#include <unordered_map>
#include <iostream>
#include "bl.h"
#include "cell.h"
#include "net.h"

using namespace std;
// implementation of Fiduccia-Mattheyses

class Fm {
public:

    Fm(string& in, string& out) 
        : in_file_(in) , out_file_(out), cut_size_(0), net_num_(0), cell_num_(0), max_pin_(0) {
            part_size_[0] = 0;
            part_size_[1] = 0;
            this->parseInput();
            BucketList_[0] = new BucketList(max_pin_);
            BucketList_[1] = new BucketList(max_pin_);
            lower_bound_ = ((1-bFactor_)/2)*cell_num_;
            upper_bound_ = ((1+bFactor_)/2)*cell_num_;
        }
 
    ~Fm(){};
    void parseInput();
    void initialize();
    void Solve();
    void writeResult();
    void calCutSize();
    void computeGain();
    void updateGain(int);
    bool isbalancedMove(int);
    int getCutSize(){return cut_size_;}

private:

    string in_file_;
    string out_file_;
    double bFactor_;
    double lower_bound_;
    double upper_bound_;
    int cut_size_ ;
    int net_num_;
    int cell_num_;
    int max_pin_;
    int part_size_ [2];

    vector<Net> net_list_;
    vector<Cell> cell_list_;

    unordered_map<int,Node*> cellid2node_ ; // use cell id to access node in bucket list
    unordered_map<string,int> cellname2id_ ; 
    unordered_map<int,string> id2cellname_ ;
    unordered_map<string,int> netname2id_ ;

    BucketList* BucketList_ [2]; 
};


#endif