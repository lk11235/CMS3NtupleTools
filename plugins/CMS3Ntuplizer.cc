#include <CommonTools/UtilAlgos/interface/TFileService.h>
#include <CMS3/NtupleMaker/interface/plugins/CMS3Ntuplizer.h>
#include "MELAStreamHelpers.hh"


using namespace std;
using namespace MELAStreamHelpers;
using namespace edm;


CMS3Ntuplizer::CMS3Ntuplizer(const edm::ParameterSet& pset_) :
  pset(pset_),
  outtree(nullptr),
  firstEvent(true),

  year(pset.getParameter<int>("year")),
  treename(pset.getUntrackedParameter<std::string>("treename")),
  isMC(pset.getParameter<bool>("isMC"))
{
  if (year!=2016 && year!=2017 && year!=2018) throw cms::Exception("CMS3Ntuplizer::CMS3Ntuplizer: Year is undefined!");

  electronsToken  = consumes< edm::View<pat::Electron> >(pset.getParameter<edm::InputTag>("electronSrc"));
  photonsToken  = consumes< edm::View<pat::Photon> >(pset.getParameter<edm::InputTag>("photonSrc"));
  muonsToken  = consumes< edm::View<pat::Muon> >(pset.getParameter<edm::InputTag>("muonSrc"));

  genInfoToken = consumes<GenInfo>(pset.getParameter<edm::InputTag>("genInfoSrc"));

}
CMS3Ntuplizer::~CMS3Ntuplizer(){
  //clearMELABranches(); // Cleans LHE branches
  //delete pileUpReweight;
  //delete metCorrHandler;
}


void CMS3Ntuplizer::beginJob(){
  edm::Service<TFileService> fs;
  TTree* tout = fs->make<TTree>(treename, "Selected event summary");
  outtree = new BaseTree(nullptr, tout, nullptr, nullptr, false);
  outtree->setAcquireTreePossession(false);
  //buildMELABranches();
}
void CMS3Ntuplizer::endJob(){
  delete outtree;
}

void CMS3Ntuplizer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&){}
void CMS3Ntuplizer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&){}

void CMS3Ntuplizer::beginRun(edm::Run const&, edm::EventSetup const&){}
void CMS3Ntuplizer::endRun(edm::Run const&, edm::EventSetup const&){}

