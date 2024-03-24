#ifndef CELL_H
#define CELL_H

#include <iostream>
#include <vector>

using namespace std;

class Cell {
public:

    Cell(){}
    Cell(int cell_id) : id_(cell_id), part_(0), gain_(0) {}
    ~Cell(){}

    vector<int>& getNet(){return net_list_;}
    void incPinNum(){ ++num_pin_;}
    void incGain(){++gain_;}
    void decGain(){--gain_;}
    void setGain(int gain){gain_ = gain;}
    void lock(){is_free_ = false;}
    void unlock(){is_free_ = true;}
    void addNet(int net_id){net_list_.push_back(net_id);}
    void move(){part_ = !part_;}
    bool getPart(){return part_;}
    bool isFree(){return is_free_;}
    int getGain(){return gain_;}
    int getId(){return id_;}
    int getPinNum(){return num_pin_;}
  

private:

    
    bool is_free_ ;
    vector<int> net_list_ ;
    int id_ ;
    int num_pin_ ;
    int gain_ ; 
    bool part_ ; // belong to which partition


};

#endif