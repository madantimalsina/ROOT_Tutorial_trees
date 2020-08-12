### Stage 2:

Now let's generate two data samples and compare them to each other. First a high stistics samples with `fGauss` and a certain mean and sigma, and then a lower statistics sample with slightly different mean and sigma. Save both as trees in a `TFile` and then use a separate script to explore the data.

In script_quick.C, we recover the trees from their respective files and use `tree->Draw()` to quickly create histograms. This works because the number of events is not too high and we only need to `Draw()` once. If we wanted to produce several histograms out of the same tree, it is always quicker to write a traditional loop (but not always necessary for just a few events).

Ideally you would save the histograms in a file and then only re-run `tree->Draw` if you want to do something different. I don't do this here due to laziness, and every time I change the style of the plot I re-run the whole script. Don't do this in real life...

A few plots are created, including a plot with two panels and a ratio on the lower panel. This is harder than it should be! But at least you can copy and paste my example now.