void CMS3Ntuplizer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){
  /*****************/
  /* Input handles */
  /*****************/

  // Electrons
  edm::Handle< edm::View<pat::Electron> > electronsHandle;
  iEvent.getByToken(electronsToken, electronsHandle);
  if (!electronsHandle.isValid()) throw cms::Exception("CMS3Ntuplizer::analyze: Error getting the electron collection from the event...");

  // Photons
  edm::Handle< edm::View<pat::Photon> > photonsHandle;
  iEvent.getByToken(photonsToken, photonsHandle);
  if (!photonsHandle.isValid()) throw cms::Exception("CMS3Ntuplizer::analyze: Error getting the photon collection from the event...");

  // Muons
  edm::Handle< edm::View<pat::Muon> > muonsHandle;
  iEvent.getByToken(muonsToken, muonsHandle);
  if (!muonsHandle.isValid()) throw cms::Exception("CMS3Ntuplizer::analyze: Error getting the muon collection from the event...");


  /********************************/
  /* Set the communicator entries */
  /********************************/
  /*
  When naeing variables, try to be conscious of the nanoAOD naming conventions, but do not make a big fuss about them either!
  The latest list of variables are documented at https://cms-nanoaod-integration.web.cern.ch/integration/master-102X/mc102X_doc.html
  */

  // Convenience macros to easily make and push vector values
#define MAKE_VECTOR_WITH_RESERVE(type_, name_, size_) std::vector<type_> name_; name_.reserve(size_);
#define PUSH_USERINT_INTO_VECTOR(name_) name_.push_back(obj->userInt(#name_));
#define PUSH_USERFLOAT_INTO_VECTOR(name_) name_.push_back(obj->userFloat(#name_));
#define PUSH_VECTOR_WITH_NAME(name_, var_) commonEntry.setNamedVal(TString(name_)+"_"+#var_, var_);

  // Electrons
  {
    const char colName[] = "electrons";

    size_t n_electrons = electronsHandle->size();

    MAKE_VECTOR_WITH_RESERVE(float, pt, n_electrons);
    MAKE_VECTOR_WITH_RESERVE(float, eta, n_electrons);
    MAKE_VECTOR_WITH_RESERVE(float, phi, n_electrons);
    MAKE_VECTOR_WITH_RESERVE(float, mass, n_electrons);

    MAKE_VECTOR_WITH_RESERVE(int, charge, n_electrons);

    // Has no convention correspondence in nanoAOD
    MAKE_VECTOR_WITH_RESERVE(float, scale_smear_corr, n_electrons);
    MAKE_VECTOR_WITH_RESERVE(float, scale_smear_corr_scale_totalUp, n_electrons);
    MAKE_VECTOR_WITH_RESERVE(float, scale_smear_corr_scale_totalDn, n_electrons);
    MAKE_VECTOR_WITH_RESERVE(float, scale_smear_corr_smear_totalUp, n_electrons);
    MAKE_VECTOR_WITH_RESERVE(float, scale_smear_corr_smear_totalDn, n_electrons);

    MAKE_VECTOR_WITH_RESERVE(float, id_MVA_Fall17V2_Iso_Val, n_electrons);
    MAKE_VECTOR_WITH_RESERVE(unsigned int, id_MVA_Fall17V2_Iso_Cat, n_electrons);
    MAKE_VECTOR_WITH_RESERVE(float, id_MVA_Fall17V2_NoIso_Val, n_electrons);
    MAKE_VECTOR_WITH_RESERVE(unsigned int, id_MVA_Fall17V2_NoIso_Cat, n_electrons);

    MAKE_VECTOR_WITH_RESERVE(unsigned int, id_cutBased_Fall17V2_Veto_Bits, n_electrons);
    MAKE_VECTOR_WITH_RESERVE(unsigned int, id_cutBased_Fall17V2_Loose_Bits, n_electrons);
    MAKE_VECTOR_WITH_RESERVE(unsigned int, id_cutBased_Fall17V2_Medium_Bits, n_electrons);
    MAKE_VECTOR_WITH_RESERVE(unsigned int, id_cutBased_Fall17V2_Tight_Bits, n_electrons);

    MAKE_VECTOR_WITH_RESERVE(unsigned int, id_cutBased_Fall17V1_Veto_Bits, n_electrons);
    MAKE_VECTOR_WITH_RESERVE(unsigned int, id_cutBased_Fall17V1_Loose_Bits, n_electrons);
    MAKE_VECTOR_WITH_RESERVE(unsigned int, id_cutBased_Fall17V1_Medium_Bits, n_electrons);
    MAKE_VECTOR_WITH_RESERVE(unsigned int, id_cutBased_Fall17V1_Tight_Bits, n_electrons);

    MAKE_VECTOR_WITH_RESERVE(unsigned int, fid_mask, n_electrons);
    MAKE_VECTOR_WITH_RESERVE(unsigned int, type_mask, n_electrons);

    for (View<pat::Electron>::const_iterator obj = electronsHandle->begin(); obj != electronsHandle->end(); obj++){
      // Core particle quantities
      // Uncorrected p4
      pt.push_back(obj->pt());
      eta.push_back(obj->eta());
      phi.push_back(obj->phi());
      mass.push_back(obj->mass());

      // Charge: Can obtain pdgId from this, so no need to record pdgId again
      PUSH_USERINT_INTO_VECTOR(charge);

      // Scale and smear
      // Nominal value: Needs to multiply the uncorrected p4 at analysis level
      PUSH_USERFLOAT_INTO_VECTOR(scale_smear_corr);
      // Uncertainties: Only store total up/dn for the moment
      PUSH_USERFLOAT_INTO_VECTOR(scale_smear_corr_scale_totalUp);
      PUSH_USERFLOAT_INTO_VECTOR(scale_smear_corr_scale_totalDn);
      PUSH_USERFLOAT_INTO_VECTOR(scale_smear_corr_smear_totalUp);
      PUSH_USERFLOAT_INTO_VECTOR(scale_smear_corr_smear_totalDn);

      // Id variables
      // Fall17V2_Iso MVA id
      PUSH_USERFLOAT_INTO_VECTOR(id_MVA_Fall17V2_Iso_Val);
      PUSH_USERINT_INTO_VECTOR(id_MVA_Fall17V2_Iso_Cat);

      // Fall17V2_NoIso MVA id
      PUSH_USERFLOAT_INTO_VECTOR(id_MVA_Fall17V2_NoIso_Val);
      PUSH_USERINT_INTO_VECTOR(id_MVA_Fall17V2_NoIso_Cat);

      // Fall17V2 cut-based ids
      PUSH_USERINT_INTO_VECTOR(id_cutBased_Fall17V2_Veto_Bits);
      PUSH_USERINT_INTO_VECTOR(id_cutBased_Fall17V2_Loose_Bits);
      PUSH_USERINT_INTO_VECTOR(id_cutBased_Fall17V2_Medium_Bits);
      PUSH_USERINT_INTO_VECTOR(id_cutBased_Fall17V2_Tight_Bits);

      // Fall17V1 cut-based ids
      PUSH_USERINT_INTO_VECTOR(id_cutBased_Fall17V1_Veto_Bits);
      PUSH_USERINT_INTO_VECTOR(id_cutBased_Fall17V1_Loose_Bits);
      PUSH_USERINT_INTO_VECTOR(id_cutBased_Fall17V1_Medium_Bits);
      PUSH_USERINT_INTO_VECTOR(id_cutBased_Fall17V1_Tight_Bits);

      // Masks
      PUSH_USERINT_INTO_VECTOR(fid_mask);
      PUSH_USERINT_INTO_VECTOR(type_mask);
    }

    // Pass collections to the communicator
    PUSH_VECTOR_WITH_NAME(colName, pt);
    PUSH_VECTOR_WITH_NAME(colName, eta);
    PUSH_VECTOR_WITH_NAME(colName, phi);
    PUSH_VECTOR_WITH_NAME(colName, mass);

    PUSH_VECTOR_WITH_NAME(colName, charge);

    // Has no convention correspondence in nanoAOD
    PUSH_VECTOR_WITH_NAME(colName, scale_smear_corr);
    PUSH_VECTOR_WITH_NAME(colName, scale_smear_corr_scale_totalUp);
    PUSH_VECTOR_WITH_NAME(colName, scale_smear_corr_scale_totalDn);
    PUSH_VECTOR_WITH_NAME(colName, scale_smear_corr_smear_totalUp);
    PUSH_VECTOR_WITH_NAME(colName, scale_smear_corr_smear_totalDn);

    PUSH_VECTOR_WITH_NAME(colName, id_MVA_Fall17V2_Iso_Val);
    PUSH_VECTOR_WITH_NAME(colName, id_MVA_Fall17V2_Iso_Cat);
    PUSH_VECTOR_WITH_NAME(colName, id_MVA_Fall17V2_NoIso_Val);
    PUSH_VECTOR_WITH_NAME(colName, id_MVA_Fall17V2_NoIso_Cat);

    PUSH_VECTOR_WITH_NAME(colName, id_cutBased_Fall17V2_Veto_Bits);
    PUSH_VECTOR_WITH_NAME(colName, id_cutBased_Fall17V2_Loose_Bits);
    PUSH_VECTOR_WITH_NAME(colName, id_cutBased_Fall17V2_Medium_Bits);
    PUSH_VECTOR_WITH_NAME(colName, id_cutBased_Fall17V2_Tight_Bits);

    PUSH_VECTOR_WITH_NAME(colName, id_cutBased_Fall17V1_Veto_Bits);
    PUSH_VECTOR_WITH_NAME(colName, id_cutBased_Fall17V1_Loose_Bits);
    PUSH_VECTOR_WITH_NAME(colName, id_cutBased_Fall17V1_Medium_Bits);
    PUSH_VECTOR_WITH_NAME(colName, id_cutBased_Fall17V1_Tight_Bits);

    PUSH_VECTOR_WITH_NAME(colName, fid_mask);
    PUSH_VECTOR_WITH_NAME(colName, type_mask);

  }

  // Photons
  {
    const char colName[] = "photons";

    size_t n_photons = photonsHandle->size();

    MAKE_VECTOR_WITH_RESERVE(float, pt, n_photons);
    MAKE_VECTOR_WITH_RESERVE(float, eta, n_photons);
    MAKE_VECTOR_WITH_RESERVE(float, phi, n_photons);
    MAKE_VECTOR_WITH_RESERVE(float, mass, n_photons);

    // Has no convention correspondence in nanoAOD
    MAKE_VECTOR_WITH_RESERVE(float, scale_smear_corr, n_photons);
    MAKE_VECTOR_WITH_RESERVE(float, scale_smear_corr_scale_totalUp, n_photons);
    MAKE_VECTOR_WITH_RESERVE(float, scale_smear_corr_scale_totalDn, n_photons);
    MAKE_VECTOR_WITH_RESERVE(float, scale_smear_corr_smear_totalUp, n_photons);
    MAKE_VECTOR_WITH_RESERVE(float, scale_smear_corr_smear_totalDn, n_photons);

    MAKE_VECTOR_WITH_RESERVE(unsigned int, id_cutBased_Fall17V2_Loose_Bits, n_photons);
    MAKE_VECTOR_WITH_RESERVE(unsigned int, id_cutBased_Fall17V2_Medium_Bits, n_photons);
    MAKE_VECTOR_WITH_RESERVE(unsigned int, id_cutBased_Fall17V2_Tight_Bits, n_photons);

    for (View<pat::Photon>::const_iterator obj = photonsHandle->begin(); obj != photonsHandle->end(); obj++){
      // Core particle quantities
      // Uncorrected p4
      pt.push_back(obj->pt());
      eta.push_back(obj->eta());
      phi.push_back(obj->phi());
      mass.push_back(obj->mass());

      // Scale and smear
      // Nominal value: Needs to multiply the uncorrected p4 at analysis level
      PUSH_USERFLOAT_INTO_VECTOR(scale_smear_corr);
      // Uncertainties: Only store total up/dn for the moment
      PUSH_USERFLOAT_INTO_VECTOR(scale_smear_corr_scale_totalUp);
      PUSH_USERFLOAT_INTO_VECTOR(scale_smear_corr_scale_totalDn);
      PUSH_USERFLOAT_INTO_VECTOR(scale_smear_corr_smear_totalUp);
      PUSH_USERFLOAT_INTO_VECTOR(scale_smear_corr_smear_totalDn);

      // Id variables
      // Fall17V2 cut-based ids
      PUSH_USERINT_INTO_VECTOR(id_cutBased_Fall17V2_Loose_Bits);
      PUSH_USERINT_INTO_VECTOR(id_cutBased_Fall17V2_Medium_Bits);
      PUSH_USERINT_INTO_VECTOR(id_cutBased_Fall17V2_Tight_Bits);
    }

    // Pass collections to the communicator
    PUSH_VECTOR_WITH_NAME(colName, pt);
    PUSH_VECTOR_WITH_NAME(colName, eta);
    PUSH_VECTOR_WITH_NAME(colName, phi);
    PUSH_VECTOR_WITH_NAME(colName, mass);

    // Has no convention correspondence in nanoAOD
    PUSH_VECTOR_WITH_NAME(colName, scale_smear_corr);
    PUSH_VECTOR_WITH_NAME(colName, scale_smear_corr_scale_totalUp);
    PUSH_VECTOR_WITH_NAME(colName, scale_smear_corr_scale_totalDn);
    PUSH_VECTOR_WITH_NAME(colName, scale_smear_corr_smear_totalUp);
    PUSH_VECTOR_WITH_NAME(colName, scale_smear_corr_smear_totalDn);

    PUSH_VECTOR_WITH_NAME(colName, id_cutBased_Fall17V2_Loose_Bits);
    PUSH_VECTOR_WITH_NAME(colName, id_cutBased_Fall17V2_Medium_Bits);
    PUSH_VECTOR_WITH_NAME(colName, id_cutBased_Fall17V2_Tight_Bits);

  }

  // Muons
  {
    //const char colName[] = "Muon"; // nanoAOD
    const char colName[] = "muons";

    size_t n_muons = muonsHandle->size();

    MAKE_VECTOR_WITH_RESERVE(float, pt, n_muons);
    MAKE_VECTOR_WITH_RESERVE(float, eta, n_muons);
    MAKE_VECTOR_WITH_RESERVE(float, phi, n_muons);
    MAKE_VECTOR_WITH_RESERVE(float, mass, n_muons);

    MAKE_VECTOR_WITH_RESERVE(int, charge, n_muons);

    MAKE_VECTOR_WITH_RESERVE(float, scale_smear_pt_corr, n_muons);
    MAKE_VECTOR_WITH_RESERVE(float, scale_smear_pt_corr_scale_totalUp, n_muons);
    MAKE_VECTOR_WITH_RESERVE(float, scale_smear_pt_corr_scale_totalDn, n_muons);
    MAKE_VECTOR_WITH_RESERVE(float, scale_smear_pt_corr_smear_totalUp, n_muons);
    MAKE_VECTOR_WITH_RESERVE(float, scale_smear_pt_corr_smear_totalDn, n_muons);


    for (View<pat::Muon>::const_iterator obj = muonsHandle->begin(); obj != muonsHandle->end(); obj++){
      // Core particle quantities
      pt.push_back(obj->pt());
      eta.push_back(obj->eta());
      phi.push_back(obj->phi());
      mass.push_back(obj->mass());

      PUSH_USERINT_INTO_VECTOR(charge);

      PUSH_USERFLOAT_INTO_VECTOR(scale_smear_pt_corr);
      PUSH_USERFLOAT_INTO_VECTOR(scale_smear_pt_corr_scale_totalUp);
      PUSH_USERFLOAT_INTO_VECTOR(scale_smear_pt_corr_scale_totalDn);
      PUSH_USERFLOAT_INTO_VECTOR(scale_smear_pt_corr_smear_totalUp);
      PUSH_USERFLOAT_INTO_VECTOR(scale_smear_pt_corr_smear_totalDn);

    }

    // Pass collections to the communicator
    PUSH_VECTOR_WITH_NAME(colName, pt);
    PUSH_VECTOR_WITH_NAME(colName, eta);
    PUSH_VECTOR_WITH_NAME(colName, phi);
    PUSH_VECTOR_WITH_NAME(colName, mass);

    PUSH_VECTOR_WITH_NAME(colName, charge);

    PUSH_VECTOR_WITH_NAME(colName, scale_smear_pt_corr);
    PUSH_VECTOR_WITH_NAME(colName, scale_smear_pt_corr_scale_totalUp);
    PUSH_VECTOR_WITH_NAME(colName, scale_smear_pt_corr_scale_totalDn);
    PUSH_VECTOR_WITH_NAME(colName, scale_smear_pt_corr_smear_totalUp);
    PUSH_VECTOR_WITH_NAME(colName, scale_smear_pt_corr_smear_totalDn);

  }


  // GenInfo
  if (isMC){
    edm::Handle< GenInfo > genInfo;
    iEvent.getByToken(genInfoToken, genInfo);
    if (!genInfo.isValid()) throw cms::Exception("CMS3Ntuplizer::analyze: Error getting the gen. info. from the event...");

    recordGenInfo(*genInfo);

  }

  // Undefine the convenience macros
#undef PUSH_VECTOR_WITH_NAME
#undef PUSH_USERFLOAT_INTO_VECTOR
#undef PUSH_USERINT_INTO_VECTOR
#undef MAKE_VECTOR_WITH_RESERVE

  /************************************************/
  /* Record the communicator values into the tree */
  /************************************************/

  // If this is the first event, create the tree branches based on what is available in the commonEntry.
  if (firstEvent){
#define SIMPLE_DATA_OUTPUT_DIRECTIVE(name_t, type) for (auto itb=commonEntry.named##name_t##s.begin(); itb!=commonEntry.named##name_t##s.end(); itb++) outtree->putBranch(itb->first, itb->second);
#define VECTOR_DATA_OUTPUT_DIRECTIVE(name_t, type) for (auto itb=commonEntry.namedV##name_t##s.begin(); itb!=commonEntry.namedV##name_t##s.end(); itb++) outtree->putBranch(itb->first, &(itb->second));
#define DOUBLEVECTOR_DATA_OUTPUT_DIRECTIVE(name_t, type) for (auto itb=commonEntry.namedVV##name_t##s.begin(); itb!=commonEntry.namedVV##name_t##s.end(); itb++) outtree->putBranch(itb->first, &(itb->second));
    SIMPLE_DATA_OUTPUT_DIRECTIVES
    VECTOR_DATA_OUTPUT_DIRECTIVES
    DOUBLEVECTOR_DATA_OUTPUT_DIRECTIVES
#undef SIMPLE_DATA_OUTPUT_DIRECTIVE
#undef VECTOR_DATA_OUTPUT_DIRECTIVE
#undef DOUBLEVECTOR_DATA_OUTPUT_DIRECTIVE
  }

  // Record whatever is in commonEntry into the tree.
#define SIMPLE_DATA_OUTPUT_DIRECTIVE(name_t, type) for (auto itb=commonEntry.named##name_t##s.begin(); itb!=commonEntry.named##name_t##s.end(); itb++) outtree->setVal(itb->first, itb->second);
#define VECTOR_DATA_OUTPUT_DIRECTIVE(name_t, type) for (auto itb=commonEntry.namedV##name_t##s.begin(); itb!=commonEntry.namedV##name_t##s.end(); itb++) outtree->setVal(itb->first, &(itb->second));
#define DOUBLEVECTOR_DATA_OUTPUT_DIRECTIVE(name_t, type) for (auto itb=commonEntry.namedVV##name_t##s.begin(); itb!=commonEntry.namedVV##name_t##s.end(); itb++) outtree->setVal(itb->first, &(itb->second));
  SIMPLE_DATA_OUTPUT_DIRECTIVES
  VECTOR_DATA_OUTPUT_DIRECTIVES
  DOUBLEVECTOR_DATA_OUTPUT_DIRECTIVES
#undef SIMPLE_DATA_OUTPUT_DIRECTIVE
#undef VECTOR_DATA_OUTPUT_DIRECTIVE
#undef DOUBLEVECTOR_DATA_OUTPUT_DIRECTIVE

  // Fill the tree
  outtree->fill();

  // No longer the first event...
  if (firstEvent) firstEvent = false;
}

