#include <stdbool.h>

#include <stdio.h>



#include "list.h"





struct Node_s nodeList[LIST_MAX_NUM_NODES];



struct List_s headList[LIST_MAX_NUM_HEADS];



bool firstList = true;



List * freeHead = NULL;

Node * freeNode = NULL;

int usedNodes = 0;

int usedHeads = 0;



//removes node from list of free nodes

static void removeNodeFreeList(Node * removed) {

    freeNode = removed->nextFreeNode;

    removed->nextFreeNode = NULL;

}

//adds node to list of free nodes

static void addNodeFreeList(Node * added) {

    added->nextFreeNode = freeNode;

    freeNode = added;

}

//adds head to list of free heads

static void addHeadFreeList(List * pList) {

    pList->nextFreeHead = freeHead;

    freeHead = pList;

}

//removes head from list of free heads

static void removeHeadFreeList(List* pList) {

    freeHead = pList->nextFreeHead;

    pList->nextFreeHead = NULL;

}



List* List_create() {

    //first time initialization

    if(firstList == true) {

        for(int i = 0; i<9; i++) {

            headList[i].head = NULL;

            headList[i].tail = NULL;

            headList[i].currNode = NULL;

            headList[i].currItem = NULL;

	    //initialize list of free heads

            headList[i].nextFreeHead = &headList[i+1];

            headList[i].currPos = LIST_OOB_START;

            headList[i].numNodes = 0;

        }

        headList[9].head = NULL;

        headList[9].tail = NULL;

        headList[9].currNode = NULL;

        headList[9].currItem = NULL;

        headList[9].nextFreeHead = NULL;

        headList[9].currPos = LIST_OOB_START;

        headList[9].numNodes = 0;

        

        freeHead = &headList[0];

        

        for(int i = 0; i<99; i++) {

            nodeList[i].item = NULL;

	    //initialize list of free nodes

            nodeList[i].nextFreeNode = &nodeList[i+1];

            nodeList[i].next = NULL;

            nodeList[i].prev = NULL;

        }

        nodeList[99].item = NULL;

        nodeList[99].nextFreeNode = NULL;

        nodeList[99].next = NULL;

        nodeList[99].prev = NULL;

        

        freeNode = &nodeList[0];

        firstList = false;

    }

    //if no free heads in array or no free nodes in array should return NULL

    if(usedHeads == LIST_MAX_NUM_HEADS) {

        return NULL;

    }

    //heads are cleared in removal so no need to update values

    List* toReturn = freeHead;

    removeHeadFreeList(toReturn);

    usedHeads++;

    return toReturn;

}



//returns current item

void* List_curr(List* pList) {

    if(pList->numNodes == 0) {

        return NULL;

    }

    if(pList->currItem == NULL || pList->currPos == LIST_OOB_START || pList->currPos == LIST_OOB_END) {

        return NULL;

    }

    return pList->currItem;

}



int List_append(List* pList, void* pItem) {

    if(usedNodes == LIST_MAX_NUM_NODES) {

        return -1;

    }

    pList->numNodes++;

    usedNodes++;

    Node * myNode = freeNode;

    removeNodeFreeList(myNode);

    //if there is only one node after addition

    if(pList->numNodes == 1) {

        myNode->next = NULL;

        myNode->prev = NULL;

        myNode->item = pItem;

        pList->currPos = -1;

        pList->currItem = pItem;

        pList->currNode = myNode;

        pList->head = myNode;

        pList->tail = myNode;

        return 0;

    }

    //default

    else {

        myNode->next = NULL;

        myNode->prev = pList->tail;

        pList->tail->next = myNode;

        myNode->item = pItem;

        pList->tail = myNode;

        pList->currItem = pItem;

        pList->currPos = -1;

        pList->currNode = myNode;

        return 0;

    }

}



int List_prepend(List* pList, void* pItem) {

    //failure if all nodes exhausted

    if(usedNodes == LIST_MAX_NUM_NODES) {

        return -1;

    }

    pList->numNodes++;

    usedNodes++;

    Node * myNode = freeNode;

    removeNodeFreeList(myNode);

    //if there is only one node after addition

    if(pList->numNodes == 1) {

        myNode->next = NULL;

        myNode->prev = NULL;

        myNode->item = pItem;

        pList->currPos = -1;

        pList->currItem = pItem;

        pList->currNode = myNode;

        pList->head = myNode;

        pList->tail = myNode;

        return 0;

    }

    //default

    else {

        myNode->next = pList->head;

        myNode->prev = NULL;

        pList->head->prev = myNode;

        myNode->item = pItem;

        pList->head = myNode;

        pList->currItem = pItem;

        pList->currPos = -1;

        pList->currNode = myNode;

        return 0;

    }

}







