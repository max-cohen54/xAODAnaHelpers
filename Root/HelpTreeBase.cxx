// c++ include(s):
#include <iostream>
#include <exception>
#include <sstream>

// EDM include(s):
#include "xAODBTagging/BTagging.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackSummaryAccessors_v1.h"
#include "xAODJet/JetConstituentVector.h"
#include "xAODPrimitives/IsolationType.h"


#include "TrigConfxAOD/xAODConfigTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

// package include(s):
#include <xAODAnaHelpers/HelperFunctions.h>
#include <xAODAnaHelpers/HelpTreeBase.h>
#include <AsgMessaging/MessageCheck.h>


#include "AsgMessaging/StatusCode.h"

using std::vector;

// needed? should it be here?
#ifdef __MAKECINT__
#pragma link C++ class vector<float>+;
#endif

HelpTreeBase::HelpTreeBase(xAOD::TEvent* event, TTree* tree, TFile* file, const float units, bool debug, xAOD::TStore* store, std::string nominalTreeName):
  m_trigInfoSwitch(nullptr),
  m_trigConfTool(nullptr),
  m_trigDecTool(nullptr),
  m_eventInfo(nullptr)
{

  m_units = units;
  m_debug = debug;
  m_tree = tree;
  m_tree->SetDirectory( file );
  m_nominalTreeName = nominalTreeName;
  m_nominalTree = strcmp(m_tree->GetName(), nominalTreeName.c_str()) == 0;
  m_event = event;
  m_store = store;
  Info("HelpTreeBase()", "HelpTreeBase setup");

  // turn things off it this is data...since TStore is not a needed input
  // default isMC to true so more is added to the tree than less
  const xAOD::EventInfo* eventInfo(nullptr);
  HelperFunctions::retrieve(eventInfo, "EventInfo", m_event, m_store);
  m_isMC = ( eventInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) );

}

HelpTreeBase::~HelpTreeBase() {

    //delete all the info switches that have been built earlier on
    //event
    if(m_eventInfo) delete m_eventInfo;

    //trig
    if(m_trigInfoSwitch) delete m_trigInfoSwitch;

    //muon
    for (auto muon: m_muons)
      delete muon.second;

    //el
    for (auto elec: m_elecs)
      delete elec.second;

    //ph
    for (auto photon: m_photons)
      delete photon.second;

    //cl
    for (auto cluster: m_clusters)
      delete cluster.second;

    //fatjet
    for (auto fatjet: m_fatjets)
      delete fatjet.second;

    //tau
    for (auto tau: m_taus)
      delete tau.second;

    //met
    for (auto met: m_met)
      delete met.second;

    //mettrig
    for (auto met: m_trigmet)
      delete met.second;

    //jet
    for (auto jet: m_jets)
      delete jet.second;

    //truth fat jet
    for (auto truth_fatjet: m_truth_fatjets)
      delete truth_fatjet.second;

    //truth
    for (auto truth: m_truth)
      delete truth.second;

    //track
    for (auto track: m_tracks)
      delete track.second;

}


HelpTreeBase::HelpTreeBase(TTree* tree, TFile* file, xAOD::TEvent* event, xAOD::TStore* store, const float units, bool debug, std::string nominalTreeName):
  HelpTreeBase(event, tree, file, units, debug, store, nominalTreeName)
{
  // use the other constructor for everything
}


void HelpTreeBase::Fill() {
  // code_or_nbytes is the number of bytes written, or -1 if an error occured
  const int code_or_nbytes = m_tree->Fill();
  if(code_or_nbytes < 0) // 0 can be OK if all branches are disabled and thus no data is written
  {
    auto msg = std::stringstream();
    msg << "HelpTreeBase::Fill(): error in TTree::Fill() : returned " << code_or_nbytes;
    throw std::runtime_error(msg.str());
  }
}

/*********************
 *
 *   EVENT
 *
 ********************/

void HelpTreeBase::AddEvent( const std::string& detailStr ) {

  if(m_debug)  Info("AddEvent()", "Adding event variables: %s", detailStr.c_str());

  m_eventInfo       = new xAH::EventInfo(detailStr, m_units, m_isMC, m_nominalTree);
  m_eventInfo -> setBranches(m_tree);
  this->AddEventUser(detailStr);
}

void HelpTreeBase::FillEvent( const xAOD::EventInfo* eventInfo, xAOD::TEvent* /*event*/, const xAOD::VertexContainer* vertices ) {

  this->ClearEvent();

  // only retrieve the vertex container if it's not set and the user asks for that information
  if( m_eventInfo->m_infoSwitch.m_pileup && !vertices ) {
    HelperFunctions::retrieve( vertices, m_vertexContainerName, m_event, 0);
  }

  m_eventInfo->FillEvent(eventInfo, m_event, vertices);

  this->FillEventUser(eventInfo);
}

/*********************
 *
 *   TRIGGER
 *
 ********************/
