#ifndef xAODAnaHelpers_L1EMContainer_H
#define xAODAnaHelpers_L1EMContainer_H

#include <TTree.h>
#include <TLorentzVector.h>

#include <vector>
#include <string>

#include <xAODAnaHelpers/HelperClasses.h>
#include <xAODAnaHelpers/HelperFunctions.h>

#include <xAODTrigger/eFexEMRoIContainer.h>
#include <xAODTrigger/jFexFwdElRoIContainer.h>

#include <xAODAnaHelpers/Photon.h>
#include <xAODAnaHelpers/ParticleContainer.h>

namespace xAH {

    class L1EMContainer : public ParticleContainer<Photon,HelperClasses::PhotonInfoSwitch>
    {
    public:
      L1EMContainer(const std::string& name = "L1Egamma", float units = 1e3, bool mc = false);
      virtual ~L1EMContainer();
      virtual void setTree    (TTree *tree);
      virtual void setBranches(TTree *tree);
      virtual void clear();

      virtual void updateParticle(uint idx, Photon& egamma);

      template <typename T>
      void FillL1Egammas(T*& egammas, bool sort){
        if(!sort) {
          for( auto egamma_itr : *egammas ) {
            m_l1Egamma_et->push_back ( static_cast<float>(egamma_itr->et()) / m_units );            
            m_l1Egamma_eta->push_back( egamma_itr->eta() );
            m_l1Egamma_phi->push_back( egamma_itr->phi() );
          }
        } else {
          std::vector< std::vector<float> > vec;
          for( auto egamma_itr : *egammas ) {
            std::vector<float> row;
            row.clear();
            row.push_back ( static_cast<float>(egamma_itr->et()) / m_units );
            row.push_back(egamma_itr->eta());
            row.push_back(egamma_itr->phi());
            vec.push_back(row);
          }
          
          std::sort(vec.begin(), vec.end(), [&](const std::vector<float> a, const std::vector<float> b) { return a.at(0) < b.at(0);});
          for (int i = int(vec.size())-1; i >= 0; i--) {
            m_l1Egamma_et->push_back((vec.at(i)).at(0) / m_units);
            m_l1Egamma_eta->push_back((vec.at(i)).at(1));
            m_l1Egamma_phi->push_back((vec.at(i)).at(2));
          }
          vec.clear();
        }
      }

    private:
      // Vector branches
      std::vector<float>* m_l1Egamma_et;
      std::vector<float>* m_l1Egamma_eta;
      std::vector<float>* m_l1Egamma_phi;
    };
}


#endif // xAODAnaHelpers_L1EMContainer_H
