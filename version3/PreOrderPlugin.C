#include "plugin.h"

#include <stack>
#include <iostream>

using namespace std;

class PreOrderPrint: public PluginAction{
public:
  PreOrderPrint() {}
  ~PreOrderPrint() {} 

  //Preorder traverse the tree and print values
   void process (Node* n) {
     stack<Node*> stk; 
     if (n)
       stk.push(n);
     while (!stk.empty())  // only process left??
     {
       Node* cur = stk.top();  
       stk.pop();
       // print value only
       cout<<cur->val<<endl;
      if (cur->right) 
         stk.push(cur->right);
       if (cur->left) 
         stk.push(cur->left);
     }
   } // end process()
};

static PluginRegistry::Add<PreOrderPrint>  X("preorder", "print node names in pre-order");
