#include "plugin.h"

#include <string>
#include <iostream>

using namespace std; 

int main()
{
  //0. Initialize the tree
  Node* root = new Node(1);
  Node* n2= new Node(2);
  Node* n3= new Node(3);
  Node* n4= new Node(4);
  root->left = n2; 
  root->right= n3; 
  n3->right=n4; 


  //2. Obtain shared library path/name and action name  
  //string lib_file("PrintNames.so"); 
  const char* lib_file="PrintNames.so"; 
  string action_name("print-names");
  
  // move most logic into the plugin.so so symbols are resolved as much as possible there.
  vector<ASTConsumer *> vec = obtainConsumers (lib_file, action_name);

  //5. Invoke the consumer's process() function 
  vec[0]->process(root);

  return 0; 
}
