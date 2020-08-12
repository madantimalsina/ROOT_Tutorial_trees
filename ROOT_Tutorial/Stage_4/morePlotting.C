{
  //Execute this as root -l fitOutput.root morePlotting.C

  TH2D* hENe = (TH2D*)_file0->Get("hENe");
  TGraphErrors* gNe = (TGraphErrors*)_file0->Get("gNe");
  TF1* fQy = (TF1*)_file0->Get("fQy");
  TF1* fQy_true = (TF1*)_file0->Get("fQy_true");
  TF1* fQy_graph = (TF1*)_file0->Get("fQy_graph");
  gStyle->SetOptStat(0); //remove stats box from plots

  TCanvas *c = new TCanvas("c", "", 800, 600);
  c->SetRightMargin(0.15);//increase by 0.05 to fit a Z axis title
  hENe->GetYaxis()->SetRangeUser(0, 70);
  hENe->Draw("colz");
  hENe->GetZaxis()->SetTitle("Counts per bin");

  fQy_true->SetLineWidth(2);
  fQy_true->SetLineStyle(9);
  fQy_true->Draw("same");

  fQy->SetLineWidth(2);
  fQy->Draw("same");

  fQy_graph->SetLineWidth(2);
  fQy_graph->SetLineColor(kViolet);
  // fQy_graph->Draw("same");


  TLegend *leg = new TLegend(0.0350877, 0.694444, 0.452381, 0.895833);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->AddEntry(hENe, "TSR 2019 8.3 #sqrt{M} kg day", "");
  leg->Draw();

  TText* t_true = new TText(17.4837, 39.0061, "CEST fit");
  t_true->SetTextAngle(321.044);
  t_true->SetTextColor(kGreen+3);
  t_true->SetTextSize(t_true->GetTextSize()/1.5);
  t_true->Draw();

  TText* t_fit = new TText(17.2498, 46.9314, "This work");
  t_fit->SetTextAngle(326.083);
  t_fit->SetTextColor(kRed);
  t_fit->SetTextSize(t_fit->GetTextSize()/1.5);
  t_fit->Draw();


  TCanvas *c2 = new TCanvas("c2", "", 800, 600);
  
  gNe->SetMarkerStyle(kOpenCircle);
  gNe->SetMarkerSize(0.7);
  gNe->SetMarkerColor(kBlue);
  //  gNe->SetLineWidth(2);
  gNe->Draw("AP"); //A: draw new axes, P: draw markers, no lines joining, etc
  fQy_graph->Draw("same");
}
