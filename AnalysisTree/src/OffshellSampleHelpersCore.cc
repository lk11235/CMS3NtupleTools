#include <cassert>
#include "HelperFunctions.h"
#include "SampleHelpersCore.h"
#include "OffshellSampleHelpers.h"
#include "OffshellTriggerHelpers.h"
#include "HostHelpersCore.h"
#include "MELAStreamHelpers.hh"


using namespace std;
using namespace MELAStreamHelpers;
using namespace HelperFunctions;


namespace SampleHelpers{
  TString theSamplesTag="";
  bool runConfigure=false;
}

void SampleHelpers::configure(TString period, TString stag, HostHelpers::Hosts input_host){
  setDataPeriod(period);
/*
  TString strInputDir = "/home/users/usarica/work/Width_AC_Run2/Samples";
  if (stag.Contains(":")){
    std::vector<TString> splitstr; char delimiter=':';
    HelperFunctions::splitOptionRecursive(stag, splitstr, delimiter);
    assert(splitstr.size()<=3);
    stag = splitstr.back();
    if (splitstr.size()>=2){ // Order goes as "[store/hadoop/nfs-7/home]:[user (optional)]:[tag]
      TString const& strlocation = splitstr.front();
      if (strlocation == "hadoop" || strlocation == "store") strInputDir = "/store/user/usarica/Offshell_2L2Nu/Production";
      else if (strlocation == "hadoop_skims" || strlocation == "store_skims") strInputDir = "/store/user/usarica/Offshell_2L2Nu/Skims";
      else if (strlocation == "store_finaltrees") strInputDir = "/store/user/usarica/Offshell_2L2Nu/FinalTrees";

      if (splitstr.size()==3) HelperFunctions::replaceString<TString, const TString>(strInputDir, "usarica", splitstr.at(1));
    }
  }
  if (strInputDir.BeginsWith("/store")) strInputDir = HostHelpers::GetStandardHostPathToStore(strInputDir, input_host);
*/
  TString strInputDir = "/eos/user/l/lkang/Active_Research/highmass_SM/samples/PROD_samples_2017_MC_update_09326dd9/ignore/disregard/Signal_gg_hadd";
  theSamplesTag=stag;
  setInputDirectory(strInputDir);

  runConfigure=true;

//  TriggerHelpers::configureHLTmap();
}

std::string SampleHelpers::getDatasetDirectoryCoreName(std::string sname){ return SampleHelpers::getDatasetCoreName(sname); }
TString SampleHelpers::getDatasetDirectoryName(std::string sname, bool ignoreDNE){
  assert(theSamplesTag!="");
  assert(theInputDirectory!="");
  if (!HostHelpers::DirectoryExists(theInputDirectory.Data())){
    MELAerr << "SampleHelpers::getDatasetDirectoryName: The input directory " << theInputDirectory << " does not exist." << endl;
    assert(0);
  }

  sname = SampleHelpers::getDatasetDirectoryCoreName(sname);
  TString res = Form("%s/%s/%s", theInputDirectory.Data(), theSamplesTag.Data(), sname.data());
  bool dirExists = HostHelpers::DirectoryExists(res.Data());
  if (!dirExists){
    MELAerr << "SampleHelpers::getDatasetDirectoryName: Cannot find directory " << res << ". Trying the 'partial' collection tag." << endl;
    HelperFunctions::replaceString<TString, TString const>(res, theSamplesTag, (theSamplesTag+"_partial"));
    dirExists = HostHelpers::DirectoryExists(res.Data());
    if (!dirExists) MELAerr << "SampleHelpers::getDatasetDirectoryName: Directory " << res << " does not exist either." << endl;
  }
  assert(ignoreDNE || dirExists);
  return res;
}
TString SampleHelpers::getDatasetDirectoryName(TString sname, bool ignoreDNE){ return SampleHelpers::getDatasetDirectoryName(std::string(sname.Data()), ignoreDNE); }

TString SampleHelpers::getDatasetFileName(std::string sname, bool ignoreDNE){
  TString dsetdir = getDatasetDirectoryName(sname, ignoreDNE);
  auto dfiles = SampleHelpers::lsdir(dsetdir.Data());
  size_t nfiles = 0;
  TString firstFile = "";
  for (auto const& fname:dfiles){
    if (fname.Contains(".root")){
      if (nfiles == 0) firstFile = fname;
      nfiles++;
    }
  }
  if (nfiles==0) return "";
  else return (dsetdir + "/" + (nfiles==1 ? firstFile : "*.root"));
}
TString SampleHelpers::getDatasetFileName(TString sname, bool ignoreDNE){ return SampleHelpers::getDatasetFileName(std::string(sname.Data()), ignoreDNE); }

std::vector<TString> SampleHelpers::getDatasetFileNames(std::string sname, bool ignoreDNE){
  TString dsetdir = getDatasetDirectoryName(sname, ignoreDNE);
  auto dfiles = SampleHelpers::lsdir(dsetdir.Data());
  std::vector<TString> res; res.reserve(dfiles.size());
  for (auto const& fname:dfiles){
    if (fname.Contains(".root")) res.push_back(dsetdir + "/" + fname);
  }
  size_t const nfiles = res.size();
  if (nfiles==0){
    MELAerr << "SampleHelpers::getDatasetFileNames: Directory " << dsetdir << " contains no ROOT files." << endl;
    assert(ignoreDNE || nfiles>0);
  }
  return res;
}
std::vector<TString> SampleHelpers::getDatasetFileNames(TString sname, bool ignoreDNE){ return SampleHelpers::getDatasetFileNames(std::string(sname.Data()), ignoreDNE); }

TString SampleHelpers::getDatasetFirstFileName(std::string sname, bool ignoreDNE){
  std::vector<TString> fileList = SampleHelpers::getDatasetFileNames(sname, ignoreDNE);
  if (fileList.empty()){
    MELAerr << "SampleHelpers::getDatasetFirstFileName: Sample " << sname << " has no ROOT files." << endl;
    assert(ignoreDNE);
  }
  return (!fileList.empty() ? fileList.front() : TString(""));
}
TString SampleHelpers::getDatasetFirstFileName(TString sname, bool ignoreDNE){ return SampleHelpers::getDatasetFirstFileName(std::string(sname.Data()), ignoreDNE); }

bool SampleHelpers::checkFileOnWorker(TString& fname, HostHelpers::Hosts input_host){
  if (HostHelpers::FileReadable(fname) || HostHelpers::DirectoryExists(fname)) return true;

  TString fname_worker = Form("/store/user/usarica/Offshell_2L2Nu/Worker/%s", fname.Data());
  fname_worker = HostHelpers::GetStandardHostPathToStore(fname_worker.Data(), input_host);
  MELAout << "SampleHelpers::checkFileOnWorker: Could not find " << fname << ". Checking for the presence of " << fname_worker << "..." << endl;

  if (HostHelpers::FileReadable(fname_worker) || HostHelpers::DirectoryExists(fname_worker)){
    fname = fname_worker;
    MELAout << "\t- Will now use " << fname << " as a substitute." << endl;
    return true;
  }

  return false;
}