int List_insert_after(List* pList, void* pItem) {

    if(usedNodes == LIST_MAX_NUM_NODES) {

        return -1;

    }

    usedNodes++;

    pList->numNodes++;

    pList->currItem = pItem;

    Node * myNode = freeNode;

    removeNodeFreeList(myNode);

    myNode->item = pItem;

    //case if only one node after addition

    if(pList->numNodes == 1) {

        myNode->next = NULL;

        myNode->prev = NULL;

        pList->currPos = -1;

        pList->head = myNode;

        pList->tail = myNode;

        pList->currNode = myNode;

        return 0;

    }

    //current pointer is before the start

    if(pList->currPos == LIST_OOB_START) {

        pList->currPos = -1;

        //since node is added to the start, the next node will be the head

        myNode->next = pList->head;

        pList->head->prev = myNode;

        

        myNode->prev = NULL;

        //update head to newly inserted node

        pList->head = myNode;

        pList->currNode = myNode;

        return 0;

    }

    //current pointer is after end

    if(pList->currPos == LIST_OOB_END) {

        pList->currPos = -1;

        myNode->next = NULL;

        myNode->prev = pList->tail;

        pList->tail->next = myNode;

        pList->tail = myNode;

        pList->currNode = myNode;

        return 0;

    }

    //if trying to add a new tail

    if(pList->currNode == pList->tail) {

        myNode->next = NULL;

        pList->currPos = -1;

        myNode->prev = pList->currNode;

        pList->currNode->next = myNode;

        pList->tail = myNode;

        pList->currNode = myNode;

        return 0;

    }

    //default case

    myNode->next = pList->currNode->next;

    myNode->prev = pList->currNode;

    myNode->next->prev = myNode;

    myNode->prev->next = myNode;

    pList->currNode = myNode;

    return 0;



}





int List_insert_before(List* pList, void* pItem) {

    if(usedNodes == LIST_MAX_NUM_NODES) {

        return -1;

    }

    usedNodes++;

    pList->numNodes++;

    pList->currItem = pItem;

    Node * myNode = freeNode;

    removeNodeFreeList(myNode);

    myNode->item = pItem;

    //if only one node after addition

    if(pList->numNodes == 1) {

        myNode->next = NULL;

        myNode->prev = NULL;

        pList->currPos = -1;

        pList->head = myNode;

        pList->tail = myNode;

        pList->currNode = myNode;

        return 0;

    }

    //if before the start

    if(pList->currPos == LIST_OOB_START) {

        pList->currPos = -1;

        //since node is added to the start, the next node will be the head

        myNode->next = pList->head;

        pList->head->prev = myNode;

        

        myNode->prev = NULL;

        //update head to newly inserted node

        pList->head = myNode;

        pList->currNode = myNode;

        //only change tail if new node is the only node in the list

        return 0;

    }

    //if after end

    if(pList->currPos == LIST_OOB_END) {

        pList->currPos = -1;

        myNode->next = NULL;

        myNode->prev = pList->tail;

        pList->tail->next = myNode;

        pList->tail = myNode;

        //only change head if new node is the only node in list

        pList->currNode = myNode;

        return 0;

    }

    //if adding a new head

    if(pList->currNode == pList->head) {

        myNode->next = pList->head;

        myNode->prev = NULL;

        pList->head->prev = myNode;

        pList->head = myNode;

        pList->currNode = myNode;

        return 0;

    }

    //default case

    myNode->next = pList->currNode;

    myNode->prev = pList->currNode->prev;

    myNode->prev->next = myNode;

    myNode->next->prev = myNode;

    pList->currNode = myNode;

    return 0;

    

}

int List_count(List* pList) {

    List myList = *(pList);

    return myList.numNodes;

}



void* List_first(List* pList) {

    if(pList->numNodes == 0) {

        pList->currNode = NULL;

        return NULL;

    }

    else {

        pList->currPos = -1;

        pList->currNode = pList->head;

        pList->currItem = pList->head->item;

        return pList->currItem;

    }

}



void* List_trim(List* pList) {

    if(pList->numNodes == 0) {

        return NULL;    

    }

    else {

        pList->numNodes--;

        usedNodes--;

        Node * lastNode = pList->tail;

        //add removed node to 

        addNodeFreeList(lastNode);

        //void * lastReturn = pList->currItem;

        //if list is empty after deletion

        if(pList->numNodes == 0) {

            pList->currPos = LIST_OOB_START;

            pList->currItem = NULL;

            pList->currNode = NULL;

            pList->head = NULL;

            pList->tail = NULL;

        }

        //default

        else {

            pList->tail = lastNode->prev;

            lastNode->prev->next = NULL;

            pList->tail->next = NULL;

            pList->currNode = pList->tail;

            pList->currItem = pList->tail->item;

            pList->currPos = -1;

        }

        return lastNode->item;

        

    }

}

