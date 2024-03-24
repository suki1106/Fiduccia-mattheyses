#include "fm.h"
#include <fstream>

using namespace std;

void Fm::parseInput(){
    ifstream in(in_file_);

    if(in.is_open()){

        string str;
        
        //<Balance Degree>
        in >> bFactor_;

        while(in >> str){
            if(str == "NET"){
                string net_name, cell_name = "";
                in >> net_name;

                int net_id = net_num_;
                netname2id_[net_name] = net_id;
                net_list_.push_back(Net(net_name));
                while(in >> cell_name){
                    if(cell_name != ";"){
                        if(cellname2id_.count(cell_name) == 0){
                            int cell_id = cell_num_;

                            cellname2id_[cell_name] = cell_id;
                            id2cellname_[cell_id] = cell_name;
                            cell_list_.push_back(Cell(cell_id));
                            cell_list_[cell_id].addNet(net_id);
                            cell_list_[cell_id].incPinNum();

                            max_pin_ = max(max_pin_,cell_list_[cell_id].getPinNum());

                            net_list_[net_id].addCell(cell_id);
                            ++cell_num_;
                            ++part_size_[0];
                        }else{
                            int cell_id = cellname2id_[cell_name];
                            cell_list_[cell_id].addNet(net_id);
                            cell_list_[cell_id].incPinNum();
                            max_pin_ = max(max_pin_,cell_list_[cell_id].getPinNum());
                            net_list_[net_id].addCell(cell_id);
                        }
                        
                        net_list_[net_id].incPartSize(0);
                    }else{
                        break;
                    }
                }
                ++net_num_;
            }
        }
    }else{
        cerr << "<Error> Unable to read " << in_file_ << endl;
    }


    in.close();
    
}

void Fm::writeResult(){



    ofstream out(out_file_);

    if(out.is_open()){
        this->calCutSize();

        out << "Cutsize = " << cut_size_ << endl;
        out << "G1 " << part_size_[0] << endl;
        for(Cell& cell : cell_list_){
            if(cell.getPart() == 0){
                out << id2cellname_[cell.getId()] << " ";
            }       
        }
        out << ";" << endl;
        out << "G2 " << part_size_[1] << endl;
        for(Cell& cell : cell_list_){
            if(cell.getPart() == 1){
                out << id2cellname_[cell.getId()] << " ";
            }       
        }
        out << ";";
    } else {
        cerr << "<Error> unable to generate final result" << endl;
    }


    out.close();
}

void Fm::initialize(){

    // generate an initial solution

    for(int id = 0 ; id < cell_num_ / 2 ;id++){
        
        Cell& cell = cell_list_[id];
        vector<int> cell_net_list = cell.getNet();

        //int part = 1 ? 0 : cell.getPart();
        cell_list_[id].move();


        for(int net_id : cell_net_list){
            net_list_[net_id].incPartSize(cell.getPart());
            net_list_[net_id].decPartSize(!cell.getPart());
        }

        part_size_[cell_list_[id].getPart()]++;
        part_size_[!cell_list_[id].getPart()]--;


    }

    // test correctness
    
    // ifstream in("test.dat");

    // string str = "";

    // vector<string> pick;

    // while(in >> str){
    //     if(str != ";") {
    //         cout << str;
    //         pick.push_back(str);
    //     }
    // }

    // for(string s:pick) {
    //     int id = cellname2id_[s];
    //     Cell& cell = cell_list_[id];
    //     vector<int> cell_net_list = cell.getNet();

    //     //int part = 1 ? 0 : cell.getPart();
    //     cell_list_[id].move();


    //     for(int net_id : cell_net_list){
    //         net_list_[net_id].incPartSize(cell.getPart());
    //         net_list_[net_id].decPartSize(!cell.getPart());
    //     }

    //     part_size_[cell_list_[id].getPart()]++;
    //     part_size_[!cell_list_[id].getPart()]--;       
    // }


    // in.close();




}

void Fm::calCutSize(){
    int cut_size = 0;
    for(auto& net : net_list_){
        if(net.getPartSize(0) && net.getPartSize(1))
            ++cut_size;
    }

    //update cut_size
    cut_size_ = cut_size;
}

bool Fm::isbalancedMove(int from_part){

    // double lower_bound = ((1-bFactor_)/2)*cell_num_;
    // double upper_bound = ((1+bFactor_)/2)*cell_num_;   
    
    int to_part = 1 - from_part;

    return part_size_[from_part] -1 >= lower_bound_ && part_size_[to_part]+1 <= upper_bound_;
}

void Fm::computeGain(){
    for(Cell& cell : cell_list_){
        const vector<int> net_id = cell.getNet();
        
        cell.setGain(0);

        for(int id : net_id){
            Net& net = net_list_[id];

            if(net.getPartSize(cell.getPart()) == 1) cell.incGain();
            if(net.getPartSize(!cell.getPart()) == 0) cell.decGain();
        }

        //cout << cell.getGain() << endl;
        cellid2node_[cell.getId()]  = BucketList_[cell.getPart()]->insert(cell);

    }
}

