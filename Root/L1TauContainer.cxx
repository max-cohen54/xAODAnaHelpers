#include "xAODAnaHelpers/L1TauContainer.h"
#include <iostream>

using namespace xAH;

L1TauContainer::L1TauContainer(const std::string& name, float units, bool mc)
  : ParticleContainer(name,"",units,mc)
{
  m_l1Tau_et                =new std::vector<float>();
  m_l1Tau_eta               =new std::vector<float>();
  m_l1Tau_phi               =new std::vector<float>();
}

L1TauContainer::~L1TauContainer()
{
  if(m_debug) std::cout << " Deleting L1TauContainer "  << std::endl;
  delete m_l1Tau_et;
  delete m_l1Tau_eta;
  delete m_l1Tau_phi;
}

void L1TauContainer::setTree(TTree *tree)
{
  ParticleContainer::setTree(tree);
  connectBranch<float>(tree,"et",&m_l1Tau_et);
  connectBranch<float>(tree,"eta",  &m_l1Tau_eta);
  connectBranch<float>(tree,"phi",  &m_l1Tau_phi);
}

void L1TauContainer::updateParticle(uint idx, Tau& tau)
{
  ParticleContainer::updateParticle(idx,tau);
}

void L1TauContainer::setBranches(TTree *tree)
{
  ParticleContainer::setBranches(tree);
  setBranch<float>(tree,"et",m_l1Tau_et); // et8x8 for the case of Legacy L1 tau RoIs
  setBranch<float>(tree,"eta",  m_l1Tau_eta);
  setBranch<float>(tree,"phi",  m_l1Tau_phi);
  return;
}

void L1TauContainer::clear()
{
  ParticleContainer::clear();
  m_l1Tau_et->clear();
  m_l1Tau_eta  ->clear();
  m_l1Tau_phi  ->clear();
  return;
}

/*
void L1TauContainer::FillLegacyL1Taus( const xAOD::TauRoIContainer* taus, bool sort){
  if(!sort) {
    for( auto tau_itr : *taus ) {
      m_l1Tau_et->push_back ( tau_itr->et8x8() / m_units );
      m_l1Tau_eta->push_back( tau_itr->eta() );
      m_l1Tau_phi->push_back( tau_itr->phi() );
    }
  } else {
    std::vector< std::vector<float> > vec;
    for( auto tau_itr : *taus ) {
      std::vector<float> row;
      row.clear();
      row.push_back(tau_itr->et8x8());
      row.push_back(tau_itr->eta());
      row.push_back(tau_itr->phi());
      vec.push_back(row);
    }
    
    std::sort(vec.begin(), vec.end(), [&](const std::vector<float> a, const std::vector<float> b) { return a.at(0) < b.at(0);});
    for (int i = int(vec.size())-1; i >= 0; i--) {
      m_l1Tau_et->push_back((vec.at(i)).at(0) / m_units);
      m_l1Tau_eta->push_back((vec.at(i)).at(1));
      m_l1Tau_phi->push_back((vec.at(i)).at(2));
    }
    vec.clear();
  }
}
*/
