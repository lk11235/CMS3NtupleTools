import os
import glob
import ROOT
from math import sqrt
import time
#from os import path
from pathlib import Path
import re
from tqdm import trange, tqdm
import numpy as np
from array import *
#%jsroot on


# filename = directory+"ggTo2mu2tau_0PMH125"+"_MCFM701/ZZ4lAnalysis.root"

# filename = "/afs/cern.ch/work/l/lkang/CMS3/CMSSW_10_2_22_/src/CMS3/AnalysisTree/test/output/ReweightedGenTrees/3/2017/GGHToZZTo4L_Nominal.root"
filename = "/eos/user/l/lkang/Active_Research/POWHEG/GGHToZZTo4L_Nominal.root"
filename = "/eos/user/l/lkang/Active_Research/POWHEG/test.root"
filename = "/afs/cern.ch/work/l/lkang/CMS3/CMSSW_10_2_22_/src/CMS3/AnalysisTree/test/output/ReweightedGenTrees/10/2017/GGHToZZTo4L_Nominal.root"

filenameorig = "/eos/cms/store/group/phys_higgs/cmshzz4l/cjlst/RunIILegacy/200205_CutBased/MC_2017/HighMass/ggH200/ZZ4lAnalysis.root"

filename1 = "/eos/cms/store/group/phys_higgs/cmshzz4l/cjlst/RunIILegacy/200205_CutBased/MC_2017/OffshellAC/gg/ggTo2e2mu_0PMH125_MCFM701/ZZ4lAnalysis.root"
filename2 = "/eos/cms/store/group/phys_higgs/cmshzz4l/cjlst/RunIILegacy/200205_CutBased/MC_2017/OffshellAC/gg/ggTo4e_0PMH125_MCFM701/ZZ4lAnalysis.root"
filename3 = "/eos/cms/store/group/phys_higgs/cmshzz4l/cjlst/RunIILegacy/200205_CutBased/MC_2017/OffshellAC/gg/ggTo4mu_0PMH125_MCFM701/ZZ4lAnalysis.root"


print(filename)
# if not Path(filename).exists():
#     if Path(directory+samples[sam]+"/ZZ4lAnalysis.root").exists():
#         filename = directory+samples[sam]+"/ZZ4lAnalysis.root"
#     else:
#         print("ERROR: " + filename + " does not exist!")
# #         continue
# #else: continue

f = ROOT.TFile(filename)

fo = ROOT.TFile(filenameorig)

f1 = ROOT.TFile(filename1)
f2 = ROOT.TFile(filename2)
f3 = ROOT.TFile(filename3)

# if f.GetListOfKeys().Contains("ZZTree"):
#     tp = f.Get("ZZTree/candTree")
#     tf = f.Get("ZZTree/candTree_failed")
# #             print(f.GetListOfKeys().Contains("ZZTree"))
# else: 
#     print("False")
# #     continue

# #         if ("-" in refden[samples[sam]]) or ("_0p" in refden[samples[sam]]): continue
# #         skey = samples[samlist][sam].split("/")[-1].replace('_M125_GaSM', '')    

print(f)

tp = f.Get("SkimTree")

tpo = fo.Get("ZZTree/candTree")
tfo = fo.Get("ZZTree/candTree_failed")

tp1 = f1.Get("ZZTree/candTree")
tf1 = f1.Get("ZZTree/candTree_failed")
tp2 = f2.Get("ZZTree/candTree")
tf2 = f2.Get("ZZTree/candTree_failed")
tp3 = f3.Get("ZZTree/candTree")
tf3 = f3.Get("ZZTree/candTree_failed")

print(tp)

c1 = ROOT.TCanvas("c1","c1",2000,1500)
leg = ROOT.TLegend(0.75,0.72,1.13,0.88)

# hp = ROOT.TH1F("pass","",300,0,150)
# hf = ROOT.TH1F("fail","",300,0,150)

hp = ROOT.TH1F("pass","",200,180,2000)
# hf = ROOT.TH1F("fail","",4000,0,2000)

hpo = ROOT.TH1F("passo","",200,180,2000)
hfo = ROOT.TH1F("failo","",200,180,2000)

hp1 = ROOT.TH1F("pass1","",200,180,2000)
hf1 = ROOT.TH1F("fail1","",200,180,2000)
hp2 = ROOT.TH1F("pass2","",200,180,2000)
hf2 = ROOT.TH1F("fail2","",200,180,2000)
hp3 = ROOT.TH1F("pass3","",200,180,2000)
hf3 = ROOT.TH1F("fail3","",200,180,2000)

print(hp)

