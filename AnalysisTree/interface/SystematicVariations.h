#ifndef SYSTEMATICVARIATIONS_H
#define SYSTEMATICVARIATIONS_H

#include <string>


namespace SystematicsHelpers{

  enum SystematicVariationTypes{
    sNominal = 0,

    tPDFScaleDn, tPDFScaleUp,
    tQCDScaleDn, tQCDScaleUp,
    tAsMZDn, tAsMZUp,
    tPDFReplicaDn, tPDFReplicaUp,
    tPythiaScaleDn, tPythiaScaleUp,
    tPythiaTuneDn, tPythiaTuneUp,

    eEleEffDn, eEleEffUp,
    eEleEffStatDn, eEleEffStatUp,
    eEleEffSystDn, eEleEffSystUp,
    eEleEffAltMCDn, eEleEffAltMCUp,
    eEleScaleDn, eEleScaleUp,
    eEleResDn, eEleResUp,

    eMuEffDn, eMuEffUp,
    eMuEffStatDn, eMuEffStatUp,
    eMuEffSystDn, eMuEffSystUp,
    eMuEffAltMCDn, eMuEffAltMCUp,
    eMuScaleDn, eMuScaleUp,
    eMuResDn, eMuResUp,

    ePhoEffDn, ePhoEffUp,
    ePhoScaleDn, ePhoScaleUp,
    ePhoResDn, ePhoResUp,

    eMETDn, eMETUp,
    eJECDn, eJECUp,
    eJERDn, eJERUp,
    ePUDn, ePUUp,
    ePUJetIdEffDn, ePUJetIdEffUp,
    eBTagSFDn, eBTagSFUp,

    eL1PrefiringDn, eL1PrefiringUp,

    eTriggerEffDn, eTriggerEffUp,

    nSystematicVariations,

    sUncorrected // For checks
  };

  bool isDownSystematic(SystematicsHelpers::SystematicVariationTypes const& type);
  bool isUpSystematic(SystematicsHelpers::SystematicVariationTypes const& type);
  SystematicsHelpers::SystematicVariationTypes getSystComplement(SystematicsHelpers::SystematicVariationTypes const& type);
  std::string getSystCoreName(SystematicsHelpers::SystematicVariationTypes const& type);
  std::string getSystName(SystematicsHelpers::SystematicVariationTypes const& type);

}

#endif
