// Project identifier: 43DE0E0C4C76BFAA6D8C2F5AEAE0518A9C42CF4E

#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <utility>

// A specialized version of the priority queue ADT implemented as a pairing
// heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
        public:
            // TODO: After you add add one extra pointer (see below), be sure
            //       to initialize it here.
            explicit Node(const TYPE &val)
                : elt{ val }, child{ nullptr }, sibling{ nullptr }, parent{ nullptr }
            {}

            // Description: Allows access to the element at that Node's
            //              position. There are two versions, getElt() and a
            //              dereference operator, use whichever one seems
            //              more natural to you.
            // Runtime: O(1) - this has been provided for you.
            const TYPE &getElt() const { return elt; }
            const TYPE &operator*() const { return elt; }

            // The following line allows you to access any private data
            // members of this Node class from within the PairingPQ class.
            // (ie: myNode.elt is a legal statement in PairingPQ's add_node()
            // function).
            friend PairingPQ;

        private:
            TYPE elt;
            Node *child;
            Node *sibling;
            // TODO: Add one extra pointer (parent or previous) as desired.
            Node *parent;
    }; // Node


    // Description: Construct an empty pairing heap with an optional
    //              comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
        root = nullptr;
        countNodes = 0;
    } // PairingPQ()


    // Description: Construct a pairing heap out of an iterator range with an
    //              optional comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
        root = nullptr;
        countNodes = 0;
        InputIterator i = start;
        while(i != end){
            push(*i++);
        }
    } // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ &other) :
        BaseClass{ other.compare } {
        root = nullptr;
        countNodes = 0;
        std::deque<Node *> a;
        a.push_back(other.root);
        while(!a.empty()){
            Node* temp = a.front();
            a.pop_front();
            if(temp->child){
                a.push_back(temp->child);
            }
            if(temp->sibling){
                a.push_back(temp->sibling);
            }
            push(temp->elt);
        }
    } // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    PairingPQ &operator=(const PairingPQ &rhs) {
        // TODO: Implement this function.
        // HINT: Use the copy-swap method from the "Arrays and Containers"
        //       lecture.
        PairingPQ temp(rhs);
        std::swap(root, temp.root);
        std::swap(countNodes, temp.countNodes);

        return *this;
    } // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
        // TODO: Implement this function.
        std::deque<Node*> a;
        a.push_back(root);
        while(a.empty()){
            Node* temp = a.front();
            a.pop_front();
            if(temp->sibling){
                a.push_back(temp->sibling);
            }
            if(temp->child){
                a.push_back(temp->child);
            }
            delete temp;
        }
    } // ~PairingPQ()


    // Description: Assumes that all elements inside the pairing heap are out
    //              of order and 'rebuilds' the pairing heap by fixing the
    //              pairing heap invariant. You CANNOT delete 'old' nodes
    //              and create new ones!
    // Runtime: O(n)
    virtual void updatePriorities() {
        // TODO: Implement this function.
        std::deque<Node*> a;
        a.push_back(root);
        root = nullptr;
        while(!a.empty()){
            Node* temp = a.front();
            a.pop_front();

            if(temp->child){
                a.push_back(temp->child);
                temp->child = nullptr;
            }
            if(temp->sibling){
                a.push_back(temp->sibling);
                temp->sibling = nullptr;
            }

            temp->parent = nullptr;

            if(root){
                root = meld(root, temp);
            }
            else{
                root = temp;
            }
        }
    } // updatePriorities()


    // Description: Add a new element to the pairing heap. This is already
    //              done. You should implement push functionality entirely in
    //              the addNode() function, and this function calls
    //              addNode().
    // Runtime: O(1)
    virtual void push(const TYPE &val) {
        addNode(val);
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element
    //              from the pairing heap.
    // Note: We will not run tests on your code that would require it to pop
    //       an element when the pairing heap is empty. Though you are
    //       welcome to if you are familiar with them, you do not need to use
    //       exceptions in this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
        // TODO: Implement this function.
        Node* temp = root;
        if(size() <= 2 && !empty()){
            if(root->child){
                root = root->child;
                root->parent = nullptr;
            }
            else{
                root = nullptr;
            }
            delete temp;
        }
        else{
            (root->child)->parent = nullptr;
            temp = temp->child;
            delete root;
            root = nullptr;

            std::deque<Node*> a;
            for(Node* ptr = temp; ptr != nullptr; ptr = ptr->sibling){
                a.push_back(ptr);
            }

            size_t i = 0;
            while(a.size() != 1){
                if(i+1<a.size()){
                    Node* n = a.at(i);
                    Node* m = a.at(i+1);
                    n->parent = nullptr;
                    m->parent = nullptr;
                    n->sibling = nullptr;
                    m->sibling = nullptr;
                    a.push_back(meld(n,m));
                    a.pop_front();
                    a.pop_front();
                }
            }
            root = a.front();
        }
        countNodes--;
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the pairing heap. This should be a reference for speed.
    //              It MUST be const because we cannot allow it to be
    //              modified, as that might make it no longer be the most
    //              extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        // TODO: Implement this function
        return root->elt;
    } // top()


    // Description: Get the number of elements in the pairing heap.
    // Runtime: O(1)
    virtual std::size_t size() const {
        // TODO: Implement this function
        return countNodes;
    } // size()

    // Description: Return true if the pairing heap is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        // TODO: Implement this function
        return (countNodes == 0);
    } // empty()


    // Description: Updates the priority of an element already in the pairing
    //              heap by replacing the element refered to by the Node with
    //              new_value. Must maintain pairing heap invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more
    //               extreme (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    void updateElt(Node* node, const TYPE &new_value) {
        // TODO: Implement this function
        node->elt = new_value;
        if(this->compare((node->parent)->elt, new_value)){
            if((node->parent)->child == node){
                (node->parent)->child = node->sibling;
            }
            else{
                Node* prev;
                for(prev = (node->parent)->child; prev->sibling != node;
                prev = prev->sibling){}
                prev->sibling = node->sibling;
            }
            node->sibling = nullptr;
            node->parent = nullptr;
            root = meld(root, node);
        }
    } // updateElt()


    // Description: Add a new element to the pairing heap. Returns a Node*
    //              corresponding to the newly added element.
    // NOTE: Whenever you create a node, and thus return a Node *, you must
    //       be sure to never move or copy/delete that node in the future,
    //       until it is eliminated by the user calling pop(). Remember this
    //       when you implement updateElt() and updatePriorities().
    // Runtime: O(1)
    Node* addNode(const TYPE &val) {
        Node* node = new Node(val);
        if(empty()){
            root = node;
        }
        else{
            root = meld(root, node);
        }
        countNodes++;
        return node;
    } // addNode()


private:
    // TODO: Add any additional member variables or member functions you
    //       require here.
    Node* root;
    size_t countNodes;

    // TODO: We recommend creating a 'meld' function (see the Pairing Heap
    //       papers).
    Node* meld(Node* a, Node* b){
        if(!a->parent && !a->sibling && !b->parent && !b->sibling){
            if(this->compare(a->elt, b->elt)){
                a->parent = b;
                a->sibling = b->child;
                b->child = a;
                return b;
            }
            else{
                b->parent = a;
                b->sibling = a->child;
                a->child = b;
                return a;
            }
        }
        return nullptr;
    }

    // NOTE: For member variables, you are only allowed to add a "root
    //       pointer" and a "count" of the number of nodes. Anything else
    //       (such as a deque) should be declared inside of member functions
    //       as needed.
};


#endif // PAIRINGPQ_H
