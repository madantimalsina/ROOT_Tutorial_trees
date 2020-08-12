### Stage 3:

Let's fit using TF1 functions.
We again recover the trees from the files created in Stage 2, and make histograms of the energy variable. Then we save them in a new file. Execute readTree.C as `root -l ../Stage_2/output_data.root readTree.C`, and the same for `output_MC.root`. Or simply `source processData.sh`


We fit it with a gaussian to find out if they're comparable or not. To do this, execute `root -l histodata.root histoMC.root fitEnergy.C+`

Since they are not, we sample the MC histogram and fit it many times to find out what's the probability that they are so different.

Let's "fix" the MC with the fit to the data. With `applyWeights.C` you can weigh the MC events to resemble the data.
We can then compare the resulting weightedMC.root to ../Stage_2/output_data.root
