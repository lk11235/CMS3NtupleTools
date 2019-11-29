#include <cassert>
#include "ParticleObjectHelpers.h"
#include "ElectronHandler.h"
#include "ElectronSelectionHelpers.h"
#include "MELAStreamHelpers.hh"


using namespace std;
using namespace MELAStreamHelpers;


#define VECTOR_ITERATOR_HANDLER_DIRECTIVES \
ELECTRON_VARIABLE(float, pt, 0) \
ELECTRON_VARIABLE(float, eta, 0) \
ELECTRON_VARIABLE(float, phi, 0) \
ELECTRON_VARIABLE(float, mass, 0) \
ELECTRON_VARIABLE(int, charge, 0) \
ELECTRON_VARIABLES


const std::string ElectronHandler::colName = "electrons";

ElectronHandler::ElectronHandler() : IvyBase()
{
#define ELECTRON_VARIABLE(TYPE, NAME, DEFVAL) this->addConsumed<std::vector<TYPE>*>(ElectronHandler::colName + "_" + #NAME);
  VECTOR_ITERATOR_HANDLER_DIRECTIVES;
#undef ELECTRON_VARIABLE
}


bool ElectronHandler::constructElectrons(SystematicsHelpers::SystematicVariationTypes const& syst){
  clear();
  if (!currentTree) return false;

#define ELECTRON_VARIABLE(TYPE, NAME, DEFVAL) std::vector<TYPE>::const_iterator itBegin_##NAME, itEnd_##NAME;
  VECTOR_ITERATOR_HANDLER_DIRECTIVES;
#undef ELECTRON_VARIABLE

    // Beyond this point starts checks and selection
  bool allVariablesPresent = true;
#define ELECTRON_VARIABLE(TYPE, NAME, DEFVAL) allVariablesPresent &= this->getConsumedCIterators<std::vector<TYPE>>(ElectronHandler::colName + "_" + #NAME, &itBegin_##NAME, &itEnd_##NAME);
  VECTOR_ITERATOR_HANDLER_DIRECTIVES;
#undef ELECTRON_VARIABLE

  if (!allVariablesPresent){
    if (this->verbosity>=TVar::ERROR) MELAerr << "ElectronHandler::constructElectrons: Not all variables are consumed properly!" << endl;
    assert(0);
  }

  if (this->verbosity>=TVar::DEBUG) MELAout << "ElectronHandler::constructElectrons: All variables are set up!" << endl;

  if (itBegin_charge == itEnd_charge) return true; // Construction is successful, it is just that no electrons exist.

  size_t nProducts = (itEnd_charge - itBegin_charge);
  productList.reserve(nProducts);
#define ELECTRON_VARIABLE(TYPE, NAME, DEFVAL) auto it_##NAME = itBegin_##NAME;
  VECTOR_ITERATOR_HANDLER_DIRECTIVES;
#undef ELECTRON_VARIABLE
  {
    size_t ip=0;
    while (it_charge != itEnd_charge){
      if (this->verbosity>=TVar::DEBUG) MELAout << "ElectronHandler::constructElectrons: Attempting electron " << ip << "..." << endl;

      ParticleObject::LorentzVector_t momentum;
      momentum = ParticleObject::PolarLorentzVector_t(*it_pt, *it_eta, *it_phi, *it_mass); // Yes you have to do this on a separate line because CMSSW...
      productList.push_back(new ElectronObject(-11*(*it_charge>0 ? 1 : -1), momentum));
      ElectronObject*& obj = productList.back();

      // Set extras
#define ELECTRON_VARIABLE(TYPE, NAME, DEFVAL) obj->extras.NAME = *it_##NAME;
      ELECTRON_VARIABLES;
#undef ELECTRON_VARIABLE

      // Replace momentum
      obj->makeFinalMomentum(syst);

      // Set the selection bits
      ElectronSelectionHelpers::setSelectionBits(*obj);

      if (this->verbosity>=TVar::DEBUG) MELAout << "\t- Success!" << endl;

      ip++;
#define ELECTRON_VARIABLE(TYPE, NAME, DEFVAL) it_##NAME++;
      VECTOR_ITERATOR_HANDLER_DIRECTIVES;
#undef ELECTRON_VARIABLE
    }
  }
  // Sort particles
  ParticleObjectHelpers::sortByGreaterPt(productList);

  return true;
}

void ElectronHandler::bookBranches(BaseTree* tree){
  if (!tree) return;

#define ELECTRON_VARIABLE(TYPE, NAME, DEFVAL) tree->bookBranch<std::vector<TYPE>*>(ElectronHandler::colName + "_" + #NAME, nullptr);
  VECTOR_ITERATOR_HANDLER_DIRECTIVES
#undef ELECTRON_VARIABLE
}


#undef VECTOR_ITERATOR_HANDLER_DIRECTIVES