#include "plugin.h"

#include <stack>
#include <iostream>

using namespace std;
// the consumer provides concrete process() implementation
class PrintNamesConsumer: public ASTConsumer 
{
public: 
   PrintNamesConsumer() {};   
  // preorder traverse the tree and print values
   void process (Node* n) override {
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

   ~PrintNamesConsumer() {}
};

// Connect the consumer's process with action
class PrintNamesAction : public PluginASTAction{
public:
    vector<ASTConsumer*> CreateASTConsumer () {
      vector<ASTConsumer*> res;
      res.push_back(new PrintNamesConsumer());
      return res;
    } 
   ~PrintNamesAction() {} 
};

static FrontendPluginRegistry::Add<PrintNamesAction>  X("print-names", "print node names in pre-order");
