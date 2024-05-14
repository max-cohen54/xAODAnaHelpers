#include "xAODAnaHelpers/L1EMContainer.h"
#include <iostream>

using namespace xAH;

L1EMContainer::L1EMContainer(const std::string& name, float units, bool mc)
  : ParticleContainer(name,"",units,mc)
{
  m_l1Egamma_et                =new std::vector<float>();
  m_l1Egamma_eta               =new std::vector<float>();
  m_l1Egamma_phi               =new std::vector<float>();
}

L1EMContainer::~L1EMContainer()
{
  if(m_debug) std::cout << " Deleting L1EMContainer "  << std::endl;
  delete m_l1Egamma_et;
  delete m_l1Egamma_eta;
  delete m_l1Egamma_phi;
}

void L1EMContainer::setTree(TTree *tree)
{
  ParticleContainer::setTree(tree);
  connectBranch<float>(tree,"et",&m_l1Egamma_et);
  connectBranch<float>(tree,"eta",  &m_l1Egamma_eta);
  connectBranch<float>(tree,"phi",  &m_l1Egamma_phi);
}

void L1EMContainer::updateParticle(uint idx, Photon& egamma)
{
  ParticleContainer::updateParticle(idx,egamma);
}

void L1EMContainer::setBranches(TTree *tree)
{
  ParticleContainer::setBranches(tree);
  setBranch<float>(tree,"et",m_l1Egamma_et); // et8x8 for the case of Legacy L1 egamma RoIs
  setBranch<float>(tree,"eta",  m_l1Egamma_eta);
  setBranch<float>(tree,"phi",  m_l1Egamma_phi);
  return;
}

void L1EMContainer::clear()
{
  ParticleContainer::clear();
  m_l1Egamma_et->clear();
  m_l1Egamma_eta  ->clear();
  m_l1Egamma_phi  ->clear();
  return;
}
