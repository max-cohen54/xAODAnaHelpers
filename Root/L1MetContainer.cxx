#include "xAODAnaHelpers/L1MetContainer.h"
#include <iostream>

#include "TMath.h"

using namespace xAH;

L1MetContainer::L1MetContainer(const std::string& name, const std::string& detailStr, float units)
  : m_name(name), m_infoSwitch(detailStr), m_debug(false), m_units(units)
{
}

L1MetContainer::~L1MetContainer()
{
  if(m_debug) std::cout << "Deleting TrigMetContainer " << std::endl;
}

void L1MetContainer::setTree(TTree *tree)
{

  connectBranch<float>(tree, "Met",    &m_l1Met   );
  connectBranch<float>(tree, "MetPx",  &m_l1Met   );
  connectBranch<float>(tree, "MetPy",  &m_l1Met   );
  connectBranch<float>(tree, "MetPhi", &m_l1Met   );

}

void L1MetContainer::setBranches(TTree *tree)
{

  setBranch(tree, "Met",     &m_l1Met,     "F");
  setBranch(tree, "MetPx",   &m_l1Met_Px,  "F");
  setBranch(tree, "MetPy",   &m_l1Met_Py,  "F");
  setBranch(tree, "MetPhi",  &m_l1Met_phi, "F");

  return;

}


void L1MetContainer::clear()
{
  m_l1Met      = -999;
  m_l1Met_Px   = -999;
  m_l1Met_Py   = -999;
  m_l1Met_phi = -999;

  return;
}