void HelpTreeBase::AddTrigger( const std::string& detailStr ) {

  if(m_debug) Info("AddTrigger()", "Adding trigger variables: %s", detailStr.c_str());

  m_trigInfoSwitch = new HelperClasses::TriggerInfoSwitch( detailStr );

  // Add these basic branches
  if ( m_trigInfoSwitch->m_basic ) {
    m_tree->Branch("passL1",          &m_passL1,       "passL1/I"      );
    m_tree->Branch("passHLT",         &m_passHLT,      "passHLT/I"     );
  }

  // Detailed trigger infoformation related to the menu (might be useful)
  // This is useful for using this database: https://atlas-trigconf.cern.ch/
  if ( m_trigInfoSwitch->m_menuKeys ) {
    m_tree->Branch("masterKey",       &m_masterKey,    "masterKey/I"         );
    m_tree->Branch("lvl1PrescaleKey", &m_L1PSKey,      "lvl1PrescaleKey/I"   );
    m_tree->Branch("hltPrescaleKey",  &m_HLTPSKey,     "hltPrescaleKey/I"    );
  }

  // Trigger Decision for each and every trigger in a vector
  if ( m_trigInfoSwitch->m_passTriggers ) {
    // vector of strings for trigger names which fired
    m_tree->Branch("passedTriggers",       &m_passedTriggers      );
    m_tree->Branch("disabledTriggers",     &m_disabledTriggers    );
  }

  if ( !m_isMC && m_trigInfoSwitch->m_prescales ) {
    m_tree->Branch("triggerPrescales",     &m_triggerPrescales    );
  }

  if ( !m_isMC && m_trigInfoSwitch->m_prescalesLumi ) {
    m_tree->Branch("triggerPrescalesLumi", &m_triggerPrescalesLumi);
  }

  if ( m_trigInfoSwitch->m_passTrigBits ) {
    m_tree->Branch("isPassBits",           &m_isPassBits          );
    m_tree->Branch("isPassBitsNames",      &m_isPassBitsNames     );
  }

  this->AddTriggerUser( detailStr );
}

// Fill the information in the trigger branches
void HelpTreeBase::FillTrigger( const xAOD::EventInfo* eventInfo ) {

  if ( m_debug ) { Info("HelpTreeBase::FillTrigger()", "Filling trigger info"); }

  // Clear previous events
  this->ClearTrigger();
  this->ClearTriggerUser();

  // Grab the global pass information from the TrigDecisionTool
  if ( m_trigInfoSwitch->m_basic ) {

    if ( m_debug ) { Info("HelpTreeBase::FillTrigger()", "Switch: m_trigInfoSwitch->m_basic"); }

    static SG::AuxElement::ConstAccessor< int > passAny("passAny");
    static SG::AuxElement::ConstAccessor< int > passL1("passL1");
    if( passL1.isAvailable( *eventInfo ) ) { m_passL1 = passL1( *eventInfo ); }
    else { m_passL1 = -1; }
    static SG::AuxElement::ConstAccessor< int > passHLT("passHLT");
    if( passHLT.isAvailable( *eventInfo ) ) { m_passHLT = passHLT( *eventInfo ); }
    else { m_passHLT = -1; }

  }

  // If detailed menu information about the configuration keys, turn this on.
  // This is useful for using this database: https://atlas-trigconf.cern.ch/
  if ( m_trigInfoSwitch->m_menuKeys ) {

    if ( m_debug ) { Info("HelpTreeBase::FillTrigger()", "Switch: m_trigInfoSwitch->m_menuKeys"); }

    static SG::AuxElement::ConstAccessor< int > masterKey("masterKey");
    if( masterKey.isAvailable( *eventInfo ) ) { m_masterKey = masterKey( *eventInfo ); }
    else { m_masterKey = -999; }
    static SG::AuxElement::ConstAccessor< int > L1PSKey("L1PSKey");
    if( L1PSKey.isAvailable( *eventInfo ) ) { m_L1PSKey = L1PSKey( *eventInfo ); }
    else { m_L1PSKey = -999; }
    static SG::AuxElement::ConstAccessor< int > HLTPSKey("HLTPSKey");
    if( HLTPSKey.isAvailable( *eventInfo ) ) { m_HLTPSKey = HLTPSKey( *eventInfo ); }
    else { m_HLTPSKey = -999; }

  }

  // If detailed information about each and every trigger is desired
  // save a vector of strings holding passing decisions
  if ( m_trigInfoSwitch->m_passTriggers ) {

    if ( m_debug ) { Info("HelpTreeBase::FillTrigger()", "Switch: m_trigInfoSwitch->m_passTriggers"); }
    static SG::AuxElement::ConstAccessor< std::vector< std::string > > acc_passedTriggers  ("passedTriggers");
    if( acc_passedTriggers  .isAvailable( *eventInfo ) ) { m_passedTriggers   = acc_passedTriggers  ( *eventInfo ); }
    static SG::AuxElement::ConstAccessor< std::vector< std::string > > acc_disabledTriggers("disabledTriggers");
    if( acc_disabledTriggers.isAvailable( *eventInfo ) ) { m_disabledTriggers = acc_disabledTriggers( *eventInfo ); }
  }

  if ( !m_isMC && m_trigInfoSwitch->m_prescales ) {

    if ( m_debug ) { Info("HelpTreeBase::FillTrigger()", "Switch: m_trigInfoSwitch->m_prescales"); }

    static SG::AuxElement::ConstAccessor< std::vector< float > > trigPrescales("triggerPrescales");
    if( trigPrescales.isAvailable( *eventInfo ) ) { m_triggerPrescales = trigPrescales( *eventInfo ); }

  }

  if ( !m_isMC && m_trigInfoSwitch->m_prescalesLumi ) {

    if ( m_debug ) { Info("HelpTreeBase::FillTrigger()", "Switch: m_trigInfoSwitch->m_prescalesLumi"); }

    static SG::AuxElement::ConstAccessor< std::vector< float > > trigPrescalesLumi("triggerPrescalesLumi");
    if( trigPrescalesLumi.isAvailable( *eventInfo ) ) { m_triggerPrescalesLumi = trigPrescalesLumi( *eventInfo ); }

  }

  if ( m_trigInfoSwitch->m_passTrigBits ) {

    static SG::AuxElement::ConstAccessor< std::vector< unsigned int > > isPassBits("isPassedBits");
    if( isPassBits.isAvailable( *eventInfo ) ) { m_isPassBits = isPassBits( *eventInfo ); }

    static SG::AuxElement::ConstAccessor< std::vector< std::string > > isPassBitsNames("isPassedBitsNames");
    if( isPassBitsNames.isAvailable( *eventInfo ) ) { m_isPassBitsNames = isPassBitsNames( *eventInfo ); }

  }
  
  this->FillTriggerUser(eventInfo);
}

