#ifndef __LINKED_LIST__
#define __LINKED_LIST__

#include <cstdbool>
#include <iostream>
#include <vector>

// Simple Linked List implementation
struct Node{
    const std::vector<double>* data;
    const int                  data_id;    // ID is used in order to print the nearest neighbor
    Node*                      next;

    Node(const std::vector<double>*& vec, const int& id, Node* nxt): 
                data(vec), data_id(id), next(nxt) {}

    ~Node(){}
};

class LinkedList_Iterator{
private:
    Node* nodePtr;

public:
    LinkedList_Iterator(Node* node): nodePtr(node){}

    const bool isNull() const { return nodePtr == NULL; }
    const std::vector<double>& getData() const{return *(nodePtr->data);}
    const int getID() const { return nodePtr->data_id; }

    LinkedList_Iterator& operator++(){
        nodePtr = nodePtr->next;
        return *this;
    }

    LinkedList_Iterator& operator++(int other)
    {
        LinkedList_Iterator temp = *this;
        ++*this;
        return temp;
    }
};

class LinkedList{
private:
    Node* first_node;
    size_t size;

public:
    LinkedList(): first_node(NULL), size(0) {}

    LinkedList_Iterator begin(){return LinkedList_Iterator(first_node);}
    
    // Adding to the start of the list
    void add(const std::vector<double>*& vec, const int& id){
        size++;
        first_node = new Node(vec, id, first_node);
    }

    size_t getSize() const { return size; }

    ~LinkedList(){
        Node* curr_node = first_node;

        while(curr_node != NULL){
            Node* tmp = curr_node;
            curr_node = curr_node->next;

            delete tmp;
        }
    }
};

#endif