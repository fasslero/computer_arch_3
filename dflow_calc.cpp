#include <vector>
#include <stdlib.h>
#include <math.h>
#include "dflow_calc.h"
//typedef std::vector<Node*> NodeVec;
//typedef std::vector<Node*>::iterator NodeVecIterator;
#define EXIT -2
#define ENTRY -1
#define MAX_OPS 32

ProgCtx ctx;
/*************************************!
* Node class
*************************************/
class Node {

    //NodeVec successors;
    int depth;
    InstInfo nodeInstructionInfo;
    int commandLatency;
    int commandNum;

public:
    Node(InstInfo* NodeInstructionInfo, int CommandNum);
    Node(int);
    ~Node();
    void getParents(int* parent1, int* parent2);
    InstInfo* getInst();
    int getDepth();
    int getLatency();
    int getNum();
    //void updateSuccessor(Node& node);
    void updateParent(int, Node*);
    void updateLatency(int);
    void updateDepth(Node*);
    Node* parent1;
    Node* parent2;

};

/*!
* node regular constructor
*/
Node::Node(InstInfo* NodeInstructionInfo, int CommandNum) {
    nodeInstructionInfo = *NodeInstructionInfo;
    commandNum = CommandNum;

    //search for the predecessors
    parent1 = NULL;
    parent2 = NULL;
    commandLatency = 0;
    depth = 0;
}

/*!
* Node entry and exit constructor
*/
Node::Node(int type) : parent1(NULL), parent2(NULL), commandNum(0) {

    if (type == EXIT) {
        // create exit node
        //I think we dont need an exit node
        commandNum = EXIT;
    }
    else if (type == ENTRY) {
        //create entry node with command num 0
        commandNum = -1;
        commandLatency = 0;
        depth = 0;
    }
}

/*!
* Node destructor - delete vectors etc.
*/
Node::~Node() {
}


/*!
 * updates the parent of the node
 */
void Node::updateParent(int num, Node* pNode) {
    if (num == 1)
        parent1 = pNode;
    else parent2 = pNode;

}

/*!
* returns the current node latency + the predecessor latency
*/
int Node::getLatency() {
    return commandLatency;
}

/*!
 * updates command latency
 */
void Node::updateLatency(int latency) {
    commandLatency = latency;
}

/*!
* returns the predecessor latency (don't include the nodes latency)
*/

void Node::updateDepth(Node* parent) {
    if (parent == NULL)
        return;
    int temp = parent->getDepth() + parent->getLatency();
    if (temp > depth)
        depth = temp;
}

int Node::getDepth() {
    return depth;
}

/*!
* returns the command number
*/
int Node::getNum() {
    return commandNum;
}

/*!
 * return the Instruction info of the node
 */
InstInfo* Node::getInst() {
    return &nodeInstructionInfo;
}

/*!
* returns the parents command numbers of the current node
*/
void Node::getParents(int* parent1Num, int* parent2Num) {
    *parent1Num = parent1->getNum();
    *parent2Num = parent2->getNum();
}

/****************************************!
* program class
****************************************/
class program {
    Node* LastCommand[MAX_OPS];  // last command that modified the register
    unsigned int numOfInsts;
    InstInfo* progTrace;
    int maxDepth;

public:
    std::vector<Node*> allNodesVector;
    program(const unsigned int OpsLatency[], InstInfo progTrace[], unsigned int numOfInsts);
    unsigned int opsLatency[MAX_OPS];
    //Node* searchForPredecessor(int srcIdx);
    int getNodeDepth(unsigned int commandNumber);
    int getNodeParents(unsigned int commandNumber, int *, int *);
    int getMaxDepth();
    ~program();
};

/*!
* program constructor, get all the inputs and analyse the program
*/
program::program(const unsigned int OpsLatency[], InstInfo progTrace[], unsigned int numOfInsts) {

    Node* EntryNode = new Node(ENTRY);
    //InstInfo* progTrace = new InstInfo[numOfInsts];
    maxDepth = 0;

    for (int i = 0; i < MAX_OPS; i++) {
        LastCommand[i] = EntryNode;
        opsLatency[i] = OpsLatency[i];
    }
    for (int i = 0; i < numOfInsts; i++) {
        InstInfo* currInst = progTrace + i;
        Node* currNode = new Node(currInst, i);
        currNode->updateParent(1, LastCommand[currInst->src1Idx]);  //updating the parents
        currNode->updateParent(2, LastCommand[currInst->src2Idx]);
        currNode->updateLatency(opsLatency[currNode->getInst()->opcode]);	//update latency
        currNode->updateDepth(currNode->parent1);
        currNode->updateDepth(currNode->parent2);
        LastCommand[currNode->getInst()->dstIdx] = currNode;
        int currDepth = currNode->getDepth() + currNode->getLatency();
        if (maxDepth < currDepth) {
            maxDepth = currDepth;
        }
        allNodesVector.push_back(currNode);
    }
}


/*!
* program deconstructor, free all the vectors and such
*/
program::~program() {}


/*!
* returns the program maxDepth (longest trail from exit to entry in sec)
*/
int program::getMaxDepth() {
    return maxDepth;
}


/*!
* returns the depth of a certain node
*/
int program::getNodeDepth(unsigned int commandNumber) {

    return allNodesVector[commandNumber]->getDepth();
}


/*!
* save the nodes parent numbers in the pointers provided, and return 0 for success or -1 if fail
*/
int program::getNodeParents(unsigned int commandNumber, int *parent1CommandNum, int *parent2CommandNum) {
    allNodesVector[commandNumber]->getParents(parent1CommandNum, parent2CommandNum);
    return 0;
}


/*!
* gets a register src idx and returns the latest command that write to that register
*/
Node* searchForPredecessor(int srcIdx, Node* LastCommand[]) {
    //go to the reg table and see who is at lastcommand[srcIdx]
    return LastCommand[srcIdx];
}


/***************************************!
* Interface functions
***************************************/
ProgCtx analyzeProg(const unsigned int opsLatency[], InstInfo progTrace[], unsigned int numOfInsts) {
    ctx = new program(opsLatency, progTrace, numOfInsts);
    return ctx;

    // todo - need this? return PROG_CTX_NULL;
}

void freeProgCtx(ProgCtx ctx) {
    delete ctx;
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    program* prog = (program*)ctx;
    return prog->getNodeDepth(theInst);
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    program* prog = (program*)ctx;
    *src1DepInst = prog->allNodesVector[theInst]->parent1->getNum();
    *src2DepInst = prog->allNodesVector[theInst]->parent2->getNum();
    return 0;
    // return -1 if fail, how can it fail?
}

int getProgDepth(ProgCtx ctx) {
    program* prog = (program*)ctx;
    return prog->getMaxDepth();
}