// Clear Trigger
void HelpTreeBase::ClearTrigger() {

  m_passL1  = -999;
  m_passHLT = -999;

  m_masterKey = 0;
  m_L1PSKey   = 0;
  m_HLTPSKey  = 0;

  m_passedTriggers.clear();
  m_disabledTriggers.clear();
  m_triggerPrescales.clear();
  m_triggerPrescalesLumi.clear();
  m_isPassBits.clear();
  m_isPassBitsNames.clear();

}

/*********************
 *
 *   JET TRIGGER
 *
 ********************/

/* TODO: jet trigger */
//CD: is this useful at all?
void HelpTreeBase::AddJetTrigger( const std::string& detailStr )
{
  if ( m_debug )  Info("AddJetTrigger()", "Adding jet trigger variables: %s", detailStr.c_str());
}
void HelpTreeBase::FillJetTrigger(  ) { }
void HelpTreeBase::ClearJetTrigger(  ) { }


/*********************
 *
 *   MUONS
 *
 ********************/

void HelpTreeBase::AddMuons(const std::string& detailStr, const std::string& muonName) {

  if ( m_debug )  Info("AddMuons()", "Adding muon variables: %s", detailStr.c_str());

  m_muons[muonName] = new xAH::MuonContainer(muonName, detailStr, m_units, m_isMC, m_nominalTree);
  xAH::MuonContainer* thisMuon = m_muons[muonName];
  HelperClasses::MuonInfoSwitch& muonInfoSwitch = thisMuon->m_infoSwitch;

  if (m_nominalTree) {

     if ( muonInfoSwitch.m_recoEff_sysNames && muonInfoSwitch.m_effSF && m_isMC ) {
       for (auto& reco : muonInfoSwitch.m_recoWPs) {
         std::string recoEffSF_sysNames = "muon_RecoEff_SF_" + reco + "_sysNames";
         m_tree->Branch( recoEffSF_sysNames.c_str() , & (m_MuonRecoEff_SF_sysNames)[ reco ] );
       }
     }

     if ( muonInfoSwitch.m_isoEff_sysNames && muonInfoSwitch.m_effSF && m_isMC ) {
       for (auto& isol : muonInfoSwitch.m_isolWPs) {
         std::string isolEffSF_sysNames = "muon_IsoEff_SF_" + isol + "_sysNames";
         m_tree->Branch( isolEffSF_sysNames.c_str() , & (m_MuonIsoEff_SF_sysNames)[ isol ] );
       }
     }

     if ( muonInfoSwitch.m_trigEff_sysNames && muonInfoSwitch.m_effSF && m_isMC ) {
       for (auto& trig : muonInfoSwitch.m_trigWPs) {
         for (auto& reco : muonInfoSwitch.m_recoWPs) {
           std::string trigEffSF_sysNames = (muonInfoSwitch.m_recoWPs.size()>1) ? "muon_TrigEff_SF_" + trig + "_" + reco + "_sysNames" : "muon_TrigEff_SF_" + trig + "_sysNames";
           m_tree->Branch( trigEffSF_sysNames.c_str() , & (m_MuonTrigEff_SF_sysNames)[ trig ][ reco ] );
         }
       }
     }

     if ( muonInfoSwitch.m_ttvaEff_sysNames && muonInfoSwitch.m_effSF && m_isMC ) {
       std::string ttvaEffSF_sysNames = "muon_TTVAEff_SF_sysNames";
       m_tree->Branch( ttvaEffSF_sysNames.c_str() , &m_MuonTTVAEff_SF_sysNames );
     }

  }

  thisMuon->setBranches(m_tree);
  this->AddMuonsUser(detailStr, muonName);
}

