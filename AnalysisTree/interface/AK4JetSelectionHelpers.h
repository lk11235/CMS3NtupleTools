#ifndef AK4JETSELECTIONHELPERS_H
#define AK4JETSELECTIONHELPERS_H

#include "AK4JetObject.h"


namespace AK4JetSelectionHelpers{
  enum SelectionBits{
    kGenPtEta,

    kPUJetId,
    kTightLeptonVetoId,

    kLooseId,
    kLooseKin,

    kTightId,
    kTightKin,

    kPreselectionLoose,
    kPreselectionTight,

    nSelectionBits
  };

  constexpr float ptThr_gen = 15.;
  constexpr float ptThr_skim_loose = 30.;
  constexpr float ptThr_skim_tight = 30.;

  constexpr float etaThr_gen = 5.2;
  constexpr float etaThr_skim_loose = 4.7;
  constexpr float etaThr_skim_tight = 4.7;

  // Preselection loose = AND of bits below
  const SelectionBits bit_preselectionLoose_id = kTightId;
  const SelectionBits bit_preselectionLoose_kin = kTightKin;

  // Preselection tight = AND of bits below && PU jet id
  const SelectionBits bit_preselectionTight_id = kTightId;
  const SelectionBits bit_preselectionTight_kin = kTightKin;

  // Control additional flags to preselection tight id
  void setApplyPUIdToJets(bool flag);
  void setApplyTightLeptonVetoIdToJets(bool flag);

  void setSelectionBits(AK4JetObject& part);
}


#endif
