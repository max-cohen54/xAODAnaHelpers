/********************************************************
 *
 * Trig Met Histogramming
 *
 * D. Rankin (dylan.sheldon.rankin@cern.ch)
 *
 *******************************************************/


#include <xAODAnaHelpers/TrigMetHists.h>
#include <sstream>

#include <AsgMessaging/MessageCheck.h>

#include "TMath.h"

using std::vector;

TrigMetHists :: TrigMetHists (std::string name, std::string detailStr) :
  HistogramManager(name, detailStr),
  m_infoSwitch(new HelperClasses::METInfoSwitch(m_detailStr))
{
  m_debug = false;
}

TrigMetHists :: ~TrigMetHists () {
  if(m_infoSwitch) delete m_infoSwitch;
}

StatusCode TrigMetHists::initialize() {

  m_met      = book(m_name,  "met",      "met",      100,     0,    200);
  m_metPx    = book(m_name,  "metPx",    "metPx",    100,  -200,    200);
  m_metPy    = book(m_name,  "metPy",    "metPy",    100,  -200,    200);
  m_metSumEt = book(m_name,  "metSumEt", "metSumEt", 100,     0,   2000);
  m_metPhi   = book(m_name,  "metPhi",   "metPhi",   100,    -3.2,    3.2);

  return StatusCode::SUCCESS;
}

StatusCode TrigMetHists::execute( const xAOD::TrigMissingETContainer* met, float eventWeight ) {

  if(m_debug) std::cout << "TrigMetHists: in execute " <<std::endl;

  if (met->size()>0) {
    const xAOD::TrigMissingET* trig_met = met->front();
    m_met      -> Fill( TMath::Sqrt(trig_met->ex()*trig_met->ex()+trig_met->ey()*trig_met->ey())   / 1e3, eventWeight);
    m_metPx    -> Fill( trig_met->ex()   / 1e3, eventWeight);
    m_metPy    -> Fill( trig_met->ey()   / 1e3, eventWeight);
    m_metSumEt -> Fill( trig_met->sumEt() / 1e3, eventWeight);
    m_metPhi   -> Fill( TMath::ATan(trig_met->ey()/trig_met->ex())        , eventWeight);
  }

  return StatusCode::SUCCESS;
}

