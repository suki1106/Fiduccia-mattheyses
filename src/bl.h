#ifndef BL_H
#define BL_H

#include "cell.h"
#include <vector>
#include <unordered_map>
#include <iostream>

using namespace std;

class Node {
public:

    Node(): prev_(NULL), next_(NULL) {}
    Node(Cell cell) : prev_(NULL), next_(NULL) {cell_ = cell;}
    ~Node(){ }

    Node* getPrev() {return prev_;}
    Node* getNext() {return next_;}
    void setPrev(Node* prev){prev_ = prev;}
    void setNext(Node* next){next_ = next;}
    int getGain(){return cell_.getGain();}
    bool getPart() {return cell_.getPart();}
    int getId(){return cell_.getId();}

private:
    
    Node* prev_ ; 
    Node* next_ ;
    Cell cell_ ;


};

class BucketList {
public:
    
    BucketList() : max_gain_cell_(NULL) {}

    BucketList(int max_pin) : max_gain_cell_(NULL), max_pin_(max_pin) {    
        for(int i = -max_pin ; i <=max_pin;i++)
            gain2head_[i] = NULL;
    }
    ~BucketList(){}

    Node* getMaxGain() {
        if(max_gain_cell_ == NULL){
           for(int max_g = max_pin_ ; max_g >=-max_pin_ ;max_g--){
                if(gain2head_[max_g] != NULL){
                    max_gain_cell_ = gain2head_[max_g];
                    break;
                }
            }
        }
        return max_gain_cell_;
    }

    

    Node* insert (Cell& cell) {

        //cout << "<insert> ID: " << cell.getId() << endl;
        
        int gain  = cell.getGain();

        Node* node_ptr = new Node(cell);

        if( max_gain_cell_ == NULL || max_gain_cell_->getGain() <= gain) 
            max_gain_cell_ = node_ptr;
        

        if (gain2head_[gain] == NULL) {

            gain2head_[gain] = node_ptr;

        } else {

            // insert at head

            node_ptr->setNext(gain2head_[gain]);

            gain2head_[gain]->setPrev(node_ptr);

            gain2head_[gain] = node_ptr;

        }

        return node_ptr;
    }

    void Delete (Node* node_ptr) {

        //cout << "<delete> ID: " << node_ptr->getId() << endl;


        Node* prev = node_ptr->getPrev();
        Node* next = node_ptr->getNext();


        if(prev)
            prev->setNext(node_ptr->getNext());
        
        if(next)
            next->setPrev(prev);
        

        if(node_ptr == max_gain_cell_) 
            max_gain_cell_ = node_ptr->getNext();

        if(node_ptr == gain2head_[node_ptr->getGain()])
            gain2head_[node_ptr->getGain()] = node_ptr->getNext();
        
        delete node_ptr ; 
        node_ptr = NULL;


    }

private:

    int max_pin_ ;
    Node* max_gain_cell_ ;
    unordered_map<int,Node*> gain2head_;

};

#endif