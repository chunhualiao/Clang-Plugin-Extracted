#include "plugin.h"

#include <stack>
#include <iostream>

using namespace std;

class PrintNamesAction : public PluginAction{
public:
  PrintNamesAction() {}
  ~PrintNamesAction() {} 

  // preorder traverse the tree and print values
   void process (Node* n) override {
     stack<Node*> stk; 
     if (n)
       stk.push(n);
     while (!stk.empty()) 
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

static PluginRegistry::Add<PrintNamesAction>  X("print-names", "print node names in pre-order");
