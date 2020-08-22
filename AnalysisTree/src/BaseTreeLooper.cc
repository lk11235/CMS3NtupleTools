#include <algorithm>
#include <utility>
#include <iterator>
#include <cassert>

#include "BaseTreeLooper.h"
#include "SamplesCore.h"

#include "SimEventHandler.h"
#include "GenInfoHandler.h"
#include "EventFilterHandler.h"

#include "HelperFunctions.h"
#include "MELAStreamHelpers.hh"


using namespace std;
using namespace MELAStreamHelpers;


BaseTreeLooper::BaseTreeLooper() :
  IvyBase(),
  looperFunction(nullptr),
  registeredSyst(SystematicsHelpers::nSystematicVariations),

  maxNEvents(-1),
  eventIndex_begin(-1),
  eventIndex_end(-1),
  useChunkIndices(false),

  isData_currentTree(false),
  isQCD_currentTree(false)

{
  set_pTG_exception_range(-1, -1);
  setExternalProductList();
  setCurrentOutputTree();
}
BaseTreeLooper::BaseTreeLooper(BaseTree* inTree, double wgt) :
  IvyBase(),
  looperFunction(nullptr),
  registeredSyst(SystematicsHelpers::nSystematicVariations),

  maxNEvents(-1),
  eventIndex_begin(-1),
  eventIndex_end(-1),
  useChunkIndices(false),

  isData_currentTree(false),
  isQCD_currentTree(false)

{
  this->addTree(inTree, wgt);
  set_pTG_exception_range(-1, -1);
  setExternalProductList();
  setCurrentOutputTree();
}
BaseTreeLooper::BaseTreeLooper(std::vector<BaseTree*> const& inTreeList) :
  IvyBase(),

  looperFunction(nullptr),
  registeredSyst(SystematicsHelpers::nSystematicVariations),

  maxNEvents(-1),
  eventIndex_begin(-1),
  eventIndex_end(-1),
  useChunkIndices(false),

  isData_currentTree(false),
  isQCD_currentTree(false),

  treeList(inTreeList)
{
  set_pTG_exception_range(-1, -1);
  setExternalProductList();
  setCurrentOutputTree();
}
BaseTreeLooper::~BaseTreeLooper(){}

void BaseTreeLooper::addTree(BaseTree* tree, double wgt){
  if (tree && !HelperFunctions::checkListVariable(this->treeList, tree)){
    this->treeList.push_back(tree);
    setExternalWeight(tree, wgt);
  }
}

void BaseTreeLooper::addExternalFunction(TString fcnname, BaseTreeLooper::LooperExtFunction_t fcn){
  if (!fcn) return;
  if (externalFunctions.find(fcnname)!=externalFunctions.end()) MELAerr << "BaseTreeLooper::addExternalFunction: " << fcnname << " already exists but will override it regardless." << endl;
  externalFunctions[fcnname] = fcn;
}
void BaseTreeLooper::addObjectHandler(IvyBase* handler){
  if (handler && !HelperFunctions::checkListVariable(this->registeredHandlers, handler)) this->registeredHandlers.push_back(handler);
}
void BaseTreeLooper::addSFHandler(ScaleFactorHandlerBase* handler){
  if (handler && !HelperFunctions::checkListVariable(this->registeredSFHandlers, handler)) this->registeredSFHandlers.push_back(handler);
}

void BaseTreeLooper::addHLTMenu(TString name, std::vector< std::string > const& hltmenu){
  if (registeredHLTMenus.find(name)!=registeredHLTMenus.end()) MELAerr << "BaseTreeLooper::addHLTMenu: Simple HLT menu " << name << " already exists but will override it regardless." << endl;
  registeredHLTMenus[name] = hltmenu;
}
void BaseTreeLooper::addHLTMenu(TString name, std::vector< std::pair<TriggerHelpers::TriggerType, HLTTriggerPathProperties const*> > const& hltmenu){
  if (registeredHLTMenuProperties.find(name)!=registeredHLTMenuProperties.end()) MELAerr << "BaseTreeLooper::addHLTMenu: HLT menu properties " << name << " already exists but will override it regardless." << endl;
  registeredHLTMenuProperties[name] = hltmenu;
}

void BaseTreeLooper::setExternalWeight(BaseTree* tree, double const& wgt){
  if (!tree) return;
  if (this->verbosity>=TVar::INFO && !HelperFunctions::checkListVariable(treeList, tree)) MELAout
    << "BaseTreeLooper::setExternalWeight: Warning! Tree " << tree->sampleIdentifier
    << " is not in the list of input trees, but a weight of " << wgt << " is being assigned to it."
    << endl;
  globalWeights[tree] = wgt;
}

void BaseTreeLooper::setExternalProductList(std::vector<SimpleEntry>* extProductListRef){
  if (extProductListRef) this->productListRef = extProductListRef;
  else this->productListRef = &(this->productList);
}

void BaseTreeLooper::setCurrentOutputTree(BaseTree* extTree){
  this->currentProductTree = extTree;
  this->productListRef = &(this->productList); // To make sure product list collects some events before flushing
}

void BaseTreeLooper::addOutputTree(BaseTree* extTree){
  if (extTree && !HelperFunctions::checkListVariable(productTreeList, extTree)){
    productTreeList.push_back(extTree);
    setCurrentOutputTree(extTree);
  }
}
void BaseTreeLooper::addOutputTrees(std::vector<BaseTree*> trees){
  for (auto const& tt:trees) addOutputTree(tt);
}

void BaseTreeLooper::setMaximumEvents(int n){ maxNEvents = n; }
void BaseTreeLooper::setEventIndexRange(int istart, int iend){ eventIndex_begin = istart; eventIndex_end = iend; }


