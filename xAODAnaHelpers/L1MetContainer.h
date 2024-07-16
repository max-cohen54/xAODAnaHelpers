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
      //std::cout << met->size() << std::endl;
      if (met->size()>0) {
        float ex = 0;
        float ey = 0;
        int fpga_number;
        int sign = 1;
        for (const auto l1_met: *met) {
          
          // FOR MC ONLY::: flip the sign of the MET components for half of the processors
          fpga_number = static_cast<int>(l1_met->fpgaNumber());
          //sign = (fpga_number == 3) ? -1 : 1;
          ex += sign * l1_met->Ex();
          ey += sign * l1_met->Ey();
          //std::cout << "FPGA Number: " << static_cast<int>(fpga_number) << std::endl;
        }

        // Check for finite values
        if (TMath::Finite(ex) && TMath::Finite(ey)){
          float metMag = TMath::Sqrt(ex*ex + ey*ey);

          // Check for extremely large values
          if (metMag < 1e30){
            m_l1Met      = metMag / m_units;
            m_l1Met_Px   = ex / m_units;
            m_l1Met_Py   = ey / m_units;
            //m_l1Met_phi  = TMath::ATan(ey/ex);
            m_l1Met_phi = TMath::ATan2(ey, ex);
            //std::cout << "L1 MET phi: " << m_l1Met_phi << std::endl;
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