void Fm::updateGain(int base_id) {
    
    Cell& base_cell = cell_list_[base_id];

    const vector<int> & net_id = base_cell.getNet();

    int from  = base_cell.getPart();
    int to = 1 - from;

    base_cell.lock();
    base_cell.move();



    for (int id : net_id) {

        Net& net =  net_list_[id];

        vector<int>& cell_id = net.getCell();

        //if T(n) = 0 then increment gains of all free cells on n
        //else if T(n) = 1 then decrement gain of the only T cell on n,
        
        if(net.getPartSize(to) == 0){
            for(int id : cell_id) {
                Cell& cell = cell_list_[id];
                if(cell.isFree()){
                    cell.incGain();

                    //update blist
                    BucketList_[cell.getPart()]->Delete(cellid2node_[id]);
                    cellid2node_[id] = BucketList_[cell.getPart()]->insert(cell);
                }
            }
        }
        else if(net.getPartSize(to) == 1){
            for(int id : cell_id) {
                Cell& cell = cell_list_[id];
                if(cell.isFree() && cell.getPart() == to){
                    cell.decGain();

                    //update blist
                    BucketList_[cell.getPart()]->Delete(cellid2node_[id]);
                    cellid2node_[id] = BucketList_[cell.getPart()]->insert(cell);
                }
            }
        }


        net.decPartSize(from);
        net.incPartSize(to);


        if(net.getPartSize(from) == 0 ){
            for(int id : cell_id) {
                Cell& cell = cell_list_[id];
                if(cell.isFree()){
                    cell.decGain();

                    //update blist
                    BucketList_[cell.getPart()]->Delete(cellid2node_[id]);
                    cellid2node_[id] = BucketList_[cell.getPart()]->insert(cell);
                }
            }
        }else if(net.getPartSize(from) == 1){
            for(int id : cell_id) {
                Cell& cell = cell_list_[id];
                if(cell.isFree() && cell.getPart() == from){
                    cell.incGain();

                    //update blist
                    BucketList_[cell.getPart()]->Delete(cellid2node_[id]);
                    cellid2node_[id] = BucketList_[cell.getPart()]->insert(cell);
                }
            }
        }




    }

    //cout << "delete base_cell " << endl;
    
    //cout << "base_cell_id " << base_id << endl;

    BucketList_[from]->Delete(cellid2node_[base_id]);
    
    //cout << "finish update" << endl;
}

void Fm::Solve(){

    this->initialize();

    int total_gain = 0;

    int max_partial_gain = 0;

    int max_partial_index = -1;

    int num_move = 0;

    vector<int> move_stack;
    


    do{
        // compute all gain
        move_stack.clear();

        this->computeGain();


        total_gain = 0;

        max_partial_gain = 0;

        max_partial_index = -1;

        num_move = 0;

        // until all the vertices are locked.
        while(num_move != cell_num_) {
            
            // choose best gain
            
            Node* max_gain_inA = BucketList_[0] -> getMaxGain();
            Node* max_gain_inB = BucketList_[1] -> getMaxGain();

            Node* move_cell = NULL;

            if( max_gain_inB == NULL) {
                move_cell = max_gain_inA;
            } else if (max_gain_inA == NULL) {
                move_cell = max_gain_inB;
            } else if(isbalancedMove(max_gain_inA->getPart()) && isbalancedMove(max_gain_inB->getPart())) {
                if(max_gain_inA->getGain()>max_gain_inB->getGain()){
                    move_cell = max_gain_inA;
                } else {
                    move_cell = max_gain_inB;
                }
            } else if(isbalancedMove(max_gain_inA->getPart())){
                move_cell = max_gain_inA;
            } else{
                move_cell = max_gain_inB;
            }

            
            //calCutSize();

            int from = move_cell->getPart();
            int to = 1 - from;

            --part_size_[from];
            ++part_size_[to];

            total_gain+=move_cell->getGain();
            move_stack.push_back(move_cell->getId());

            //cout << "start update" << endl;
            updateGain(move_cell->getId());

            //calCutSize();



            if(total_gain > max_partial_gain){
                max_partial_gain = total_gain;
                max_partial_index = num_move;
            }


            ++num_move;

        }

        for(int i = 0 ;i <= max_partial_index; i++ ){
            int cell_id = move_stack[i];
            Cell& cell = cell_list_[cell_id];
            

            int from = cell.getPart();
            int to = 1 - from;

            --part_size_[from];
            ++part_size_[to];

            cell.move();

            // forget to update net 

            const vector<int>& net_ids  = cell.getNet();


            for(int net_id  : net_ids){
                Net& net = net_list_[net_id];

                net.incPartSize(to);
                net.decPartSize(from);
            }
            
        }


        for(Cell& cell : cell_list_)
            cell.unlock();


        
        cout << "max_partial_gain: " << max_partial_gain << endl;

        //calCutSize();


    }while(max_partial_gain > 0);

    writeResult();

}