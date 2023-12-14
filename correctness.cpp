bool BSTinvariant(Node* root, int left, int right){
    if (root == NULL)  return true;
    if(root->val < right && root->val > left){
        return BSTinvariant(root->left, left, root->val) && BSTinvariant(root->right, root->val, right);
    }
    else{
      return false;
    } 
}

bool checkBST(Node* root) {
    int min = -100000000;
    int max = 10000000;
    return BSTinvariant(root, min, max);
}
