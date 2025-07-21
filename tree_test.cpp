#include<iostream>
# include <new.h>

using namespace std;

struct Node
{
    int type; //スタートから見て　左分岐→-1　T字路→0　右分岐→1　スタート→-10　行き止まり→5　ゴール→7
    int direction; //親から見て　左→-1　右→1
    Node *parent, *left, *right;
};

Node *root, *curNode, *goal;
int direction = -1; //親から見て　左→-1　右→1
int reverse = 0; //葉に向かって進む→0　根に向かって進む→1　ゴール発見後根に戻る→-1
int goalPath[20]; //ゴールからスタートまでの道筋　左折→-1　直進→5　右折→1
int finalPath[20]; //スタートからゴールまでの道筋　左折→-1　直進→5　右折→1
int pathIndex = 0;

Node* insert(int type, int direction, Node* preNode){
    Node *newNode;
    newNode = new Node;
    newNode->type = type;
    newNode->parent = preNode;
    newNode->left = NULL;
    newNode->right = NULL;
    if(direction == -1){
        preNode->left = newNode;
        newNode->direction = -1;
    }else{
        preNode->right = newNode;
        newNode->direction = 1;
    }
    return newNode;
}

void printTree(Node* node, int depth = 0, string prefix = "") {
    if (node == NULL) return;

    // Determine the connector for the current node
    string connector = (node->parent == NULL) ? "" : ((node->parent->left == node) ? "|-- L: " : "-- R: ");

    // Print the current node with appropriate indentation and connector
    cout << prefix << connector << "Node(type=" << node->type << ")";
    if (node->parent == NULL) {
        cout << " [root]";
    }
    cout << endl;

    // Recursively print left and right children
    // Adjust prefix for children to maintain the tree structure visually
    string childPrefix = prefix;
    if (node->parent != NULL) {
        if (node->parent->left == node) {
            childPrefix += "|   ";
        } else {
            childPrefix += "    ";
        }
    } else { // For the root, just add spaces
        childPrefix += "    ";
    }
    
    printTree(node->left, depth + 1, childPrefix);
    printTree(node->right, depth + 1, childPrefix);
}

void interchange(int type){

    if (reverse == -1)
    {
        if (curNode->parent == NULL)
        {
            printf("null");
            return;
        }
        
        if (curNode->direction == -1)
        {
            if (curNode->parent->type == 1)
            {
                printf("return forward(L)\n");
                goalPath[pathIndex] = 5;
            }else{
                printf("return right(L)\n");
                goalPath[pathIndex] = -1;
            }
            curNode = curNode->parent;
        }else{
            if (curNode->parent->type == -1)
            {
                printf("return forward(R)\n");
                goalPath[pathIndex] = 5;
            }else{
                printf("return left(R)\n");
                goalPath[pathIndex] = 1;
            }
            curNode = curNode->parent;
        }
        pathIndex++;
        return;
    }

    if(type == 5){
        reverse = 1;
        insert(5,direction,curNode);
        printf("wall\n");
        return;
    }

    if(type == 7){
        reverse = -1;
        curNode = insert(7,direction,curNode);
        goal = curNode;
        printf("goal\n");
        return;
    }

    if(reverse == 0){
        curNode = insert(type,direction,curNode);
        direction = -1;
        if(type == -1){
            printf("left\n");
        }else if(type == 0){
            printf("left\n");
        }else{
            printf("foward\n");
        }
    }else{
        if(curNode->right == NULL){
            if(curNode->type == 0){
                printf("forward\n");
            }else{
                printf("left\n");
            }
            reverse = 0;
            direction = 1;
            
        }else{
            curNode = curNode->parent;
            printf("back ");
            if(curNode->type == -1){
                printf("forward\n");
            }else{
                printf("left\n");
            }
        }
    }
    return;
}

void makePath(){
    int j=0;
    for (int i = 20; i > -1; i--)
    {
        if (goalPath[i] != 0)
        {
            finalPath[j] = goalPath[i];
            j++;
        }
    }
    
    for (int i = 0; i < j; i++)
    {
        printf("%d,",finalPath[i]);
    }
    return;
}
int main(void){
    root = new Node;
    root->type = -10;
    root->parent = NULL;
    curNode = root;
    printf("start\n");
    interchange(0);
    interchange(1);
    interchange(5);
    interchange(0);
    interchange(0);
    interchange(5);
    interchange(0);
    interchange(5);
    interchange(0);
    interchange(0);
    interchange(0);
    interchange(-1);
    interchange(0);
    interchange(5);
    interchange(-1);
    interchange(-1);
    interchange(7);
    interchange(0);
    interchange(0);
    interchange(0);
    interchange(0);
    printTree(root);

    makePath();
    
    return 0;
}
