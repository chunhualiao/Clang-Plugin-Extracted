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

  //1. Obtain shared library path/name and action name  
  const char* lib_file="PreOrderPlugin.so"; 
  string action_name("preorder");
  
  PluginAction* act = obtainAction(lib_file, action_name);

  //2. Invoke the process() function 
  act->process(root);

  return 0; 
}
