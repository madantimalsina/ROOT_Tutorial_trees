### Stage 1:

Some ROOT has been used in this main.cxx file. For this, ROOT classes have to be `#include`d. The Makefile has been changed to allow for this. Have a look at the changes and see if they make sense to you. It's OK that they don't, though...

A histogram `TH1D` of the number of electrons has been created. It is filled only if the number of electrons passes a certain threshold. Then it is saved to a `TFile`. Notice that I specified the binning starting from -0.5 to 100.5, just to get the bins to centre at the integers. This is not very important, but since we're dealing with integer x-axis... 

A `TTree` was also created, to store all of the information event by event. In this case, I save the energy `E` and the number of electrons `Ne`. When `tree->Fill()` is called, the values stored in `thisE` and `thisNe` are saved.

Finally some functionality of the distribution class `TF1` is demonstrated. The CEST library only allows to draw random energy samples from a flat distribution, so I created other distributions with `TF1`: a normalised gaussian `fGauss` with mean 7.5 and sigma 2.5, made by calling the native `gausn` in ROOT, and a "modified gaussian" `fModGauss`, which uses a lambda function (not a C++ expert, please correct me) created on the fly inside the `TF1` constructor. This function returns the previous `fGauss` unless the `x < 3` condition is met, in which case a different function is output. The number of parameters must be specified for this `TF1` constructor to work.

Then inside the loop a random sample is drawn from a `TF1` using `GetRandom()` and used as an input to CEST `c->GetNe(thisE);`.

The histogram and tree are then written into the file and the file is closed. We could also save the `TF1` functions. The native `gausn` function can be recovered and used normally from a TFile, but `fModGauss` would just save a frozen version of the curve, the parameters could not be changed if it is read from a TFile.