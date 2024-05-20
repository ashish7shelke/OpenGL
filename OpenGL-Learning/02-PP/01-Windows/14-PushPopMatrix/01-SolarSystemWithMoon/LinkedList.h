#pragma once
#include<windows.h>
#include<stdio.h>
#include<malloc.h>

extern FILE* gpFile;

typedef void (*FnPtr)();

struct matrixData
{
    float translateXvalue;
    float translateYvalue;
    float translateZvalue;

    float rotateXvalue;
    float rotateYvalue;
    float rotateZvalue;

    float scaleXvalue;
    float scaleYvalue;
    float scaleZvalue;

    struct matrixData *next;

};

typedef struct matrixData NODE;
typedef struct matrixData* PNODE;
typedef struct matrixData** PPNODE;

BOOL insertAtFirst(PPNODE First,FnPtr funName)
{

    PNODE newNode = (PNODE)malloc(sizeof(NODE));

    if(newNode == NULL)
    {
        fprintf(gpFile, "Node memory allocation failed....\n");
        return(FALSE);
    }

    newNode->scene = funName;
    newNode->next = NULL;

    newNode->next = *First;
    *First = newNode;

    return(TRUE);
}

BOOL enqueue(PPNODE First,FnPtr fname)
{
    PNODE currentNode = *First;

    PNODE newNode = (PNODE)malloc(sizeof(NODE));
    newNode->scene = fname;
    newNode->next = NULL;

    if(*First == NULL)
    {
        *First = newNode;
    }
    else
    {
        while(currentNode->next != NULL)
        {
            currentNode=currentNode->next;
        }
        currentNode->next = newNode;
    }
    return(TRUE);
}

PNODE dequeue(PPNODE First)
{
    PNODE DELETING_NODE  = (*First);

    if(*First != NULL)
    {
        *First = (*First)->next;
        // free(DELETING_NODE);
    }

    return(DELETING_NODE);
}

void displayList(PNODE First)
{
    while(First!=NULL)
    {
        First->scene();
        First = First->next;
    }
    printf("\n");
}

PNODE traverse(int iPosition,PNODE HEAD)
{
    PNODE currentNode = HEAD;
    for(int i = 0;i< iPosition;i++)
    {
        currentNode = currentNode->next;
    }
    return(currentNode);
}
