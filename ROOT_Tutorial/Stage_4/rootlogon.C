{
   gROOT->ProcessLine(".L SetLZStyle.h");
   gROOT->ProcessLine(".L ROOTutils.C");

   SetLZStyle(1);
   gROOT->Reset();
   gROOT->SetStyle("LZ");
   gROOT->ForceStyle();


}
