#ifndef xAODAnaHelpers_L1MuonContainer_H
#define xAODAnaHelpers_L1MuonContainer_H

#include <TTree.h>
#include <TLorentzVector.h>

#include <vector>
#include <string>

#include <xAODAnaHelpers/HelperClasses.h>
#include <xAODAnaHelpers/HelperFunctions.h>

#include <xAODTrigger/MuonRoI.h> // Assuming MuonRoI.h is the correct header
#include <xAODTrigger/MuonRoIContainer.h>

#include <xAODAnaHelpers/Muon.h>
#include <xAODAnaHelpers/ParticleContainer.h>

namespace xAH {

    class L1MuonContainer : public ParticleContainer<Muon,HelperClasses::MuonInfoSwitch>
    {
    public:
      L1MuonContainer(const std::string& name = "L1Muon", float units = 1e3, bool mc = false);
      virtual ~L1MuonContainer();

      virtual void setTree(TTree *tree);
      virtual void setBranches(TTree *tree);
      virtual void clear();
      virtual void updateParticle(uint idx, Muon& muon);
      
      // Access to et, eta, phi is the same for the Muon collections
      template <typename T>
      void FillL1Muons(T*& muons, bool sort) {
        float et = 0.0;
        if (!sort) {
          for (auto muon_itr : *muons) {
            et = calculateEt(*muon_itr); // Calculate muon et
            m_l1Muon_et->push_back(et / m_units);
            m_l1Muon_eta->push_back( muon_itr->eta() );
            m_l1Muon_phi->push_back( muon_itr->phi() );
          } // for muons
        } else {
          std::vector< std::vector<float> > vec;
          for( auto muon_itr : *muons ){
            std::vector<float> row;
            row.clear();
            row.push_back ( et / m_units );
            row.push_back(muon_itr->eta());
            row.push_back(muon_itr->phi());
            vec.push_back(row); 
          } //for muons

          std::sort(vec.begin(), vec.end(), [&](const std::vector<float> a, const std::vector<float> b) { return a.at(0) < b.at(0);});
          for (int i = int(vec.size())-1; i >= 0; i--) {
            m_l1Muon_et->push_back((vec.at(i)).at(0) / m_units);
            m_l1Muon_eta->push_back((vec.at(i)).at(1));
            m_l1Muon_phi->push_back((vec.at(i)).at(2));
          }
          vec.clear();
        } // else
      } // FillL1Muons

    private:
      // Method to calculate ET
      float calculateEt(const xAOD::MuonRoI& muonRoI) {

        std::unordered_map<int, int> rpcPtValues = { // Barrel
          {1, 4},
          {2, 6},
          {3, 8},
          {4, 10},
          {5, 12},
          {6, 14}
        };      

        std::unordered_map<int, int> tgcPtValues = { // endCap
          {1, 3},
          {2, 4},
          {3, 5},
          {4, 6},
          {5, 7},
          {6, 8},
          {7, 9},
          {8, 10},
          {9, 11},
          {10, 12},
          {11, 13},
          {12, 14},
          {13, 15},
          {14, 18},
          {15, 20}
        };
 
        float et = 0.0;
        int thrNumber = muonRoI.getThrNumber();
        if (muonRoI.getSource() == xAOD::MuonRoI::RoISource::Barrel) { //RPC
          et = rpcPtValues.at(thrNumber); // map is in GeV
        } else {
          et = tgcPtValues.at(thrNumber); // map is in GeV
        }

        return et;
      }

      // Vector branches
      std::vector<float>* m_l1Muon_et;
      std::vector<float>* m_l1Muon_eta;
      std::vector<float>* m_l1Muon_phi;

      // Additional class members if needed
    };

}

#endif // xAODAnaHelpers_L1MuonContainer_H