void HelpTreeBase::FillMuons( const xAOD::MuonContainer* muons, const xAOD::Vertex* primaryVertex, const std::string& muonName ) {

  this->ClearMuons(muonName);
  HelperClasses::MuonInfoSwitch& muonInfoSwitch = m_muons[muonName]->m_infoSwitch;

  if (m_nominalTree) {

    if ( muonInfoSwitch.m_recoEff_sysNames && muonInfoSwitch.m_effSF && m_isMC ) {
      for ( auto& reco : muonInfoSwitch.m_recoWPs ) {
        std::vector< std::string >* tmp_reco_sys(nullptr);
        if ( m_store->retrieve(tmp_reco_sys, "MuonEfficiencyCorrector_RecoSyst_Reco" + reco).isSuccess() ) {
          (m_MuonRecoEff_SF_sysNames)[ reco ] = *tmp_reco_sys;
        }
      }
    }

    if ( muonInfoSwitch.m_isoEff_sysNames && muonInfoSwitch.m_effSF && m_isMC ) {
      for ( auto& isol : muonInfoSwitch.m_isolWPs ) {
        std::vector< std::string >* tmp_iso_sys(nullptr);
        if ( m_store->retrieve(tmp_iso_sys, "MuonEfficiencyCorrector_IsoSyst_Iso" + isol).isSuccess() ) {
          (m_MuonIsoEff_SF_sysNames)[ isol ] = *tmp_iso_sys;
        }
      }
    }

    if ( muonInfoSwitch.m_trigEff_sysNames && muonInfoSwitch.m_effSF && m_isMC ) {
      for ( auto& trig : muonInfoSwitch.m_trigWPs ) {
        for ( auto& reco : muonInfoSwitch.m_recoWPs ) {
          std::vector< std::string >* tmp_trig_sys(nullptr);
          if ( m_store->retrieve(tmp_trig_sys, "MuonEfficiencyCorrector_TrigSyst_" + trig + "_Reco" + reco).isSuccess() ) {
            (m_MuonTrigEff_SF_sysNames)[ trig ][ reco ] = *tmp_trig_sys;
          }
        }
      }
    }

    if ( muonInfoSwitch.m_ttvaEff_sysNames && muonInfoSwitch.m_effSF && m_isMC ) {
      std::vector< std::string >* tmp_ttva_sys(nullptr);
      if ( m_store->retrieve(tmp_ttva_sys, "MuonEfficiencyCorrector_TTVASyst_TTVA").isSuccess() ) {
        m_MuonTTVAEff_SF_sysNames = *tmp_ttva_sys;
      }
    }

  }

  for( auto muon_itr : *muons ) {
    this->FillMuon(muon_itr, primaryVertex, muonName);
  }

}

void HelpTreeBase::FillMuon( const xAOD::Muon* muon, const xAOD::Vertex* primaryVertex, const std::string& muonName ) {

  xAH::MuonContainer* thisMuon = m_muons[muonName];

  thisMuon->FillMuon(muon, primaryVertex);

  this->FillMuonsUser(muon, muonName, primaryVertex);

  return;
}

void HelpTreeBase::ClearMuons(const std::string& muonName) {

  xAH::MuonContainer* thisMuon = m_muons[muonName];
  HelperClasses::MuonInfoSwitch& muonInfoSwitch = thisMuon->m_infoSwitch;

  if (m_nominalTree) {

    if ( muonInfoSwitch.m_recoEff_sysNames && muonInfoSwitch.m_effSF && m_isMC ) {
      for ( auto& reco : muonInfoSwitch.m_recoWPs ) {
          (m_MuonRecoEff_SF_sysNames)[ reco ].clear();
      }
    }

    if ( muonInfoSwitch.m_isoEff_sysNames && muonInfoSwitch.m_effSF && m_isMC ) {
      for ( auto& isol : muonInfoSwitch.m_isolWPs ) {
          (m_MuonIsoEff_SF_sysNames)[ isol ].clear();
      }
    }

    if ( muonInfoSwitch.m_trigEff_sysNames && muonInfoSwitch.m_effSF && m_isMC ) {
      for ( auto& trig : muonInfoSwitch.m_trigWPs ) {
        for ( auto& reco : muonInfoSwitch.m_recoWPs ) {
            (m_MuonTrigEff_SF_sysNames)[ trig ][ reco ].clear();
        }
      }
    }

    if ( muonInfoSwitch.m_ttvaEff_sysNames && muonInfoSwitch.m_effSF && m_isMC ) {
       m_MuonTTVAEff_SF_sysNames.clear();
    }

  }

  thisMuon->clear();

  this->ClearMuonsUser(muonName);

}

/*********************
 *
 *   ELECTRONS
 *
 ********************/

