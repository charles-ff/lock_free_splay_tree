bool BSTinvariant(Node* root, int left, int right){
    if (root == NULL) return true;
    if(left < root->key && root->key < right){
        return BSTinvariant(root->left, left, root->key) && BSTinvariant(root->right, root->key, right);
    } else {
      return false;
    } 
}

bool checkBST(Node* root) {
    int min = INT_MIN;
    int max = INT_MAX;
    return BSTinvariant(root, min, max);
}
