#include "xAODAnaHelpers/L1MuonContainer.h"
#include <iostream>

using namespace xAH;

L1MuonContainer::L1MuonContainer(const std::string& name, float units, bool mc)
  : ParticleContainer(name,"",units,mc)
{
  m_l1Muon_et                =new std::vector<float>();
  m_l1Muon_eta               =new std::vector<float>();
  m_l1Muon_phi               =new std::vector<float>();
}

L1MuonContainer::~L1MuonContainer()
{
  if(m_debug) std::cout << " Deleting L1MuonContainer "  << std::endl;
  delete m_l1Muon_et;
  delete m_l1Muon_eta;
  delete m_l1Muon_phi;
}

void L1MuonContainer::setTree(TTree *tree)
{
  ParticleContainer::setTree(tree);
  connectBranch<float>(tree,"et",&m_l1Muon_et);
  connectBranch<float>(tree,"eta",  &m_l1Muon_eta);
  connectBranch<float>(tree,"phi",  &m_l1Muon_phi);
}

void L1MuonContainer::updateParticle(uint idx, Muon& muon)
{
  ParticleContainer::updateParticle(idx,muon);
}

void L1MuonContainer::setBranches(TTree *tree)
{
  ParticleContainer::setBranches(tree);
  setBranch<float>(tree,"et",m_l1Muon_et); 
  setBranch<float>(tree,"eta",  m_l1Muon_eta);
  setBranch<float>(tree,"phi",  m_l1Muon_phi);
  return;
}

void L1MuonContainer::clear()
{
  ParticleContainer::clear();
  m_l1Muon_et->clear();
  m_l1Muon_eta  ->clear();
  m_l1Muon_phi  ->clear();
  return;
}

