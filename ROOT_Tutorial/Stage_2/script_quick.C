{
  TFile *in_MC = new TFile("output_MC.root", "READ");
  TTree* MCtree = (TTree*)in_MC->Get("CESTree");

  TFile *in_data = new TFile("output_data.root", "READ");
  TTree* datatree = (TTree*)in_data->Get("CESTree");

  TH1D* hNe_MC = new TH1D("hNe_MC", ";Extracted electrons;", 71, -0.5, 70.5);
  hNe_MC->SetLineColor(kBlue);
  hNe_MC->SetFillColor(kBlue);
  hNe_MC->SetFillColorAlpha(kBlue, 0.10);

  TH1D* hNe_data = (TH1D*)hNe_MC->Clone();
  hNe_data->SetName("hNe_data");
  hNe_data->SetMarkerStyle(kFullCircle);
  hNe_data->SetMarkerColor(kBlack);
  hNe_data->SetLineColor(kBlack); //line color affects error bars

  TCanvas *c = new TCanvas("c", "", 800, 600);
  MCtree->Draw("Ne>>hNe_MC", "E > 0");
  datatree->Draw("Ne>>hNe_data", "E > 0");

  //To fill Tree on 2D hist (h2 is name of hisogram)
  //MCtree->Draw("Ne:E>>h2", "E > 0", "colz");
  //MCtree->Draw("Y:X>>h2", "E > 0", "colz");


  hNe_MC->Draw("histo"); //draw as a continuous line
  hNe_data->Draw("sameerrors"); //draw markers with error bars on the "same" canvas (i.e. without emptying the canvas before drawing)

  c->WaitPrimitive(); //allows you to stops the program and look at the canvas. Double click the canvas to continue.

  //People don't usually like the "stats box" on the top right, so let's remove it:
  gStyle->SetOptStat(0);

  //Obviously we need to normalise the MC, since we simulated 100x more than "data". So let's do that...

  hNe_MC->Scale(hNe_data->Integral()/hNe_MC->Integral());
  c->WaitPrimitive();

  //You'll notice now that, sadly, the Y axis does not scale properly to accommodate all of the histograms (a few data points are outside the plotted range)

  //We could change the range by hand (by default defined by the first histogram drawn) or we could create a "stack" of histograms. The first case is done like this:

  // hNe_MC->GetYaxis()->SetRangeUser(0, 60);

  //To create a stack of histograms:

  THStack* hs = new THStack("hs", ""); //I don't know all of these things by heart
  //The plotting style is inherited from the histograms that are added
  hs->Add(hNe_MC, "histo");
  hs->Add(hNe_data, "errors");


  TCanvas *cs = new TCanvas("cs", "", 800, 600);

  hs->Draw("nostack"); //we want to compare one to the other, not make one sit on top of the other (nostack vs stack)

  //THStack axes only exist after drawing:
  hs->GetXaxis()->SetTitle("Extracted electrons");
  hs->GetXaxis()->CenterTitle();
  hs->GetYaxis()->SetTitle("Counts/electron");
  hs->GetYaxis()->CenterTitle();

  hs->GetYaxis()->SetRangeUser(0.1, 70);
  hs->SetMinimum(0.1); //the THStack forces the range to include all the points that are non-zero. To avoid this you can set a "minimum value" for the histogram

  gPad->Modified(); //Ensure that changes to axes are propagated to the canvas, gPad represents the current pad

  //Let's also create a legend:

  TLegend *leg = new TLegend(0.593985, 0.786458, 0.892231, 0.892361); //you can modify the legend box with your mouse and then right click + "dump" to see the x1, y1, x2, y2 that you need here
  leg->AddEntry(hNe_data, "FSR1 data", "lp");
  leg->AddEntry(hNe_MC, "CEST MC", "lf");
  leg->Draw();



  //What if we want to make a ratio plot? This would allow us to see how preposterous our simulation actually was in comparison with the data:

  TH1D* hRatio = (TH1D*)hNe_data->Clone();
  hRatio->SetName("hRatio");

  hRatio->Sumw2(); //ROOT will propagate bin uncertainties for us after this!
  hNe_MC->Sumw2(); //Not sure if this is also needed, but it doesn't hurt
  hRatio->Divide(hNe_MC); //uncertainties are propagated in this ratio
  hRatio->GetYaxis()->SetTitle("Data/MC");
  hRatio->GetXaxis()->CenterTitle();
  hRatio->GetYaxis()->CenterTitle();

  c->cd(); //Let's plot it on the first canvas to have a look:

  hRatio->Draw("errors");

  TLine *ratio_line = new TLine(0, 1, 70, 1);
  ratio_line->SetLineStyle(kDashed);
  ratio_line->SetLineColor(kRed);
  ratio_line->Draw("same");

  //We can create a canvas with two panels showing both plots on the top and then the ratio on the bottom.

  //Canvas to contain the pads:
  TCanvas *c2 = new TCanvas("c2", "", 800, 800);
  //Top pad covering a fraction of the canvas (x1, y1, x2, y2)
  TPad *gtop = new TPad("gtop", "gtop", 0., 200./800., 1., 1.); //Fraction of the canvas
  gtop->SetLogy(); //Let's set log scale in the Y axis
  gtop->Draw();
  gtop->cd();
  //Top and bottom plots share x-axes, so let's hide the top's labels:
  gtop->SetBottomMargin(0.02);
  hs->Draw("nostack");
  hs->GetXaxis()->SetLabelOffset(2);

  TPad *gbottom = new TPad("gbottom", "gbottom", 0., 0., 1., 200./800.);
  c2->cd();//Drawing this pad on the canvas, not on gtop!
  gbottom->Draw();
  gbottom->cd();

  hRatio->Draw("errors");
  ratio_line->Draw("same");

  //Since we're sharing x-axis with the top plot, extend this plot upwards
  gbottom->SetTopMargin(0.01);
  //And make some extra space for larger fonts:
  gbottom->SetBottomMargin(0.3);

  //ROOT sets the font sizes relative to the pad height...
  //For some reason...
  //So let's set the fonts of the bottom plot to match the top's...
  hRatio->GetXaxis()->SetTitleSize(600./200. * hRatio->GetXaxis()->GetTitleSize());
  hRatio->GetXaxis()->SetLabelSize(600./200. * hRatio->GetXaxis()->GetLabelSize());

  hRatio->GetYaxis()->SetRangeUser(0, 3.4);

  hRatio->GetYaxis()->SetLabelSize(600./200. * hRatio->GetYaxis()->GetLabelSize());
  hRatio->GetYaxis()->SetDecimals(true); //even integers have decimals
  hRatio->GetYaxis()->SetNdivisions(310, true); //fewer ticks on the axis
  hRatio->GetYaxis()->SetTitleSize(600./200. * hRatio->GetYaxis()->GetTitleSize());
  hRatio->GetYaxis()->SetTitleOffset(0.45); //Title position depends on title size and pad height, so adjust it... (did this manually)


  gtop->cd(); //go back to the top to plot a legend
  leg->SetBorderSize(0); //removes box borders
  leg->SetFillStyle(0); //makes the legend transparent
  leg->Draw();

  c2->Print("Plot_with_ratio.pdf"); //Transparencies only show up in .pdf
  //c2->Print("Plot_with_ratio.C"); //Save as .C to modify plot later, but some of the style may be lost

  //Yes, all of this for a "simple" ratio plot! Once you have it, though, you can just copy and paste...
}
