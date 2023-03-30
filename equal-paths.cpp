#ifndef RECCHECK
#define RECCHECK

// Add any necessary #includes here
#include <algorithm>

#endif

#include "equal-paths.h"


using namespace std;

// You may add any prototypes of helper functions here

bool equalPaths(Node* root)
{
    if (root == nullptr) {
        return true;
    }
    if (root->left == nullptr && root->right == nullptr) {
        return true;
    }

    int leftHeight = 1;
    int rightHeight = 1;
    bool leftEqualCheck = true;
    bool rightEqualCheck = true;

    if (root->left != nullptr) {
        leftEqualCheck = equalPaths(root->left);
        leftHeight = height_of_tree(root->left);
    }
    if (root->right != nullptr) {
        rightEqualCheck = equalPaths(root->right);
        rightHeight = height_of_tree(root->right);
    }

    if (leftEqualCheck && rightEqualCheck && leftHeight == rightHeight) {
        return true;
    }
    return false;
}



int height_of_tree(Node* root){
    if (root == nullptr){
        return 0;
    }
    int height_of_left_tree = height_of_tree(root->left);
    int height_of_right_tree = height_of_tree(root->right);
    return max(height_of_left_tree, height_of_right_tree) + 1;

}

