#ifndef xAODAnaHelpers_L1MetContainer_H
#define xAODAnaHelpers_L1MetContainer_H



#include <TTree.h>
#include <TLorentzVector.h>

#include <vector>
#include <string>

#include <xAODAnaHelpers/HelperClasses.h>
#include <xAODAnaHelpers/HelperFunctions.h>

#include <xAODTrigger/jFexMETRoIContainer.h>
#include <xAODTrigger/jFexSumETRoIContainer.h>

#include <xAODAnaHelpers/HelperFunctions.h>


namespace xAH {

  class L1MetContainer
  {
  public:
    L1MetContainer(const std::string& name = "L1met", const std::string& detailStr="", float units = 1e3);
    ~L1MetContainer();

    void setTree(TTree *tree);
    void setBranches(TTree *tree);
    void clear();
    template <typename T_BR>
      void connectBranch(TTree *tree, std::string name, T_BR *variable);
    template <typename T_BR>
      void setBranch(TTree *tree, std::string name, T_BR *variable, std::string type);

    template <typename T>
    void FillL1Met(T*& met){
      //for( auto met_itr : *met ) {
      //  m_l1Met_Px->push_back ( static_cast<float>(met_itr->Ex()) / m_units );
      //}
      
      //float totalEx = 0.0;
      //float totalEy = 0.0;
      //for (auto met_itr : * met ){
      //  totalEx += met_itr->Ex();
      //  totalEy += met_itr->Ey();
      //}
 
      //m_l1Met     = TMath::Sqrt(totalEx*totalEx + totalEy*totalEy) / m_units;
      //m_l1Met_Px  = ( static_cast<float>(totalEx) / m_units );
      //m_l1Met_Py  = ( static_cast<float>(totalEy) / m_units );
      //m_l1Met_phi = TMath::ATan(totalEy/totalEx);

      if (met->size()>0) {
        const auto& l1_met = met->front();
        float ex = l1_met->Ex();
        float ey = l1_met->Ey();

        // Check for finite values
        if (TMath::Finite(ex) && TMath::Finite(ey)){
          float metMag = TMath::Sqrt(ex*ex + ey*ey);

          // Check for extremely large values
          if (metMag < 1e30){
            m_l1Met      = metMag / m_units;
            m_l1Met_Px   = ex / m_units;
            m_l1Met_Py   = ey / m_units;
            m_l1Met_phi  = TMath::ATan(ey/ex);
          }
        }  
      }

      return;

    } // FillL1Met
    
  public:

    // MET
    float m_l1Met;
    float m_l1Met_Px;
    float m_l1Met_Py;
    float m_l1Met_phi;

  public:

    std::string m_name;
    HelperClasses::METInfoSwitch m_infoSwitch;
    bool m_debug;
    float m_units;
  };

  template <typename T_BR> void L1MetContainer::connectBranch(TTree *tree, std::string name, T_BR *variable)
    {
      tree->SetBranchStatus  ((m_name + name).c_str()  , 1);
      tree->SetBranchAddress ((m_name + name).c_str()  , variable);
    }
  template <typename T_BR> void L1MetContainer::setBranch(TTree *tree, std::string name, T_BR *variable, std::string type)
    {
      if (!type.empty()) {
        tree->Branch((m_name + name).c_str(), variable, (m_name + name + "/" + type).c_str());
      } else {
        tree->Branch((m_name + name).c_str(), variable);
      }  
    }

}





#endif // xAODAnaHelpers_L1MetContainer
