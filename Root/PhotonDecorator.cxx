#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODAnaHelpers/HelperFunctions.h"
#include <xAODAnaHelpers/PhotonDecorator.h>
#include "ElectronPhotonSelectorTools/egammaPIDdefs.h"

// this is needed to distribute the algorithm to the workers
ClassImp(PhotonDecorator)

PhotonDecorator :: PhotonDecorator () :
    Algorithm("PhotonDecorator")
{
}

EL::StatusCode PhotonDecorator :: setupJob (EL::Job& job)
{
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PhotonDecorator :: histInitialize ()
{

  ANA_MSG_INFO( m_name );
  ANA_CHECK( xAH::Algorithm::algInitialize());

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PhotonDecorator :: fileExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode PhotonDecorator :: changeInput (bool /*firstFile*/) { return EL::StatusCode::SUCCESS; }

EL::StatusCode PhotonDecorator :: initialize ()
{
  ANA_MSG_INFO( "PhotonDecorator");
  m_event = wk()->xaodEvent();
  m_store = wk()->xaodStore();
  // needed here and not in initalize since this is called first
  if( m_inContainerName.empty() || m_detailStr.empty() ){
    ANA_MSG_WARNING( "One or more required configuration values are empty");
  }

  if (m_photonLooseIsEMSelector.empty()) {
    m_photonLooseIsEMSelector.setTypeAndName("AsgPhotonIsEMSelector/PhotonLooseIsEMSelector");
    ANA_CHECK( m_photonLooseIsEMSelector.setProperty("isEMMask", egammaPID::PhotonLoose));
    ANA_CHECK( m_photonLooseIsEMSelector.setProperty("ConfigFile", "ElectronPhotonSelectorTools/trigger/rel22_20210611/PhotonIsEMLooseSelectorCutDefs.conf"));
    ANA_CHECK( m_photonLooseIsEMSelector.setProperty("skipAmbiguityCut", true));
  }
  ANA_CHECK( m_photonLooseIsEMSelector.retrieve() );
  ANA_MSG_DEBUG("Retrieved tool: " << m_photonLooseIsEMSelector);
  if (m_photonMediumIsEMSelector.empty()) {
    m_photonMediumIsEMSelector.setTypeAndName("AsgPhotonIsEMSelector/PhotonMediumIsEMSelector");
    ANA_CHECK( m_photonMediumIsEMSelector.setProperty("isEMMask", egammaPID::PhotonMedium));
    ANA_CHECK( m_photonMediumIsEMSelector.setProperty("ConfigFile", "ElectronPhotonSelectorTools/trigger/rel22_20210611/PhotonIsEMMediumSelectorCutDefs.conf"));
    ANA_CHECK( m_photonMediumIsEMSelector.setProperty("skipAmbiguityCut", true));
  }
  ANA_CHECK( m_photonMediumIsEMSelector.retrieve() );
  ANA_MSG_DEBUG("Retrieved tool: " << m_photonMediumIsEMSelector);
  if (m_photonTightIsEMSelector.empty()) {
    m_photonTightIsEMSelector.setTypeAndName("AsgPhotonIsEMSelector/PhotonTightIsEMSelector");
    ANA_CHECK( m_photonTightIsEMSelector.setProperty("isEMMask", egammaPID::PhotonTight));
    ANA_CHECK( m_photonTightIsEMSelector.setProperty("ConfigFile", "ElectronPhotonSelectorTools/trigger/rel22_20210611/PhotonIsEMTightSelectorCutDefs.conf"));
    ANA_CHECK( m_photonTightIsEMSelector.setProperty("skipAmbiguityCut", true));
  }
  ANA_CHECK( m_photonTightIsEMSelector.retrieve() );
  ANA_MSG_DEBUG("Retrieved tool: " << m_photonTightIsEMSelector);

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PhotonDecorator :: execute ()
{
  const xAOD::PhotonContainer* photons(nullptr);
  if (!m_inContainerName.empty()) {
    ANA_CHECK( HelperFunctions::retrieve(photons, m_inContainerName, m_event, m_store, msg()) );
  }
  for (const auto& photon : *photons) {
    // quality decorators
    SG::AuxElement::Decorator< bool > isLoose("PhotonID_Loose");
    SG::AuxElement::Decorator< bool > isMedium("PhotonID_Medium");
    SG::AuxElement::Decorator< bool > isTight("PhotonID_Tight");

    isLoose(*photon)  = bool(m_photonLooseIsEMSelector->accept(photon));
    isMedium(*photon) = bool(m_photonMediumIsEMSelector->accept(photon));
    isTight(*photon)  = bool(m_photonTightIsEMSelector->accept(photon));

    ANA_MSG_DEBUG("Photon " << photon->index()
      << " passes Loose: " << isLoose(*photon)
      << ", Medium: " << isMedium(*photon)
      << ", Tight: " << isTight(*photon)
    );
  }
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode PhotonDecorator :: postExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode PhotonDecorator :: finalize () { return EL::StatusCode::SUCCESS; }
EL::StatusCode PhotonDecorator :: histFinalize ()
{
  // clean up memory
  ANA_CHECK( xAH::Algorithm::algFinalize());
  return EL::StatusCode::SUCCESS;
}