void HelpTreeBase::AddElectrons(const std::string& detailStr, const std::string& elecName) {

  if(m_debug)  Info("AddElectrons()", "Adding electron variables: %s", detailStr.c_str());

  m_elecs[elecName] = new xAH::ElectronContainer(elecName, detailStr, m_units, m_isMC, m_nominalTree);

  xAH::ElectronContainer* thisElec = m_elecs[elecName];

  thisElec->setBranches(m_tree);
  this->AddElectronsUser(detailStr, elecName);
}


void HelpTreeBase::FillElectrons( const xAOD::ElectronContainer* electrons, const xAOD::Vertex* primaryVertex, const std::string& elecName ) {

  this->ClearElectrons(elecName);

  for ( auto el_itr : *electrons ) {
    this->FillElectron(el_itr, primaryVertex, elecName);
  }
}

void HelpTreeBase::FillElectron ( const xAOD::Electron* elec, const xAOD::Vertex* primaryVertex, const std::string& elecName ) {

  xAH::ElectronContainer* thisElec = m_elecs[elecName];

  thisElec->FillElectron(elec, primaryVertex);

  this->FillElectronsUser(elec, elecName, primaryVertex);

  return;
}


void HelpTreeBase::ClearElectrons(const std::string& elecName) {

  xAH::ElectronContainer* thisElec = m_elecs[elecName];
  thisElec->clear();

  this->ClearElectronsUser(elecName);
}

/*********************
 *
 *   PHOTONS
 *
 ********************/

void HelpTreeBase::AddPhotons(const std::string& detailStr, const std::string& photonName) {

  if(m_debug)  Info("AddPhotons()", "Adding photon variables: %s", detailStr.c_str());

  m_photons[photonName] = new xAH::PhotonContainer(photonName, detailStr, m_units, m_isMC);

  xAH::PhotonContainer* thisPhoton = m_photons[photonName];

  thisPhoton->setBranches(m_tree);
  this->AddPhotonsUser(detailStr, photonName);
}


void HelpTreeBase::FillPhotons( const xAOD::PhotonContainer* photons, const std::string& photonName ) {

  this->ClearPhotons(photonName);

  for ( auto ph_itr : *photons ) {
    this->FillPhoton(ph_itr, photonName);
  }
}

void HelpTreeBase::FillPhoton( const xAOD::Photon* photon, const std::string& photonName ) {

  xAH::PhotonContainer* thisPhoton = m_photons[photonName];

  thisPhoton->FillPhoton(photon);

  this->FillPhotonsUser(photon, photonName);

  return;
}


void HelpTreeBase::ClearPhotons(const std::string& photonName) {

  xAH::PhotonContainer* thisPhoton = m_photons[photonName];
  thisPhoton->clear();

  this->ClearPhotonsUser(photonName);
}

/*********************
 *
 * CLUSTERS
 *
 *********************/

void HelpTreeBase::AddClusters(const std::string& detailStr, const std::string& clusterName) {

  if(m_debug)  Info("AddClusters()", "Adding cluster variables: %s", detailStr.c_str());

  m_clusters[clusterName] = new xAH::ClusterContainer(clusterName, detailStr, m_units, m_isMC);

  xAH::ClusterContainer* thisCluster = m_clusters[clusterName];

  thisCluster->setBranches(m_tree);
  this->AddClustersUser(detailStr, clusterName);
}


void HelpTreeBase::FillClusters( const xAOD::CaloClusterContainer* clusters, const std::string& clusterName ) {

  this->ClearClusters(clusterName);

  for ( auto cl_itr : *clusters ) {
    this->FillCluster(cl_itr, clusterName);
  }
}

void HelpTreeBase::FillCluster( const xAOD::CaloCluster* cluster, const std::string& clusterName ) {

  xAH::ClusterContainer* thisCluster = m_clusters[clusterName];

  thisCluster->FillCluster(cluster);

  this->FillClustersUser(cluster, clusterName);

  return;
}


void HelpTreeBase::ClearClusters(const std::string& clusterName) {

  xAH::ClusterContainer* thisCluster = m_clusters[clusterName];
  thisCluster->clear();

  this->ClearClustersUser(clusterName);
}

/*********************
 *
 *   L1 JETS
 *
 ********************/

void HelpTreeBase::AddL1Jets( const std::string& jetName)
{

  if(m_debug) Info("AddL1Jets()", "Adding %s L1 jets", jetName.c_str());

  m_l1Jets[jetName] = new xAH::L1JetContainer(jetName, m_units, m_isMC);
  m_l1Jets[jetName]->m_debug = m_debug;

  xAH::L1JetContainer* thisL1Jet = m_l1Jets[jetName];
  thisL1Jet->setBranches(m_tree);

}

void HelpTreeBase::FillLegacyL1Jets( const xAOD::JetRoIContainer* jets, const std::string& jetName, bool sortL1Jets ) {

  this->ClearL1Jets(jetName);

  xAH::L1JetContainer* thisL1Jet = m_l1Jets[jetName];
  
  thisL1Jet->FillLegacyL1Jets(jets,sortL1Jets);

}

void HelpTreeBase::ClearL1Jets(const std::string& jetName) {

  xAH::L1JetContainer* thisL1Jet = m_l1Jets[jetName];
  thisL1Jet->clear();

}


