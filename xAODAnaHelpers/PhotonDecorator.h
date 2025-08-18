#ifndef xAODAnaHelpers_PhotonDecorator_H
#define xAODAnaHelpers_PhotonDecorator_H

// algorithm wrapper
#include "xAODAnaHelpers/Algorithm.h"
#include "ElectronPhotonSelectorTools/AsgPhotonIsEMSelector.h"

class PhotonDecorator : public xAH::Algorithm
{
  // put your configuration variables here as public variables.
  // that way they can be set directly from CINT and python.
public:
  std::string m_inContainerName = "";

  // configuration variables
  std::string m_detailStr = "";

private:

  // variables that don't get filled at submission time should be
  // protected from being send from the submission node to the worker
  // node (done by the //!)
  asg::AnaToolHandle<AsgPhotonIsEMSelector> m_photonLooseIsEMSelector{}; //!!
  asg::AnaToolHandle<AsgPhotonIsEMSelector> m_photonMediumIsEMSelector{}; //!!
  asg::AnaToolHandle<AsgPhotonIsEMSelector> m_photonTightIsEMSelector{}; //!!
public:
  // Tree *myTree; //!
  // TH1 *myHist; //!

  // this is a standard constructor
  PhotonDecorator ();

  // these are the functions inherited from Algorithm
  virtual EL::StatusCode setupJob (EL::Job& job);
  virtual EL::StatusCode fileExecute ();
  virtual EL::StatusCode histInitialize ();
  virtual EL::StatusCode changeInput (bool firstFile);
  virtual EL::StatusCode initialize ();
  virtual EL::StatusCode execute ();
  virtual EL::StatusCode postExecute ();
  virtual EL::StatusCode finalize ();
  virtual EL::StatusCode histFinalize ();

  /// @cond
  // this is needed to distribute the algorithm to the workers
  ClassDef(PhotonDecorator, 1);
  /// @endcond

};

#endif
