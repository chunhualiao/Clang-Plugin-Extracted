#include "plugin.h"
#include <string>
#include <iostream>
#include <stdlib.h>

using namespace std;

template class Registry<PluginAction>;

PluginAction* obtainAction(const char* lib_file_name, string action_name)
{
  //load the shared library 
  void * handle = dlopen(lib_file_name, RTLD_LAZY|RTLD_GLOBAL);

  if (!handle)
  {
    cout<<"Error in dlopen: error code: "<<dlerror()<<endl;
    exit(1);
    return NULL;
  }

  //4. Iterate through the registered plugins
  PluginAction* front_action=NULL;

  for (PluginRegistry::iterator it = PluginRegistry::begin();
      it != PluginRegistry::end(); ++it)
  {
    // find an action matching the action name 
    if (it->getName() == action_name)
    {
      front_action= it->instantiate();

      //TODO: parse args related to the plugin
#if 0
      if ((P->getActionType() != PluginAction::ReplaceAction &&                                                            
            P->getActionType() != PluginAction::Cmdline) ||                                                                 
          !P->ParseArgs(CI, CI.getFrontendOpts().PluginArgs[it->getName()]))                                                  
        return nullptr;             
#endif
    }
  }

  return front_action; 
}