void* List_remove(List* pList) {

    if(pList->numNodes == 0 || pList->currPos == LIST_OOB_END || pList->currPos == LIST_OOB_START || pList->currNode == NULL) {

        return NULL;

    }

    Node * removed = pList->currNode;

    usedNodes--;

    pList->numNodes--;

    addNodeFreeList(removed);

    //case if no nodes after removal

    if(pList->numNodes == 0) {

        pList->currPos = LIST_OOB_START;

        pList->currItem = NULL;

        pList->currNode = NULL;

        pList->head = NULL;            

        pList->tail = NULL;

        return removed->item;

    }

    //case if head is being removed

    if(pList->currNode == pList->head) {

        pList->currPos = -1;

        pList->currItem = pList->currNode->next->item;

        pList->currNode = pList->currNode->next;

        pList->head = pList->currNode;

        removed->next = NULL;

        removed->prev = NULL;

        pList->head->prev = NULL;

        return removed->item;

    }

    //case if tail is being removed

    //no next item

    if(pList->currNode == pList->tail) {

        pList->currPos = LIST_OOB_END;

        pList->currItem = NULL;

        pList->currNode = NULL;

        pList->tail = removed->prev;

        pList->tail->next = NULL;

        removed->prev = NULL;

        removed->next = NULL;

        return removed->item;

        

    }

    //default case

    removed->prev->next = removed->next;

    removed->next->prev = removed->prev;

    pList->currNode = removed->next;
    
    pList->currItem = pList->currNode->item;


    removed->next = NULL;

    removed->prev = NULL;

    return removed->item;

    

}

void* List_last(List* pList) {

    if(pList -> numNodes == 0) {

        pList->currNode = NULL;

        return NULL;

    }

    else {

        pList->currPos = -1;

        pList->currNode = pList->tail;

        pList->currItem = pList->tail->item;

        return pList->currNode->item;

    }

}



void* List_next(List* pList) {

    if(pList->numNodes == 0 && pList->currPos == LIST_OOB_START) {

        pList->currPos = LIST_OOB_END;

        pList->currNode = NULL;

        pList->currItem = NULL;

        return NULL;

    }

    if(pList->currNode == pList->tail || pList->currPos == LIST_OOB_END || pList->numNodes == 0) {

        pList->currPos = LIST_OOB_END;

        pList->currNode = NULL;

        pList->currItem = NULL;

        return NULL;

    }

    else if(pList->currPos == LIST_OOB_START) {

        pList->currPos = -1;

        pList->currNode = pList->head;

        pList->currItem = pList->head->item;

        return pList->currItem;

    }

    else {

        pList->currNode = pList->currNode->next;

        pList->currItem = pList->currNode->item;

        return pList->currItem;

    }

}



void* List_prev(List* pList) {

    if(pList->numNodes == 0 && pList->currPos == LIST_OOB_END) {

        pList->currPos = LIST_OOB_START;

        pList->currNode = NULL;

        pList->currItem = NULL;

        return NULL;

    }

   if(pList->currNode == pList->head || pList->currPos == LIST_OOB_START || pList->numNodes == 0) {

        pList->currPos = LIST_OOB_START;

        pList->currNode = NULL;

        pList->currItem = NULL;

        return NULL;

    }

    else if(pList->currPos == LIST_OOB_END) {

        pList->currPos = -1;

        pList->currNode = pList->tail;

        pList->currItem = pList->tail->item;

        return pList->currItem;

    }

    else {

        pList->currNode = pList->currNode->prev;

        pList->currItem = pList->currNode->item;

        return pList->currItem;

    }

}





void List_concat(List* pList1, List* pList2) {

    //case if list1 is empty

    if(pList1->numNodes == 0) {

        pList1->head = pList2->head;

        pList1->tail = pList2->tail;

        pList1->numNodes = pList2->numNodes;

    

    }

    else if(pList2->numNodes == 0) {

        //nothing to do

    }

    else {

        pList1->tail->next = pList2->head;

        pList2->head->prev = pList1->tail;

        pList1->tail = pList2->tail;

        pList1->numNodes = pList1->numNodes + pList2->numNodes;

        

    }

    //clear pList2

    usedHeads--;

    pList2->head = NULL;

    pList2->tail = NULL;

    pList2->numNodes = 0;

    pList2->currItem = NULL;

    pList2->currPos = -1;

    pList2->currNode = NULL;

    addHeadFreeList(pList2);

    return;

}



void List_free(List* pList, FREE_FN pItemFreeFn) {

    Node * looper = pList->head;

    usedHeads--;

    while(looper != NULL) {

        addNodeFreeList(looper);

        usedNodes--;

        (*pItemFreeFn)(looper->item);

        looper = looper->next;

    }

    pList->head = NULL;

    pList->tail = NULL;

    pList->numNodes = 0;

    pList->currItem = NULL;

    pList->currPos = -1;

    pList->currNode = NULL;

    addHeadFreeList(pList);

    return;

    

}



void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg) {

    if(pList->numNodes == 0 || pList->currPos == LIST_OOB_END) {

        return NULL;

    }

    if(pList->currPos == LIST_OOB_START) {

        List_first(pList);

    }

    while(List_curr(pList)!=NULL && pList->currPos != LIST_OOB_END) {

        if((*pComparator)(List_curr(pList), pComparisonArg)) {

            return List_curr(pList);

        }

        List_next(pList);

    }

    return NULL;

}