/*********************
 *
 *   JETS
 *
 ********************/

void HelpTreeBase::AddJets(const std::string& detailStr, const std::string& jetName)
{

  if(m_debug) Info("AddJets()", "Adding jet %s with variables: %s", jetName.c_str(), detailStr.c_str());

  m_jets[jetName] = new xAH::JetContainer(jetName, detailStr, m_units, m_isMC);
  m_jets[jetName]->m_debug = m_debug;

  xAH::JetContainer* thisJet = m_jets[jetName];
  thisJet->setBranches(m_tree);
  this->AddJetsUser(detailStr, jetName);

}


void HelpTreeBase::FillJets( const xAOD::JetContainer* jets, int pvLocation, const std::string& jetName ) {

  this->ClearJets(jetName);

  const xAOD::VertexContainer* vertices(nullptr);
  const xAOD::Vertex *pv = 0;

  xAH::JetContainer* thisJet = m_jets[jetName];

  if( thisJet->m_infoSwitch.m_trackPV || thisJet->m_infoSwitch.m_allTrack ) {
    HelperFunctions::retrieve( vertices, m_vertexContainerName, m_event, 0);
    pvLocation = HelperFunctions::getPrimaryVertexLocation( vertices );
    if ( pvLocation >= 0 ) pv = vertices->at( pvLocation );
  }

  for( auto jet_itr : *jets ) {
    this->FillJet(jet_itr, pv, pvLocation, jetName);
  }

}



void HelpTreeBase::FillJet( const xAOD::Jet* jet_itr, const xAOD::Vertex* pv, int pvLocation, const std::string& jetName ) {

  xAH::JetContainer* thisJet = m_jets[jetName];

  thisJet->FillJet(jet_itr, pv, pvLocation);

  this->FillJetsUser(jet_itr, jetName);

  return;
}

void HelpTreeBase::ClearJets(const std::string& jetName) {

  xAH::JetContainer* thisJet = m_jets[jetName];
  thisJet->clear();

  this->ClearJetsUser(jetName);

}

/*********************
 *
 *   TRUTH
 *
 ********************/

void HelpTreeBase::AddTruthParts(const std::string& detailStr, const std::string& truthName)
{

  if(m_debug) Info("AddTruthParts()", "Adding truth particle %s with variables: %s", truthName.c_str(), detailStr.c_str());
  m_truth[truthName] = new xAH::TruthContainer(truthName, detailStr, m_units);

  xAH::TruthContainer* thisTruth = m_truth[truthName];
  thisTruth->setBranches(m_tree);
  this->AddTruthUser(truthName, detailStr);
}

void HelpTreeBase::FillTruth( const xAOD::TruthParticleContainer* truthParts, const std::string& truthName  ) {

  this->ClearTruth(truthName);

  // We need some basic cuts here to avoid many PseudoRapiditity warnings being thrown ...
  float truthparticle_ptmin  = 2.0;
  float truthparticle_etamax = 8.0;

  for( auto truth_itr : *truthParts ) {

    if((truth_itr->pt() / m_units < truthparticle_ptmin) || (fabs(truth_itr->eta()) > truthparticle_etamax) ){
      continue;
    }

    this->FillTruth(truth_itr, truthName);
  }

}

void HelpTreeBase::FillTruth( const xAOD::TruthParticle* truthPart, const std::string& truthName )
{
  xAH::TruthContainer* thisTruth = m_truth[truthName];

  thisTruth->FillTruth(truthPart);

  this->FillTruthUser(truthPart, truthName);

  return;
}

void HelpTreeBase::ClearTruth(const std::string& truthName) {

  xAH::TruthContainer* thisTruth = m_truth[truthName];
  thisTruth->clear();

  this->ClearTruthUser(truthName);

}

/*********************
 *
 *   TRACKS
 *
 ********************/

void HelpTreeBase::AddTrackParts(const std::string& detailStr, const std::string& trackName)
{
  if(m_debug) Info("AddTrackParts()", "Adding track particle %s with variables: %s", trackName.c_str(), detailStr.c_str());
  m_tracks[trackName] = new xAH::TrackContainer(trackName, detailStr, m_units);

  xAH::TrackContainer* thisTrack = m_tracks[trackName];
  thisTrack->setBranches(m_tree);
  this->AddTracksUser(trackName, detailStr);
}

void HelpTreeBase::FillTracks( const xAOD::TrackParticleContainer* trackParts, const std::string& trackName  ) {

  this->ClearTracks(trackName);

  // We need some basic cuts here to avoid many PseudoRapiditity warnings being thrown ...
  // float trackparticle_ptmin  = 1.0;
  // float trackparticle_etamax = 8.0;

  for( auto track_itr : *trackParts ) {

    // if((track_itr->pt() / m_units < trackparticle_ptmin) || (fabs(track_itr->eta()) > trackparticle_etamax) ){
    //  continue;
    // }

    this->FillTrack(track_itr, trackName);
  }

}

void HelpTreeBase::FillTrack( const xAOD::TrackParticle* trackPart, const std::string& trackName )
{
  xAH::TrackContainer* thisTrack = m_tracks[trackName];

  thisTrack->FillTrack(trackPart);

  this->FillTracksUser(trackPart, trackName);

  return;
}

