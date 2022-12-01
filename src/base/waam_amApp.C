#include "waam_amApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

InputParameters
waam_amApp::validParams()
{
  InputParameters params = MooseApp::validParams();

  return params;
}

waam_amApp::waam_amApp(InputParameters parameters) : MooseApp(parameters)
{
  waam_amApp::registerAll(_factory, _action_factory, _syntax);
}

waam_amApp::~waam_amApp() {}

void
waam_amApp::registerAll(Factory & f, ActionFactory & af, Syntax & syntax)
{
  ModulesApp::registerAll(f, af, syntax);
  Registry::registerObjectsTo(f, {"waam_amApp"});
  Registry::registerActionsTo(af, {"waam_amApp"});

  /* register custom execute flags, action syntax, etc. here */
}

void
waam_amApp::registerApps()
{
  registerApp(waam_amApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
waam_amApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  waam_amApp::registerAll(f, af, s);
}
extern "C" void
waam_amApp__registerApps()
{
  waam_amApp::registerApps();
}
