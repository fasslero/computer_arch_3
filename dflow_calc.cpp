#include "dflow_calc.h"
#include <vector>
typedef std::vector<Node&> NodeVec;
typedef std::vector<Node&>::iterator NodeVecIterator;
#define EXIT -2
#define ENTRY -1


/*hello world*/
/*checking the  fucking git */
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
    int commandNum;

public:
    ~Node();
    void getParents(int* parent1, int* parent2);
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
    depth = max(parent1.getLatency(),parent2.getLatency());
}

/*!
 * Node entry and exit constructor
 */
Node::Node(int type){

    if (type == EXIT) {
        // create exit node
        // todo - I think we can use use the node object by reverse (successors vector will be the parent vector)
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
    // todo - delete EXIT?
    successors.push_back(node); //todo - is push_back the right method?
}

/*!
 * returns the current node latency + the predecessor latency
 */
int Node::getLatency() {
    return commandLatency + depth;
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
    Node* registerToNode[MAX_OPS];   // todo - change the name to something "last command that touched the register"
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
int program::getMaxDepth() {
    return maxDepth;
}

/*!
 * returns the depth of a certain node
 */
int program::getNodeDepth(unsigned int commandNumber) {
    return allNodesVector[commandNumber].getDepth();
}

/*!
 * save the nodes parent numbers in the pointers provided, and return 0 for success or -1 if fail
 */
int program::getNodeParents(unsigned int commandNumber, int *parent1CommandNum, int *parent2CommandNum) {
    allNodesVector[commandNumber].getParents(parent1CommandNum, parent2CommandNum);
    return 0;
}

/*!
 * gets a register src idx and returns the latest command that write to that register
 */
Node& program::searchForPredecessor(int srcIdx){
//go to the reg table and see who is at regTable[srcIdx]
    return registerToNode[srcIdx];
}


/***************************************!
 * Interface functions
 ***************************************/
ProgCtx analyzeProg(const unsigned int opsLatency[],  InstInfo progTrace[], unsigned int numOfInsts) {
    ProgCtx program = new program(opsLatency, progTrace, numOfInsts);
    return program;

    // todo - need this? return PROG_CTX_NULL;
}

void freeProgCtx(ProgCtx ctx) {
    delete ctx;
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    return ctx.getNodeDepth(theInst);
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    ctx.getNodeParents(theInst,src1DepInst,src2DepInst);
    return 0;
    // return -1 if fail, how can it fail?
}

int getProgDepth(ProgCtx ctx) {
    ctx.getMaxDepth();
    return 0;
}
