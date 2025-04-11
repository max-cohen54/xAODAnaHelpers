#ifndef xAODAnaHelpers_L1TauContainer_H
#define xAODAnaHelpers_L1TauContainer_H

#include <TTree.h>
#include <TLorentzVector.h>

#include <vector>
#include <string>

#include <xAODAnaHelpers/HelperClasses.h>
#include <xAODAnaHelpers/HelperFunctions.h>

#include <xAODTrigger/EmTauRoIContainer.h>
#include <xAODTrigger/eFexTauRoIContainer.h>
#include <xAODTrigger/jFexTauRoIContainer.h>

#include <xAODAnaHelpers/Tau.h>
#include <xAODAnaHelpers/ParticleContainer.h>

namespace xAH {

    class L1TauContainer : public ParticleContainer<Tau,HelperClasses::TauInfoSwitch>
    {
    public:
      L1TauContainer(const std::string& name = "L1Tau", float units = 1e3, bool mc = false);
      virtual ~L1TauContainer();

      virtual void setTree    (TTree *tree);
      virtual void setBranches(TTree *tree);
      virtual void clear();
      //virtual void FillLegacyL1Taus( const xAOD::TauRoIContainer* taus, bool sort);
      virtual void updateParticle(uint idx, Tau& tau);

      // access to et, eta and phi is the same for all the Fex L1 tau collections
      template <typename T>
      void FillFexL1Taus(T*& taus, bool sort){
        if(!sort) {
          for( auto tau_itr : *taus ) {
            m_l1Tau_et->push_back ( static_cast<float>(tau_itr->et()) / m_units );            
            m_l1Tau_eta->push_back( tau_itr->eta() );
            m_l1Tau_phi->push_back( tau_itr->phi() );
          }
        } else {
          std::vector< std::vector<float> > vec;
          for( auto tau_itr : *taus ) {
            std::vector<float> row;
            row.clear();
            row.push_back ( static_cast<float>(tau_itr->et()) / m_units );
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
    
      // access to et, eta and phi is the same for the Em L1 tau collections (could avoid templating...)
      template <typename T>
      void FillEmL1Taus(T*& taus, bool sort){
        if(!sort) {
          for( auto tau_itr : *taus ) {
            m_l1Tau_et->push_back ( static_cast<float>(tau_itr->eT()) / m_units );            
            m_l1Tau_eta->push_back( tau_itr->eta() );
            m_l1Tau_phi->push_back( tau_itr->phi() );
          }
        } else {
          std::vector< std::vector<float> > vec;
          for( auto tau_itr : *taus ) {
            std::vector<float> row;
            row.clear();
            row.push_back ( static_cast<float>(tau_itr->eT()) / m_units );
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
      

    private:
      // Vector branches
      std::vector<float>* m_l1Tau_et;
      std::vector<float>* m_l1Tau_eta;
      std::vector<float>* m_l1Tau_phi;
    };

}

#endif // xAODAnaHelpers_L1TauContainer_H
