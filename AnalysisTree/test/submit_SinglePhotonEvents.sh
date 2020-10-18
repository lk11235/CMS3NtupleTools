#!/bin/bash

date=$1
period=$2
prodVersion=$3
script=produceSinglePhotonCR.cc
function=getTrees
jobdate="SinglePhotonEvents_${date}"
arguments='"<strSampleSet>","<period>","<prodVersion>","<strdate>",<ichunk>,<nchunks>,<theGlobalSyst>,<computeMEs>,<applyPUIdToAK4Jets>,<applyTightLeptonVetoIdToAK4Jets>,<use_MET_Puppi>,<use_MET_XYCorr>,<use_MET_JERCorr>,<use_MET_ParticleMomCorr>,<use_MET_p4Preservation>,<use_MET_corrections>'
arguments="${arguments/<strdate>/$date}"
arguments="${arguments/<prodVersion>/$prodVersion}"
arguments="${arguments/<computeMEs>/true}"
arguments="${arguments/<applyPUIdToAK4Jets>/true}"
arguments="${arguments/<applyTightLeptonVetoIdToAK4Jets>/false}"
arguments="${arguments/<use_MET_Puppi>/false}"
arguments="${arguments/<use_MET_XYCorr>/true}"
arguments="${arguments/<use_MET_JERCorr>/true}"
arguments="${arguments/<use_MET_ParticleMomCorr>/true}"
arguments="${arguments/<use_MET_p4Preservation>/true}"
arguments="${arguments/<use_MET_corrections>/true}"

declare -a dataPeriods=( $period )
declare -a DataSampleList=( )
declare -a MCDataPeriods=( $period )
declare -a MCSysts=( sNominal )
declare -i dataYear

DataSampleList=( Run )

if [[ "$period" == "2018"* ]]; then
  dataYear=2018
  if [[ "$period" == "2018" ]]; then
    dataPeriods=( 2018A 2018B 2018C 2018D )
  fi
elif [[ "$period" == "2017"* ]]; then
  dataYear=2017
  if [[ "$period" == "2017" ]]; then
    dataPeriods=( 2017B 2017C 2017D 2017E 2017F )
  fi
elif [[ "$period" == "2016"* ]]; then
  dataYear=2016
  if [[ "$period" == "2016" ]]; then
    dataPeriods=( 2016B 2016C 2016D 2016E 2016F 2016G 2016H )
  fi
fi

MCDataPeriods=( $period )


csvfile="skimSamples_${dataYear}.csv"
for sample in $(readCMS3SkimSamplesFromCSV.py --csv=${csvfile} --sim --tree_req="SinglePhoton"); do
  sampleDir=${sample//MINIAODSIM}

  echo "====="
  skimdir="/hadoop/cms/store/user/usarica/Offshell_2L2Nu/Skims/${prodVersion}/${sampleDir}"
  if [[ ! -d $skimdir ]]; then
    echo "$skimdir does not exist"
    continue
  fi

  let nfiles=$(ls $skimdir | grep -e ".root" | wc -l)
  echo "$sampleDir has $nfiles files"

  nchunks=$((nfiles / 4))
  nfeff=$((nchunks * 4))
  if [[ $nfeff -lt $nfiles ]];then
    nchunks=$((nchunks + 1))
  fi
  echo " - There will be $nchunks chunks"
  if [[ $nchunks -eq 0 ]]; then
    echo "Oh-uh..."
  fi

  for dataperiod in "${MCDataPeriods[@]}"; do
    for syst in "${MCSysts[@]}"; do

      let ichunk=0
      while [[ $ichunk -lt $nchunks ]]; do
        strargs="${arguments}"
        strargs="${strargs/<strSampleSet>/${sample}}"
        strargs="${strargs/<ichunk>/${ichunk}}"
        strargs="${strargs/<nchunks>/${nchunks}}"
        strargs="${strargs/<theGlobalSyst>/${syst}}"
        strargs="${strargs/<period>/$dataperiod}"


        submitCMS3AnalysisProduction.sh script="${script}" function="${function}" arguments="${strargs}" date="${jobdate}"
        let ichunk=$ichunk+1
      done

    done
  done
  
  echo "====="
done

for spl in "${DataSampleList[@]}"; do
  for dataperiod in "${dataPeriods[@]}"; do
    sample="${spl}${dataperiod}"

    strargs="${arguments}"
    strargs="${strargs/<strSampleSet>/${sample}}"
    strargs="${strargs/<ichunk>/0}"
    strargs="${strargs/<nchunks>/0}"
    strargs="${strargs/<theGlobalSyst>/sNominal}"
    strargs="${strargs/<period>/$dataperiod}"

    submitCMS3AnalysisProduction.sh script="${script}" function="${function}" arguments="${strargs}" date="${jobdate}"
  done
done