void BaseTreeLooper::addProduct(SimpleEntry& product, unsigned int* ev_rec){
  this->productListRef->push_back(product);
  if (ev_rec) (*ev_rec)++;

  // Record products to external tree
  this->recordProductsToTree();
}

void BaseTreeLooper::recordProductsToTree(){
  if (!this->currentProductTree) return;

  auto it_tree = firstTreeOutput.find(this->currentProductTree);
  if (it_tree == firstTreeOutput.cend()){
    firstTreeOutput[this->currentProductTree] = true;
    it_tree = firstTreeOutput.find(this->currentProductTree);
  }

  BaseTree::writeSimpleEntries(this->productListRef->cbegin(), this->productListRef->cend(), this->currentProductTree, it_tree->second);
  this->clearProducts();
}

bool BaseTreeLooper::wrapTree(BaseTree* tree){
  if (!tree) return false;
  bool res = true;

  // Sample flags
  TString const& sid = tree->sampleIdentifier;
  this->isData_currentTree = SampleHelpers::checkSampleIsData(sid);
  this->isQCD_currentTree = !this->isData_currentTree && sid.Contains("QCD") && sid.Contains("HT");
  if (!this->isData_currentTree && (sid.Contains("ZGTo2NuG") || sid.Contains("ZGTo2LG")) && sid.Contains("amcatnloFXFX") && !sid.Contains("PtG-130")) set_pTG_exception_range(-1, 130);
  else set_pTG_exception_range(-1, -1);

  for (auto const& handler:registeredHandlers){
    bool isHandlerForSim = (dynamic_cast<GenInfoHandler*>(handler) != nullptr || dynamic_cast<SimEventHandler*>(handler) != nullptr);
    EventFilterHandler* eventFilter = dynamic_cast<EventFilterHandler*>(handler);
    if (eventFilter){
      bool isFirstInputFile = (tree == treeList.front());
      eventFilter->setTrackDataEvents(this->isData_currentTree);
      eventFilter->setCheckUniqueDataEvent(this->isData_currentTree && !isFirstInputFile);
    }
    if (!this->isData_currentTree || (this->isData_currentTree && !isHandlerForSim)) res &= handler->wrapTree(tree);
  }
  res &= IvyBase::wrapTree(tree);
  return res;
}


void BaseTreeLooper::loop(bool keepProducts){
  if (!looperFunction){
    MELAerr << "BaseTreeLooper::loop: The looper function is not registered. Please register it using BadeTreeLooper::setLooperFunction." << endl;
    return;
  }

  // Count total number of events
  int nevents_total = 0;
  for (auto& tree:treeList) nevents_total += tree->getNEvents();

  // Adjust event ranges to actual event indices
  if (this->useChunkIndices && eventIndex_end>0){
    const int ichunk = eventIndex_begin;
    const int nchunks = eventIndex_end;
    int ev_inc = static_cast<int>(float(nevents_total)/float(nchunks));
    eventIndex_begin = ev_inc*ichunk;
    eventIndex_end = std::min(nevents_total, (ichunk == nchunks-1 ? nevents_total : eventIndex_begin + ev_inc));
  }

  // Loop over the trees
  unsigned int ev_traversed=0;
  unsigned int ev_acc=0;
  unsigned int ev_rec=0;
  for (auto& tree:treeList){
    // Skip the tree if it cannot be wrapped
    if (!(this->wrapTree(tree))) continue;

    if (
      this->isData_currentTree
      &&
      (eventIndex_begin>0 || (eventIndex_end>0 && eventIndex_end<nevents_total))
      ){
      MELAerr << "BaseTreeLooper::loop: " << tree->sampleIdentifier << " is a data tree, and splitting events is not permitted for data!" << endl;
      assert(0);
    }

    double globalTreeWeight = 1;
    auto it_globalWgt = globalWeights.find(tree);
    if (it_globalWgt!=globalWeights.cend()) globalTreeWeight = it_globalWgt->second;

    const int nevents = tree->getNEvents();
    MELAout << "BaseTreeLooper::loop: Looping over " << nevents << " events in " << tree->sampleIdentifier << "..." << endl;
    for (int ev=0; ev<nevents; ev++){
      if (maxNEvents>=0 && (int) ev_rec==maxNEvents) break;
      if (
        (eventIndex_begin<0 || (int) ev_traversed>=eventIndex_begin)
        &&
        (eventIndex_end<0 || (int) ev_traversed<eventIndex_end)
        ){
        if (tree->getEvent(ev)){
          SimpleEntry product;
          if (tree->isValidEvent()){
            if (this->looperFunction(this, globalTreeWeight, product)){
              if (keepProducts) this->addProduct(product, &ev_rec);
            }
          }
        }
        ev_acc++;
      }
      HelperFunctions::progressbar(ev, nevents);
      ev_traversed++;
    }
  } // End loop over the trees
  MELAout << "BaseTreeLooper::loop: Total number of products: " << ev_rec << " / " << ev_acc << " / " << ev_traversed << endl;
}

std::vector<SimpleEntry> const& BaseTreeLooper::getProducts() const{ return *productListRef; }

void BaseTreeLooper::moveProducts(std::vector<SimpleEntry>& targetColl){
  MELAout << "BaseTreeLooper::moveProducts: Moving " << productListRef->size() << " products into a list of initial size " << targetColl.size() << endl;
  std::move(productListRef->begin(), productListRef->end(), std::back_inserter(targetColl));
  clearProducts();
  MELAout << "BaseTreeLooper::moveProducts: Target list final size: " << targetColl.size() << endl;
}

void BaseTreeLooper::clearProducts(){ productListRef->clear(); }