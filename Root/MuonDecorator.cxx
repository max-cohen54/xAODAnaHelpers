#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include "xAODMuon/MuonContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODAnaHelpers/HelperFunctions.h"
#include <xAODAnaHelpers/MuonDecorator.h>

// this is needed to distribute the algorithm to the workers
ClassImp(MuonDecorator)

MuonDecorator :: MuonDecorator () :
    Algorithm("MuonDecorator")
{
}

EL::StatusCode MuonDecorator :: setupJob (EL::Job& job)
{
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode MuonDecorator :: histInitialize ()
{

  ANA_MSG_INFO( m_name );
  ANA_CHECK( xAH::Algorithm::algInitialize());

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode MuonDecorator :: fileExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode MuonDecorator :: changeInput (bool /*firstFile*/) { return EL::StatusCode::SUCCESS; }

EL::StatusCode MuonDecorator :: initialize ()
{
  ANA_MSG_INFO( "MuonDecorator");
  m_event = wk()->xaodEvent();
  m_store = wk()->xaodStore();
  // needed here and not in initalize since this is called first
  if( m_inContainerName.empty() || m_detailStr.empty() ){
    ANA_MSG_WARNING( "One or more required configuration values are empty");
  }
  ANA_CHECK( m_muonSelectionTool_handle.setProperty( "IsRun3Geo", true ));
  ANA_CHECK( m_muonSelectionTool_handle.setProperty( "TurnOffMomCorr", true ));
  ANA_CHECK( m_muonSelectionTool_handle.setProperty( "OutputLevel", msg().level() ));
  ANA_CHECK( m_muonSelectionTool_handle.retrieve());
  ANA_MSG_DEBUG("Retrieved tool: " << m_muonSelectionTool_handle);

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode MuonDecorator :: execute ()
{
  const xAOD::MuonContainer* Muons(nullptr);
  if (m_inContainerName.empty()) {
    ANA_CHECK( HelperFunctions::retrieve(Muons, m_inContainerName, m_event, m_store, msg()) );
  }
  // quality decorators
  static SG::AuxElement::Decorator< char > isVeryLoose("VeryLoose");
  static SG::AuxElement::Decorator< char > isLoose("Loose");
  static SG::AuxElement::Decorator< char > isMedium("Medium");
  static SG::AuxElement::Decorator< char > isTight("Tight");
  // TODO:: Find out if any ID cuts are applied in the online muon selection
  // static SG::AuxElement::Decorator< char > passIDcuts("passIDcuts");

  for (const auto& muon : *Muons) {    
    int this_quality = static_cast<int>( m_muonSelectionTool_handle->getQuality( *muon ) );
    isVeryLoose( *muon ) = ( this_quality <= static_cast<int>(xAOD::Muon::VeryLoose) ) ? 1 : 0;
    isLoose( *muon )     = ( this_quality <= static_cast<int>(xAOD::Muon::Loose) )     ? 1 : 0;
    isMedium( *muon )    = ( this_quality <= static_cast<int>(xAOD::Muon::Medium) )    ? 1 : 0;
    isTight( *muon )     = ( this_quality <= static_cast<int>(xAOD::Muon::Tight) )     ? 1 : 0;
    std::cout << "Muon " << muon->index()
      << " passes VeryLoose: " << isVeryLoose(*muon)
      << ", Loose: " << isLoose(*muon)
      << ", Medium: " << isMedium(*muon)
      << ", Tight: " << isTight(*muon) 
      << std::endl;
  }

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode MuonDecorator :: postExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode MuonDecorator :: finalize () { return EL::StatusCode::SUCCESS; }
EL::StatusCode MuonDecorator :: histFinalize ()
{
  // clean up memory
  ANA_CHECK( xAH::Algorithm::algFinalize());
  return EL::StatusCode::SUCCESS;
}