void CMS3Ntuplizer::recordGenInfo(GenInfo const& genInfo){

#define SET_GENINFO_VARIABLE(var) commonEntry.setNamedVal(#var, genInfo.var);

  SET_GENINFO_VARIABLE(xsec)
  SET_GENINFO_VARIABLE(xsecerr)

  SET_GENINFO_VARIABLE(qscale)
  SET_GENINFO_VARIABLE(alphaS)

  SET_GENINFO_VARIABLE(genMET)
  SET_GENINFO_VARIABLE(genMETPhi)

  SET_GENINFO_VARIABLE(sumEt)
  SET_GENINFO_VARIABLE(pThat)

  // LHE variations
  SET_GENINFO_VARIABLE(genHEPMCweight_default)
  SET_GENINFO_VARIABLE(genHEPMCweight_NNPDF30)

  SET_GENINFO_VARIABLE(LHEweight_QCDscale_muR1_muF1)
  SET_GENINFO_VARIABLE(LHEweight_QCDscale_muR1_muF2)
  SET_GENINFO_VARIABLE(LHEweight_QCDscale_muR1_muF0p5)
  SET_GENINFO_VARIABLE(LHEweight_QCDscale_muR2_muF1)
  SET_GENINFO_VARIABLE(LHEweight_QCDscale_muR2_muF2)
  SET_GENINFO_VARIABLE(LHEweight_QCDscale_muR2_muF0p5)
  SET_GENINFO_VARIABLE(LHEweight_QCDscale_muR0p5_muF1)
  SET_GENINFO_VARIABLE(LHEweight_QCDscale_muR0p5_muF2)
  SET_GENINFO_VARIABLE(LHEweight_QCDscale_muR0p5_muF0p5)

  SET_GENINFO_VARIABLE(LHEweight_PDFVariation_Up_default)
  SET_GENINFO_VARIABLE(LHEweight_PDFVariation_Dn_default)
  SET_GENINFO_VARIABLE(LHEweight_AsMZ_Up_default)
  SET_GENINFO_VARIABLE(LHEweight_AsMZ_Dn_default)

  SET_GENINFO_VARIABLE(LHEweight_PDFVariation_Up_NNPDF30)
  SET_GENINFO_VARIABLE(LHEweight_PDFVariation_Dn_NNPDF30)
  SET_GENINFO_VARIABLE(LHEweight_AsMZ_Up_NNPDF30)
  SET_GENINFO_VARIABLE(LHEweight_AsMZ_Dn_NNPDF30)

  // Pythis PS weights
  SET_GENINFO_VARIABLE(PythiaWeight_isr_muRoneoversqrt2)
  SET_GENINFO_VARIABLE(PythiaWeight_fsr_muRoneoversqrt2)
  SET_GENINFO_VARIABLE(PythiaWeight_isr_muRsqrt2)
  SET_GENINFO_VARIABLE(PythiaWeight_fsr_muRsqrt2)
  SET_GENINFO_VARIABLE(PythiaWeight_isr_muR0p5)
  SET_GENINFO_VARIABLE(PythiaWeight_fsr_muR0p5)
  SET_GENINFO_VARIABLE(PythiaWeight_isr_muR2)
  SET_GENINFO_VARIABLE(PythiaWeight_fsr_muR2)
  SET_GENINFO_VARIABLE(PythiaWeight_isr_muR0p25)
  SET_GENINFO_VARIABLE(PythiaWeight_fsr_muR0p25)
  SET_GENINFO_VARIABLE(PythiaWeight_isr_muR4)
  SET_GENINFO_VARIABLE(PythiaWeight_fsr_muR4)

#undef SET_GENINFO_VARIABLE

  for (auto const it:genInfo.LHE_ME_weights) commonEntry.setNamedVal(it.first, it.second);

}


//define this as a plug-in
DEFINE_FWK_MODULE(CMS3Ntuplizer);
// vim: ts=2:sw=2