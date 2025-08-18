#adapted from: https://xaodanahelpers.readthedocs.io/en/latest/UsingUs.html

from xAODAnaHelpers import Config
c = Config()

c.algorithm("BasicEventSelection", {"m_truthLevelOnly": False,
                                    #"m_applyGRLCut": True,
                                    #"m_GRLxml": "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data24_13p6TeV/20241020/data24_13p6TeV.periodsEtoN_DetStatus-v128-pro36-07_MERGED_PHYS_StandardGRL_All_Good_25ns.xml",
                                    "m_doPUreweighting": False,
                                    "m_vertexContainerName": "PrimaryVertices",
                                    "m_PVNTrack": 2,
                                    "m_useMetaData": False,
                                    "m_triggerSelection": "L1_.*|HLT_.*",
                                    "m_applyTriggerCut": True,
                                    "m_storeTrigDecisions": True,
                                    "m_storePassL1": True,
                                    "m_storePassHLT": True,
                                    "m_name": "myBaseEventSel"})

c.algorithm("ElectronDecorator", {
                              "m_name"                      : "ElectronDecor",
                              #----------------------- Container Flow ----------------------------#
                              "m_inContainerName"           : "HLT_egamma_Electrons_GSF",
                              "m_detailStr"                 : "",
                           }
)

# c.algorithm("MuonDecorator", {
#                               "m_name"                      : "MuonDecor",
#                               #----------------------- Container Flow ----------------------------#
#                               "m_inContainerName"           : "HLT_Muons_FS",
#                               "m_detailStr"                 : "",
#                            }
# )

c.algorithm("PhotonDecorator", {
                              "m_name"                      : "PhotonDecor",
                              #----------------------- Container Flow ----------------------------#
                              "m_inContainerName"           : "HLT_egamma_Photons",
                              "m_detailStr"                 : "",
                           }
)

c.algorithm("TreeAlgo", {
                         "m_debug": True,
                         "m_name": "EB_Tree",
                         "m_jetContainerName": "HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftf",
                         "m_jetBranchName": "HLT_jet",
                         "m_jetDetailStr": "kinematic clean timing energy",
                         "m_photonContainerName": "HLT_egamma_Photons HLT_egamma_Iso_Photons",
                         "m_photonBranchName": "HLT_ph HLT_iso_ph",
                         "m_photonDetailStr": "kinematic PID ISOL_FCLoose ISOL_FCTight",
                         "m_elContainerName": "HLT_egamma_Electrons_GSF",
                         "m_elBranchName": "HLT_el",
                         "m_elDetailStr": "kinematic PID PID_LHVeryLoose PID_LHLoose PID_LHMedium PID_LHTight",
                         "m_muContainerName": "HLT_Muons_FS",
                         "m_muBranchName": "HLT_muon",
                         "m_muDetailStr": "kinematic quality RECO_VeryLoose RECO_Loose RECO_Medium RECO_Tight ",
			                "m_l1JetContainerName": "L1_jFexSRJetRoI L1_jFexLRJetRoI",
                         "m_l1JetBranchName": "L1_jFexSRJet L1_jFexLRJet",
			                "m_TrigMETContainerName": "HLT_MET_nn HLT_MET_cell HLT_MET_tcpufit HLT_MET_pfopufit HLT_MET_mhtpufit_pf HLT_MET_mhtpufit_pf_subjesgscIS",
                         "m_TrigMETBranchName": "HLT_MET_nn HLT_MET_cell HLT_MET_tcpufit HLT_MET_pfopufit HLT_MET_mhtpufit_pf HLT_MET_mhtpufit_pf_subjesgscIS",
                         "m_TrigMETDetailStr": "kinematic",
                         "m_trigDetailStr": "basic passTriggers",
                         "m_l1MuonContainerName": "LVL1MuonRoIs",
                         "m_l1MuonBranchName": "L1Muon",
                         "m_l1TauContainerName": "L1_jFexTauRoI L1_eTauRoI",
                         "m_l1TauBranchName": "L1Tau_jFex L1Tau_eFex",
                         "m_l1EMContainerName": "L1_eEMRoI",
                         "m_l1EMBranchName": "L1Egamma",
                         "m_l1MetContainerName": "L1_jFexMETRoI",
                         "m_l1MetDetailStr": "kinematic",
                         "m_l1MetBranchName": "L1MET",
			                "m_evtDetailStr": "pileup"
                         })


