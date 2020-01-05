#ifndef METOBJECT_H
#define METOBJECT_H

#include "SystematicVariations.h"
#include "ParticleObject.h"


#define MET_RECORDED_VARIABLES \
MET_VARIABLE(float, met_Nominal, 0) \
MET_VARIABLE(float, metPhi_Nominal, 0) \
MET_VARIABLE(float, met_JECUp, 0) \
MET_VARIABLE(float, metPhi_JECUp, 0) \
MET_VARIABLE(float, met_JECDn, 0) \
MET_VARIABLE(float, metPhi_JECDn, 0) \
MET_VARIABLE(float, metSignificance, 0)

#define MET_EXTRA_PT_VARIABLES \
MET_VARIABLE(float, met_original, 0) \
MET_VARIABLE(float, met_JERUp, 0) \
MET_VARIABLE(float, met_JERDn, 0) \
MET_VARIABLE(float, met_PUUp, 0) \
MET_VARIABLE(float, met_PUDn, 0) \
MET_VARIABLE(float, met_METUp, 0) \
MET_VARIABLE(float, met_METDn, 0) \

#define MET_EXTRA_PHI_VARIABLES \
MET_VARIABLE(float, metPhi_original, 0) \
MET_VARIABLE(float, metPhi_JERUp, 0) \
MET_VARIABLE(float, metPhi_JERDn, 0) \
MET_VARIABLE(float, metPhi_PUUp, 0) \
MET_VARIABLE(float, metPhi_PUDn, 0) \
MET_VARIABLE(float, metPhi_METUp, 0) \
MET_VARIABLE(float, metPhi_METDn, 0)


#define MET_EXTRA_VARIABLES \
MET_EXTRA_PT_VARIABLES \
MET_EXTRA_PHI_VARIABLES

#define MET_VARIABLES \
MET_RECORDED_VARIABLES \
MET_EXTRA_VARIABLES


class METVariables{
public:
#define MET_VARIABLE(TYPE, NAME, DEFVAL) TYPE NAME;
  MET_VARIABLES;
#undef MET_VARIABLE

  METVariables();
  METVariables(METVariables const& other);
  METVariables& operator=(const METVariables& other);

  void swap(METVariables& other);

};

class METObject{
public:
  METVariables extras;

protected:
  SystematicsHelpers::SystematicVariationTypes currentSyst;

public:
  METObject();
  METObject(const METObject& other);
  METObject& operator=(const METObject& other);
  ~METObject();

  void swap(METObject& other);

  void setSystematic(SystematicsHelpers::SystematicVariationTypes const&);

  void getPtPhi(float const*& pt, float const*& phi) const;
  ParticleObject::LorentzVector_t::Scalar met() const;
  ParticleObject::LorentzVector_t::Scalar phi() const;
  ParticleObject::LorentzVector_t::Scalar pt() const{ return met(); }
  ParticleObject::LorentzVector_t::Scalar px(float phi_rot=0) const;
  ParticleObject::LorentzVector_t::Scalar py(float phi_rot=0) const;
  ParticleObject::LorentzVector_t p4(float phi_rot=0) const;

};

#endif
