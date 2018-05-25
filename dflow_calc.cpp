#include "dflow_calc.h"
typedef std::vector<Node&> NodeVec;
typedef std::vector<Node&>::iterator NodeVecIterator;
#define EXIT -2;
#define ENTRY -1;

/*************************************!
 * Node class
 *************************************/
class Node{
    Node& parent1;
    Node& parent2;
    NodeVec successors;
    int depth;
    InstInfo nodeInstructionInfo;
    int commandLatency;
    int predecessorLatency;
    int commandNum;

public:
    ~Node();
    void getParents(int parent1, int parent2);
    int getDepth();
    int getLatency();
    int getNum();
    void updateSuccessor(Node& node);

};

/*!
 * node regular constructor
 */
Node::Node (InstInfo* nodeInstructionInfo, int commandNum){
    nodeInstructionInfo = nodeInstructionInfo;
    commandNum = commandNum;

    //search for the predecessors
    parent1 = program::searchForPredecessor(nodeInstructionInfo->src1Idx);
    parent2 = program::searchForPredecessor(nodeInstructionInfo->src2Idx);
    // update parents with current node details (they have a new son!)
    parent1.updateSuccessor(this);
    parent2.updateSuccessor(this);
    // TODO - need to solve the entry and exit nodes
    // the new node successor is always exit until changed
    successors = EXIT; //todo - exit node
    // update latency for later functions
    commandLatency = program::opsLatency[nodeInstructionInfo->opcode];
    predecessorLatency = max(parent1.getLatency(),parent2.getLatency());
}

/*!
 * Node entry and exit constructor
 */
Node::Node(int type){

    if (type == EXIT) {
        //create exit node todo - something is wrong here
    }
    else if (type == ENTRY){
        //create entry node

    }

}

/*!
 * Node destructor - delete vectors etc.
 */
Node::~Node(){

}

/*!
 * Add the given node to the successor node vector.
 */
void Node::updateSuccessor(Node &node) {
    successors.push(node); //todo - is push the right method?
}

/*!
 * returns the current node latency + the predecessor latency
 */
int Node::getLatency() {
    return commandLatency + predecessorLatency;
}

/*!
 * returns the predecessor latency (don't include the nodes latency)
 */
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
 * returns the parents command numbers of the current node
 */
void Node::getParents(int* parent1Num, int* parent2Num) {
    *parent1Num = parent1.getNum();
    *parent2Num = parent2.getNum();
}

/****************************************!
 * program class
 ****************************************/
class program{
    Node* registerToNode[MAX_OPS];   //change the name to something "last command that touched the register"
    NodeVec allNodesVector;
    InstInfo progTrace[];
    unsigned int numOfInsts;
    int maxDepth;

public:
    const unsigned int opsLatency[];
    Node& searchForPredecessor(int srcIdx);
    int getNodeDepth(unsigned int commandNumber);
    int getNodeParents(unsigned int commandNumber, int *parent1CommandNum, int *parent1CommandNum);
    int getMaxDepth();
    ~program();
};

/*!
 * program constructor, get all the inputs and analyse the program
 */
program::program(const unsigned int opsLatency[],  InstInfo progTrace[], unsigned int numOfInsts){

}

/*!
 * program deconstructor, free all the vectors and such
 */
program::~program() {}

/*!
 * returns the program maxDepth (longest trail from exit to entry in sec)
 */
int program::getMaxDepth() {}

/*!
 * returns the depth of a certain node
 */
int program::getNodeDepth(unsigned int commandNumber) {
    return allNodesVector[commandNumber].getDepth();
}

/*!
 * save the nodes parent numbers in the pointers provided, and return 0 for success or -1 if fail
 */
int program::getNodeParents(unsigned int commandNumber, int *parent1CommandNum, int *parent1CommandNum) {}

/*!
 * gets a register src idx and returns the latest command that write to that register
 */
Node& program::searchForPredecessor(int srcIdx){
//go to the reg table and see who is at regTable[srcIdx]
    return registerToNode[srcIdx];
}



ProgCtx analyzeProg(const unsigned int opsLatency[],  InstInfo progTrace[], unsigned int numOfInsts) {
    return PROG_CTX_NULL;
}

void freeProgCtx(ProgCtx ctx) {
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    return -1;
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {

    return -1;
}

int getProgDepth(ProgCtx ctx) {
    return 0;
}
