//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#include "waam_amTestApp.h"
#include "waam_amApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"
#include "ModulesApp.h"

InputParameters
waam_amTestApp::validParams()
{
  InputParameters params = waam_amApp::validParams();
  return params;
}

waam_amTestApp::waam_amTestApp(InputParameters parameters) : MooseApp(parameters)
{
  waam_amTestApp::registerAll(
      _factory, _action_factory, _syntax, getParam<bool>("allow_test_objects"));
}

waam_amTestApp::~waam_amTestApp() {}

void
waam_amTestApp::registerAll(Factory & f, ActionFactory & af, Syntax & s, bool use_test_objs)
{
  waam_amApp::registerAll(f, af, s);
  if (use_test_objs)
  {
    Registry::registerObjectsTo(f, {"waam_amTestApp"});
    Registry::registerActionsTo(af, {"waam_amTestApp"});
  }
}

void
waam_amTestApp::registerApps()
{
  registerApp(waam_amApp);
  registerApp(waam_amTestApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
// External entry point for dynamic application loading
extern "C" void
waam_amTestApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  waam_amTestApp::registerAll(f, af, s);
}
extern "C" void
waam_amTestApp__registerApps()
{
  waam_amTestApp::registerApps();
}
