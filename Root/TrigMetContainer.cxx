#include "xAODAnaHelpers/TrigMetContainer.h"
#include <iostream>

#include "TMath.h"

using namespace xAH;

TrigMetContainer::TrigMetContainer(const std::string& name, const std::string& detailStr, float units)
  : m_name(name), m_infoSwitch(detailStr), m_debug(false), m_units(units)
{
}

TrigMetContainer::~TrigMetContainer()
{
  if(m_debug) std::cout << " Deleting TrigMetContainer "  << std::endl;
}

void TrigMetContainer::setTree(TTree *tree)
{

  connectBranch<float>(tree, "Met",         &m_met     );
  connectBranch<float>(tree, "MetPx",       &m_metPx   );
  connectBranch<float>(tree, "MetPy",       &m_metPy   );
  connectBranch<float>(tree, "MetSumEt",    &m_metSumEt);
  connectBranch<float>(tree, "MetPhi",      &m_metPhi  );

}


void TrigMetContainer::setBranches(TTree *tree)
{

  setBranch(tree, "Met",         &m_met,      "F");
  setBranch(tree, "MetPx",       &m_metPx,    "F");
  setBranch(tree, "MetPy",       &m_metPy,    "F");
  setBranch(tree, "MetSumEt",    &m_metSumEt, "F");
  setBranch(tree, "MetPhi",      &m_metPhi,   "F");


  return;
}


void TrigMetContainer::clear()
{
  m_met      = -999;
  m_metPx    = -999;
  m_metPy    = -999;
  m_metSumEt = -999;
  m_metPhi   = -999;

  return;
}

void TrigMetContainer::FillTrigMET( const xAOD::TrigMissingETContainer* met) {


  //if ( m_debug ) { Info("HelpTreeBase::FillTrigMET()", "Filling Trig MET info"); }
  //std::cout<<"MET SIZE:::"<<met->size()<<std::endl;
  if (met->size()>0) {
    const xAOD::TrigMissingET* trig_met = met->front();
    m_met      = TMath::Sqrt(trig_met->ex()*trig_met->ex()+trig_met->ey()*trig_met->ey()) / m_units;
    m_metPx    = trig_met->ex() / m_units;
    m_metPy    = trig_met->ey() / m_units;
    m_metSumEt = trig_met->sumEt() / m_units;
    m_metPhi   = TMath::ATan2(trig_met->ey(),trig_met->ex());
  }

  return;
}
