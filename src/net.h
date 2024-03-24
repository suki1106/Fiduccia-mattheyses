#ifndef NET_H
#define NET_H

#include <iostream>
#include <vector>
using namespace std;


class Net {
public:
	
	Net(string& name) : name_(name) {
		part_size_[0] = 0;
		part_size_[1] = 0;
	}

	~Net(){}

	void addCell(int cell_id){cell_list_.push_back(cell_id);}
	void incPartSize(int part){++part_size_[part];}
	void decPartSize(int part){--part_size_[part];}
	int getPartSize(int part){return part_size_[part];}
	vector<int>& getCell(){ return cell_list_;}

private:

	int part_size_ [2];
	string name_;
	vector<int> cell_list_;


};



#endif