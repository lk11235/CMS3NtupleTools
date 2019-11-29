#include <cassert>
#include "ParticleObjectHelpers.h"
#include "DileptonHandler.h"
#include "MELAStreamHelpers.hh"


using namespace std;
using namespace MELAStreamHelpers;


DileptonHandler::DileptonHandler(){}


bool DileptonHandler::constructDileptons(
  std::vector<MuonObject*> const* muons,
  std::vector<ElectronObject*> const* electrons
){
  bool res = (constructOSDileptons(muons, electrons) && constructSSDileptons(muons, electrons));
  // Sort particles here
  if (res) ParticleObjectHelpers::sortByGreaterPt(productList);
  return res;
}
bool DileptonHandler::constructSSDileptons(
  std::vector<MuonObject*> const* muons,
  std::vector<ElectronObject*> const* electrons
){
  std::vector<ParticleObject*> lepMinusPlus[2][2]; // l-, l+

  if (electrons){
    for (std::vector<ElectronObject*>::const_iterator it = electrons->begin(); it!=electrons->end(); it++){ // Electrons
      int iFirst = 0;
      int iSecond = ((*it)->pdgId()<0 ? 1 : 0);
      lepMinusPlus[iFirst][iSecond].push_back(*it);
    }
  }
  if (muons){
    for (std::vector<MuonObject*>::const_iterator it = muons->begin(); it!=muons->end(); it++){ // Muons
      int iFirst = 1;
      int iSecond = ((*it)->pdgId()<0 ? 1 : 0);
      lepMinusPlus[iFirst][iSecond].push_back(*it);
    }
  }
  // OSSF
  for (int c=0; c<2; c++){
    for (ParticleObject* F1:lepMinusPlus[c][0]){
      for (ParticleObject* F2:lepMinusPlus[c][1]){
        if (ParticleObject::checkDeepDaughtership(F1, F2)) continue;
        ParticleObject::LorentzVector_t pV = F1->p4() + F2->p4();
        ParticleObject* V = new ParticleObject(23, pV);
        V->addDaughter(F1);
        V->addDaughter(F2);
        productList.push_back(V);
      }
    }
  }
  // OSDF
  for (int c=0; c<2; c++){
    for (ParticleObject* F1:lepMinusPlus[c][0]){
      for (ParticleObject* F2:lepMinusPlus[1-c][1]){
        if (ParticleObject::checkDeepDaughtership(F1, F2)) continue;
        ParticleObject::LorentzVector_t pV = F1->p4() + F2->p4();
        ParticleObject* V = new ParticleObject(0, pV);
        V->addDaughter(F1);
        V->addDaughter(F2);
        productList.push_back(V);
      }
    }
  }

  return true;
}
bool DileptonHandler::constructOSDileptons(
  std::vector<MuonObject*> const* muons,
  std::vector<ElectronObject*> const* electrons
){
  std::vector<ParticleObject*> lepMinusPlus[2][2]; // l-, l+

  if (electrons){
    for (std::vector<ElectronObject*>::const_iterator it = electrons->begin(); it!=electrons->end(); it++){ // Electrons
      int iFirst = 0;
      int iSecond = ((*it)->pdgId()<0 ? 1 : 0);
      lepMinusPlus[iFirst][iSecond].push_back(*it);
    }
  }
  if (muons){
    for (std::vector<MuonObject*>::const_iterator it = muons->begin(); it!=muons->end(); it++){ // Muons
      int iFirst = 1;
      int iSecond = ((*it)->pdgId()<0 ? 1 : 0);
      lepMinusPlus[iFirst][iSecond].push_back(*it);
    }
  }
  // OSSF
  for (int c=0; c<2; c++){
    for (ParticleObject* F1:lepMinusPlus[c][0]){
      for (ParticleObject* F2:lepMinusPlus[c][1]){
        if (ParticleObject::checkDeepDaughtership(F1, F2)) continue;
        ParticleObject::LorentzVector_t pV = F1->p4() + F2->p4();
        ParticleObject* V = new ParticleObject(23, pV);
        V->addDaughter(F1);
        V->addDaughter(F2);
        productList.push_back(V);
      }
    }
  }
  // OSDF
  for (int c=0; c<2; c++){
    for (ParticleObject* F1:lepMinusPlus[c][0]){
      for (ParticleObject* F2:lepMinusPlus[1-c][1]){
        if (ParticleObject::checkDeepDaughtership(F1, F2)) continue;
        ParticleObject::LorentzVector_t pV = F1->p4() + F2->p4();
        ParticleObject* V = new ParticleObject(0, pV);
        V->addDaughter(F1);
        V->addDaughter(F2);
        productList.push_back(V);
      }
    }
  }

  return true;
}