#ifndef xAODAnaHelpers_TrigMetContainer_H
#define xAODAnaHelpers_TrigMetContainer_H

#include <TTree.h>

#include <string>

#include "xAODTrigMissingET/TrigMissingETContainer.h"

#include <xAODAnaHelpers/HelperClasses.h>

namespace xAH {

  class TrigMetContainer
  {
  public:
    TrigMetContainer(const std::string& name = "trigmet", const std::string& detailStr="", float units = 1e3);
    ~TrigMetContainer();

    void setTree    (TTree *tree);
    void setBranches(TTree *tree);
    void clear();
    void FillTrigMET( const xAOD::TrigMissingETContainer* met);
    template <typename T_BR>
      void connectBranch(TTree *tree, std::string name, T_BR *variable);
    template <typename T_BR>
      void setBranch(TTree *tree, std::string name, T_BR *variable, std::string type);

  public:

    std::string m_name;
    HelperClasses::METInfoSwitch  m_infoSwitch;
    bool m_debug;
    float m_units;

  public:

    // met
    float m_met;
    float m_metPx;
    float m_metPy;
    float m_metPhi;
    float m_metSumEt;
    

  };

  template <typename T_BR> void TrigMetContainer::connectBranch(TTree *tree, std::string name, T_BR *variable)
    {
      tree->SetBranchStatus  ((m_name + name).c_str()  , 1);
      tree->SetBranchAddress ((m_name + name).c_str()  , variable);
    }
  template <typename T_BR> void TrigMetContainer::setBranch(TTree *tree, std::string name, T_BR *variable, std::string type)
    {
      if (!type.empty()) {
        tree->Branch((m_name + name).c_str(), variable, (m_name + name + "/" + type).c_str());
      } else {
        tree->Branch((m_name + name).c_str(), variable);
      }
    }

}



#endif // xAODAnaHelpers_TrigMetContainer_H
