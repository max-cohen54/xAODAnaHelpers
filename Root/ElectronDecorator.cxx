#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODAnaHelpers/HelperFunctions.h"
#include <xAODAnaHelpers/ElectronDecorator.h>

// this is needed to distribute the algorithm to the workers
ClassImp(ElectronDecorator)

ElectronDecorator :: ElectronDecorator () :
    Algorithm("ElectronDecorator")
{
}

EL::StatusCode ElectronDecorator :: setupJob (EL::Job& job)
{
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode ElectronDecorator :: histInitialize ()
{

  ANA_MSG_INFO( m_name );
  ANA_CHECK( xAH::Algorithm::algInitialize());

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode ElectronDecorator :: fileExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode ElectronDecorator :: changeInput (bool /*firstFile*/) { return EL::StatusCode::SUCCESS; }

EL::StatusCode ElectronDecorator :: initialize ()
{
  ANA_MSG_INFO( "ElectronDecorator");
  m_event = wk()->xaodEvent();
  m_store = wk()->xaodStore();
  // needed here and not in initalize since this is called first
  if( m_inContainerName.empty() || m_detailStr.empty() ){
    ANA_MSG_WARNING( "One or more required configuration values are empty");
  }
  if (m_electronLLHToolVeryLoose.empty()) {
    m_electronLLHToolVeryLoose.setTypeAndName("AsgElectronLikelihoodTool/ElectronLikelihoodToolVeryLoose");
    ANA_CHECK( m_electronLLHToolVeryLoose.setProperty( "ConfigFile", "ElectronPhotonSelectorTools/trigger/rel22_20210611/ElectronLikelihoodVeryLooseTriggerConfig.conf" ));
  }
  ANA_CHECK( m_electronLLHToolVeryLoose.retrieve() );
  ANA_MSG_DEBUG("Retrieved tool: " << m_electronLLHToolVeryLoose);
  if (m_electronLLHToolLoose.empty()) {
    m_electronLLHToolLoose.setTypeAndName("AsgElectronLikelihoodTool/ElectronLikelihoodToolLoose");
    ANA_CHECK( m_electronLLHToolLoose.setProperty( "ConfigFile", "ElectronPhotonSelectorTools/trigger/rel22_20210611/ElectronLikelihoodLooseTriggerConfig.conf" ));
  }
  ANA_CHECK( m_electronLLHToolLoose.retrieve() );
  ANA_MSG_DEBUG("Retrieved tool: " << m_electronLLHToolLoose);
  if (m_electronLLHToolMedium.empty()) {
    m_electronLLHToolMedium.setTypeAndName("AsgElectronLikelihoodTool/ElectronLikelihoodToolMedium");
    ANA_CHECK( m_electronLLHToolMedium.setProperty( "ConfigFile", "ElectronPhotonSelectorTools/trigger/rel22_20210611/ElectronLikelihoodMediumTriggerConfig.conf" ));
  }
  ANA_CHECK( m_electronLLHToolMedium.retrieve() );
  ANA_MSG_DEBUG("Retrieved tool: " << m_electronLLHToolMedium);
  if (m_electronLLHToolTight.empty()) {
    m_electronLLHToolTight.setTypeAndName("AsgElectronLikelihoodTool/ElectronLikelihoodToolTight");
    ANA_CHECK( m_electronLLHToolTight.setProperty( "ConfigFile", "ElectronPhotonSelectorTools/trigger/rel22_20210611/ElectronLikelihoodTightTriggerConfig.conf" ));
  }
  ANA_CHECK( m_electronLLHToolTight.retrieve() );
  ANA_MSG_DEBUG("Retrieved tool: " << m_electronLLHToolTight);
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode ElectronDecorator :: execute ()
{
   const xAOD::ElectronContainer* electrons(nullptr);
  if (!m_inContainerName.empty()) {
    ANA_CHECK( HelperFunctions::retrieve(electrons, m_inContainerName, m_event, m_store, msg()) );
  }
  static SG::AuxElement::Decorator< char > isLHVeryLoose ("LHVeryLoose");
  static SG::AuxElement::Decorator< char > isLHLoose ("LHLoose");
  static SG::AuxElement::Decorator< char > isLHMedium ("LHMedium");
  static SG::AuxElement::Decorator< char > isLHTight ("LHTight");
  for (const auto& el : *electrons) {
    bool passLHVeryLoose = bool(m_electronLLHToolVeryLoose->accept(el));
    isLHVeryLoose(*el) = passLHVeryLoose ? 1 : 0;
    bool passLHLoose = bool(m_electronLLHToolLoose->accept(el));
    isLHLoose(*el) = passLHLoose ? 1 : 0;
    bool passLHMedium = bool(m_electronLLHToolMedium->accept(el));
    isLHMedium(*el) = passLHMedium ? 1 : 0;
    bool passLHTight = bool(m_electronLLHToolTight->accept(el));
    isLHTight(*el) = passLHTight ? 1 : 0;
    ANA_MSG_DEBUG("Electron " << el->index()
      << " passes LHVeryLoose: " << passLHVeryLoose
      << ", LHLoose: " << passLHLoose 
      << ", LHMedium: " << passLHMedium
      << ", LHTight: " << passLHTight 
    );
  }

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode ElectronDecorator :: postExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode ElectronDecorator :: finalize () { return EL::StatusCode::SUCCESS; }
EL::StatusCode ElectronDecorator :: histFinalize ()
{
  // clean up memory
  ANA_CHECK( xAH::Algorithm::algFinalize());
  return EL::StatusCode::SUCCESS;
}
