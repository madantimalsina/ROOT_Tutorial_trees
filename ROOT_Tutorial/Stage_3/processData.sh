#!/bin/bash


#I want to avoid doing string manipulations in C++, so I just do this:

root -l -q -b ../Stage_2/output_data.root readTree.C
mv output.root histodata.root

root -l -q -b ../Stage_2/output_MC.root readTree.C
mv output.root histoMC.root

#The -q option quits root when the script finishes, the -b option prevents any graphics from being displayed ("batch mode")

root -l histodata.root histoMC.root fitEnergy.C