# tp.Draw("GenHMass>>pass","77.4*event_wgt*KFactor_QCD_ggZZ_Nominal*CPS*samnple*(!invalidrwghtweigh)*p_Gen_GG_SIG_kappaTopBot_1_ghz1_1_MCFM","")
tp.Draw("GenHMass>>pass","77.4*event_wgt*KFactor_QCD_ggZZ_Nominal*p_Gen_CPStoBWPropRewgt*sample_wgt*(!invalidReweightingWgts)*p_Gen_GG_SIG_kappaTopBot_1_ghz1_1_MCFM","")

tpo.Draw("GenHMass>>passo","77.4*1000*xsec*KFactor_QCD_ggZZ_Nominal*p_Gen_CPStoBWPropRewgt*overallEventWeight*L1prefiringWeight/"+str(f1.Get("ZZTree/Counters").GetBinContent(40)),"")
tfo.Draw("GenHMass>>failo","77.4*1000*xsec*KFactor_QCD_ggZZ_Nominal*p_Gen_CPStoBWPropRewgt/"+str(f3.Get("ZZTree/Counters").GetBinContent(40)),"")

tp1.Draw("GenHMass>>pass1","77.4*1000*xsec*KFactor_QCD_ggZZ_Nominal*overallEventWeight*L1prefiringWeight/"+str(f1.Get("ZZTree/Counters").GetBinContent(40)),"")
tf1.Draw("GenHMass>>fail1","77.4*1000*xsec*KFactor_QCD_ggZZ_Nominal/"+str(f3.Get("ZZTree/Counters").GetBinContent(40)),"")
tp2.Draw("GenHMass>>pass2","77.4*1000*xsec*KFactor_QCD_ggZZ_Nominal*overallEventWeight*L1prefiringWeight/"+str(f2.Get("ZZTree/Counters").GetBinContent(40)),"")
tf2.Draw("GenHMass>>fail2","77.4*1000*xsec*KFactor_QCD_ggZZ_Nominal/"+str(f3.Get("ZZTree/Counters").GetBinContent(40)),"")
tp3.Draw("GenHMass>>pass3","77.4*1000*xsec*KFactor_QCD_ggZZ_Nominal*overallEventWeight*L1prefiringWeight/"+str(f3.Get("ZZTree/Counters").GetBinContent(40)),"")
tf3.Draw("GenHMass>>fail3","77.4*1000*xsec*KFactor_QCD_ggZZ_Nominal/"+str(f3.Get("ZZTree/Counters").GetBinContent(40)),"")

# tp.Draw("GenHMass>>pass","(140 < GenHMass && GenHMass <= 1500)*PUWeight*genHEPMCweight","")
# tf.Draw("GenHMass>>fail","(140 < GenHMass && GenHMass <= 1500)*PUWeight*genHEPMCweight","")

# tp.Draw("GenHMass>>pass","(105 <= GenHMass && GenHMass <= 140)*PUWeight*genHEPMCweight","")
# tf.Draw("GenHMass>>fail","(105 <= GenHMass && GenHMass <= 140)*PUWeight*genHEPMCweight","")

# tp.Draw("GenHMass>>pass","(124 <= GenHMass && GenHMass <= 126)","")
# tf.Draw("GenHMass>>fail","(124 <= GenHMass && GenHMass <= 126)","")

# tp.Draw("GenHMass>>pass","(124 <= GenHMass && GenHMass <= 126)*PUWeight*genHEPMCweight","")
# tf.Draw("GenHMass>>fail","(124 <= GenHMass && GenHMass <= 126)*PUWeight*genHEPMCweight","")
# t.Draw("p_Gen_GG_BKG_MCFM:GenHMass","","")

# print(hp.Integral()+hf.Integral())

print(tp)

hpt = ROOT.TH1F("tot","",200,180,2000)

hpt.Add(hp1)
hpt.Add(hp2)
hpt.Add(hp3)

# hpt.Add(hf1)
# hpt.Add(hf2)
# hpt.Add(hf3)

print(hp.Integral())
# print(hf.Integral())

print(hpo.Integral())

# print(hp1.Integral())
# print(hp2.Integral())
# print(hp3.Integral())

print(hpt.Integral())

# print(hp.Integral()/(hp.Integral()+hf.Integral()))


hp.SetLineColor(1)
hpo.SetLineColor(2)
hpt.SetLineColor(4)


#hp.Scale(1/hp.Integral())
#hpo.Scale(1/hpo.Integral())
#hpt.Scale(1/hpt.Integral())


# hp.Draw("hist same")
# hpo.Draw("hist")
hp.Draw("hist ")
hpt.Draw("hist same")

# hp1.Draw("hist same")
# hp2.Draw("hist same")
# hp3.Draw("hist same")

# hf1.Draw("hist same")
# hf2.Draw("hist same")
# hf3.Draw("hist same")

c1.SaveAs("/eos/user/l/lkang/Active_Research/POWHEG/TestOut.pdf")
# c1.Draw()

quit()
