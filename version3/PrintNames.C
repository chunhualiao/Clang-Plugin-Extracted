#include "plugin.h"

#include <stack>
#include <iostream>

using namespace std;

class PrintNamesAction : public PluginAction {
  private: 
    void postorder (Node* n)
    {
      if (!n) return; 
      postorder (n->left); 
      postorder (n->right); 
      cout<<n->val<<endl;
    }
  public:
    PrintNamesAction() {}
    ~PrintNamesAction() {} 

    // preorder traverse the tree and print values
    void process (Node* n) {
#if 0     
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
#endif 
      postorder (n);
    } // end process()
};

static PluginRegistry::Add<PrintNamesAction>  X("print-names", "print node names in pre-order");