void HelpTreeBase::ClearTracks(const std::string& trackName) {

  xAH::TrackContainer* thisTrack = m_tracks[trackName];
  thisTrack->clear();

  this->ClearTracksUser(trackName);

}

/*********************
 *
 *   FAT JETS
 *
 ********************/

// make a unique container:suffix key to lookup the branches in the maps
std::string HelpTreeBase::FatJetCollectionName(const std::string& fatjetName,
					       const std::string& suffix) {
  return suffix.empty() ? fatjetName : (fatjetName + ":" + suffix);
}

void HelpTreeBase::AddFatJets(const std::string& detailStr, const std::string& fatjetName,
			      const std::string& subjetDetailStr,
			      const std::string& suffix) {

  if(m_debug) Info("AddFatJets()", "Adding fat jet variables: %s", detailStr.c_str());

  const std::string& collectionName = FatJetCollectionName(fatjetName, suffix);
  m_fatjets[collectionName] = new xAH::FatJetContainer(fatjetName, detailStr, subjetDetailStr, suffix, m_units, m_isMC);

  xAH::FatJetContainer* thisFatJet = m_fatjets[collectionName];
  thisFatJet->setBranches(m_tree);

  this->AddFatJetsUser(detailStr, fatjetName, suffix);
}

void HelpTreeBase::AddTruthFatJets(const std::string& detailStr, const std::string& truthFatJetName) {

  if(m_debug) Info("AddTruthFatJets()", "Adding fat jet variables: %s", detailStr.c_str());

  m_truth_fatjets[truthFatJetName] = new xAH::FatJetContainer(truthFatJetName, detailStr, "", "", m_units, m_isMC);

  xAH::FatJetContainer* thisTruthFatJet = m_truth_fatjets[truthFatJetName];
  thisTruthFatJet->setBranches(m_tree);

  this->AddTruthFatJetsUser(detailStr, truthFatJetName);
}


void HelpTreeBase::FillFatJets( const xAOD::JetContainer* fatJets , int pvLocation, const std::string& fatjetName, const std::string& suffix ) {

  this->ClearFatJets(fatjetName, suffix);

  for( auto fatjet_itr : *fatJets ) {

    this->FillFatJet(fatjet_itr, pvLocation, fatjetName, suffix);

  } // loop over fat jets

}

void HelpTreeBase::FillFatJet( const xAOD::Jet* fatjet_itr, int pvLocation, const std::string& fatjetName, const std::string& suffix ) {

  const std::string& collectionName = FatJetCollectionName(fatjetName, suffix);
  xAH::FatJetContainer* thisFatJet = m_fatjets[collectionName];

  thisFatJet->FillFatJet(fatjet_itr, pvLocation);

  this->FillFatJetsUser(fatjet_itr, pvLocation, fatjetName, suffix);

  return;
}


void HelpTreeBase::FillTruthFatJets( const xAOD::JetContainer* truthTruthFatJets, int pvLocation, const std::string& truthFatJetName ) {
  this->ClearTruthFatJets(truthFatJetName);

  for( auto truth_fatjet_itr : *truthTruthFatJets ) {

    this->FillTruthFatJet(truth_fatjet_itr, pvLocation, truthFatJetName);

  } // loop over truth fat jets

}

void HelpTreeBase::FillTruthFatJet( const xAOD::Jet* truth_fatjet_itr, int pvLocation, const std::string& truthFatJetName ) {

  xAH::FatJetContainer* thisTruthFatJet = m_truth_fatjets[truthFatJetName];

  thisTruthFatJet->FillFatJet(truth_fatjet_itr, pvLocation);

  this->FillTruthFatJetsUser(truth_fatjet_itr, pvLocation, truthFatJetName);

  return;
}


void HelpTreeBase::ClearFatJets(const std::string& fatjetName, const std::string& suffix) {
  const std::string& collectionName = FatJetCollectionName(fatjetName, suffix);

  xAH::FatJetContainer* thisFatJet = m_fatjets[collectionName];
  thisFatJet->clear();

  this->ClearFatJetsUser(fatjetName, suffix);
}

void HelpTreeBase::ClearTruthFatJets(const std::string& truthFatJetName) {

  xAH::FatJetContainer* thisTruthFatJet = m_truth_fatjets[truthFatJetName];
  thisTruthFatJet->clear();

  this->ClearTruthFatJetsUser(truthFatJetName);
}

void HelpTreeBase::ClearEvent() {
  m_eventInfo->clear();
  this->ClearEventUser();
}


/*********************
 *
 *   TAUS
 *
 ********************/

void HelpTreeBase::AddTaus(const std::string& detailStr, const std::string& tauName) {

  if ( m_debug )  Info("AddTaus()", "Adding tau variables: %s", detailStr.c_str());

  m_taus[tauName] = new xAH::TauContainer(tauName, detailStr, m_units, m_isMC, m_nominalTree);

  xAH::TauContainer* thisTau = m_taus[tauName];

  thisTau->setBranches(m_tree);
  this->AddTausUser(detailStr, tauName);
}

