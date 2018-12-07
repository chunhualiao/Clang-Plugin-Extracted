#include "plugin.h"
#include <string>
#include <iostream>
#include <stdlib.h>

using namespace std;

template class Registry<PluginASTAction>;

vector<ASTConsumer *> obtainConsumers (const char* lib_file_name, string action_name)
{
  vector<ASTConsumer *> res;
  //load the shared library 
  void * handle = dlopen(lib_file_name, RTLD_LAZY|RTLD_GLOBAL);

  if (!handle)
  {
    cout<<"Error in dlopen: error code: "<<dlerror()<<endl;
    exit(1);
    return res;
  }

  //4. Iterate through the registered plugins
  std::unique_ptr<PluginASTAction> front_action;

  for (FrontendPluginRegistry::iterator it = FrontendPluginRegistry::begin();
      it != FrontendPluginRegistry::end(); ++it)
  {
    if (it->getName() == action_name)
    {
      // find action matching the action name print-fns
      front_action= it->instantiate();

      //parse args related to the plugin
#if 0
      if ((P->getActionType() != PluginASTAction::ReplaceAction &&                                                            
            P->getActionType() != PluginASTAction::Cmdline) ||                                                                 
          !P->ParseArgs(CI, CI.getFrontendOpts().PluginArgs[it->getName()]))                                                  
        return nullptr;             
#endif
    }
  }

  res= front_action->CreateASTConsumer() ;

  return res; 
}

