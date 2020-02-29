#ifndef SIMEVENTHANDLER_H
#define SIMEVENTHANDLER_H

#include <vector>
#include <unordered_map>
#include "IvyBase.h"
#include "ElectronObject.h"
#include "PhotonObject.h"
#include "AK4JetObject.h"
#include "AK8JetObject.h"
#include "SystematicVariations.h"
#include "TH1F.h"


class SimEventHandler : public IvyBase{
public:
  enum EventRandomNumberType{
    kDataPeriod,
    kGenMETSmear
  };

protected:
  std::unordered_map< TString, std::vector<TH1F*> > map_DataPeriod_PUHistList;

  std::unordered_map<EventRandomNumberType, unsigned long long> product_rnds;
  TString theChosenDataPeriod;
  bool hasHEM2018Issue;
  float pileupWeight;
  float const* l1prefiringWeight;

  void setupPUHistograms();
  void clearPUHistograms();

  bool constructRandomNumbers();
  bool constructPUWeight(SystematicsHelpers::SystematicVariationTypes const& syst);
  bool constructL1PrefiringWeight(SystematicsHelpers::SystematicVariationTypes const& syst);

  void clear();

public:
  SimEventHandler();
  ~SimEventHandler();

  bool constructSimEvent(SystematicsHelpers::SystematicVariationTypes const& syst);

  void bookBranches(BaseTree* intree);

  TString const& getChosenDataPeriod() const;
  bool const& getHasHEM2018Issue() const{ return hasHEM2018Issue; }
  float const& getPileUpWeight() const{ return pileupWeight; }
  float getL1PrefiringWeight() const{ return (l1prefiringWeight ? *l1prefiringWeight : 1.); }

  unsigned long long const& getRandomNumberSeed(SimEventHandler::EventRandomNumberType type) const;

};


#endif