void HelpTreeBase::FillTaus( const xAOD::TauJetContainer* taus, const std::string& tauName ) {

  this->ClearTaus(tauName);

  for( auto tau_itr : *taus ) {
    this->FillTau(tau_itr, tauName);
  }
}

void HelpTreeBase::FillTau( const xAOD::TauJet* tau, const std::string& tauName ) {

  xAH::TauContainer* thisTau = m_taus[tauName];

  thisTau->FillTau(tau);

  this->FillTausUser(tau, tauName);
}

void HelpTreeBase::ClearTaus(const std::string& tauName) {

  xAH::TauContainer* thisTau = m_taus[tauName];

  thisTau->clear();

  this->ClearTausUser(tauName);

}



/*********************
 *
 *     MET
 *
 ********************/
void HelpTreeBase::AddMET( const std::string& detailStr, const std::string& metName ) {

  if(m_debug) Info("AddMET()", "Adding MET variables: %s", detailStr.c_str());

  m_met[metName] = new xAH::MetContainer(metName, detailStr, m_units);

  xAH::MetContainer* thisMet = m_met[metName];

  thisMet->setBranches(m_tree);
  this->AddMETUser(detailStr, metName);
}

void HelpTreeBase::FillMET( const xAOD::MissingETContainer* met, const std::string& metName ) {

  // Clear previous events
  this->ClearMET(metName);

  xAH::MetContainer* thisMet = m_met[metName];

  thisMet->FillMET(met);

  this->FillMETUser(met, metName);
}

void HelpTreeBase::ClearMET( const std::string& metName ) {
  xAH::MetContainer* thisMet = m_met[metName];

  thisMet->clear();

  this->ClearMETUser(metName);
}


/*********************
 *
 *     MET
 *
 ********************/
void HelpTreeBase::AddTrigMET( const std::string& detailStr, const std::string& metName ) {

  if(m_debug) Info("AddTrigMET()", "Adding Trig MET variables: %s", detailStr.c_str());

  m_trigmet[metName] = new xAH::TrigMetContainer(metName, detailStr, m_units);

  xAH::TrigMetContainer* thisMet = m_trigmet[metName];

  thisMet->setBranches(m_tree);
  this->AddTrigMETUser(detailStr, metName);
}

void HelpTreeBase::FillTrigMET( const xAOD::TrigMissingETContainer* met, const std::string& metName ) {

  // Clear previous events
  this->ClearTrigMET(metName);

  xAH::TrigMetContainer* thisMet = m_trigmet[metName];

  thisMet->FillTrigMET(met);

  this->FillTrigMETUser(met, metName);
}

void HelpTreeBase::ClearTrigMET( const std::string& metName ) {
  xAH::TrigMetContainer* thisMet = m_trigmet[metName];

  thisMet->clear();

  this->ClearTrigMETUser(metName);
}


bool HelpTreeBase::writeTo( TFile* file ) {
  file->cd(); // necessary?
  int status( m_tree->Write() );
  if ( status == 0 ) { return false; }
  return true;
}

/*********************
 *
 *   VERTICES
 *
 ********************/

void HelpTreeBase::AddVertices( const std::string& detailStr, const std::string& vertexName )
{

  if(m_debug) Info("AddVertices()", "Adding %s vertices", vertexName.c_str());

  m_vertices[vertexName] = new xAH::VertexContainer(detailStr, vertexName);
  xAH::VertexContainer* thisVertex = m_vertices[vertexName];
  thisVertex->setBranches(m_tree);

}

void HelpTreeBase::FillVertices( const xAOD::VertexContainer* vertices, const std::string& vertexName ) {

  this->ClearVertices(vertexName);

  xAH::VertexContainer* thisVertex = m_vertices[vertexName];

  thisVertex->FillVertices(vertices);

}

void HelpTreeBase::ClearVertices( const std::string& vertexName )
{

  xAH::VertexContainer* thisVertex = m_vertices[vertexName];
  thisVertex->clear();

}

void HelpTreeBase::AddTruthVertices( const std::string& detailStr, const std::string& truthVertexName )
{

  if(m_debug) Info("AddTruthVertices()", "Adding %s vertices", truthVertexName.c_str());

  m_truth_vertices[truthVertexName] = new xAH::VertexContainer(detailStr, truthVertexName);
  xAH::VertexContainer* thisTruthVertex = m_truth_vertices[truthVertexName];
  thisTruthVertex->setBranches(m_tree);

}

void HelpTreeBase::FillTruthVertices( const xAOD::TruthVertexContainer* truthVertices, const std::string& truthVertexName ) {

  this->ClearTruthVertices(truthVertexName);

  xAH::VertexContainer* thisTruthVertex = m_truth_vertices[truthVertexName];

  thisTruthVertex->FillTruthVertices(truthVertices);

}

void HelpTreeBase::ClearTruthVertices( const std::string& truthVertexName )
{

  xAH::VertexContainer* thisTruthVertex = m_truth_vertices[truthVertexName];
  thisTruthVertex->clear();

}

