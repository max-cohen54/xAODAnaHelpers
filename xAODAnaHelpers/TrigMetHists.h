#ifndef xAODAnaHelpers_TrigMetHists_H
#define xAODAnaHelpers_TrigMetHists_H

#include "xAODAnaHelpers/HistogramManager.h"
#include "xAODAnaHelpers/HelperClasses.h"
#include "xAODTrigMissingET/TrigMissingETContainer.h"

class TrigMetHists : public HistogramManager
{
  public:


    TrigMetHists(std::string name, std::string detailStr);
    virtual ~TrigMetHists() ;

    bool m_debug;
    StatusCode initialize();
    StatusCode execute( const xAOD::TrigMissingETContainer* met, float eventWeight );

    using HistogramManager::book; // make other overloaded version of book() to show up in subclass
    using HistogramManager::execute; // overload

  protected:

    // holds bools that control which histograms are filled
    HelperClasses::METInfoSwitch* m_infoSwitch;

  private:

    TH1F*   m_met      ; //!
    TH1F*   m_metPx    ; //!
    TH1F*   m_metPy    ; //!
    TH1F*   m_metSumEt ; //!
    TH1F*   m_metPhi   ; //!

};

#endif
