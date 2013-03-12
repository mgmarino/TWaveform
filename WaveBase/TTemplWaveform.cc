#include "TH1D.h"
#include "TROOT.h"
#include "TTemplWaveform.hh"
#include <algorithm>
//______________________________________________________________________________
//
//  TTemplWaveform
//
//  Class describes a generic waveform, including automatic conversion
//  between different types.  The template parameter describes the type of
//  data stored by the waveform.  There are several available typedefs:
//
//    TDoubleWaveform
//    TWaveformFT
//    TIntWaveform
//    TFloatWaveform
//
//  Visualization of the class is handled by GimmeHist() which returns a
//  TH1D.  For example:
//
//    TDoubleWaveform wf; 
//    wf.GimmeHist().Draw();
//
//  It is also possible (and desirable) to pass in a TH1D that the waveform
//  will fill, via:
//
//    TH1D hist;
//    wf.LoadIntoHist(hist);
//
//  This is preferable because it makes it obvious who owns the histogram.
//  TWaveformFT also allows options to be passed in:
//
//    GimmeHist("", "Real") or LoadIntoHist(hist, "Real") // return Hist with Reals 
//    GimmeHist("", "Imag") or LoadIntoHist(hist, "Imag") // return Hist with
//                                                        // Imaginary components 
//    GimmeHist("", "Abs") or LoadIntoHist(hist, "Abs")   // return Hist with
//                                                        // Absolute value 
// 
//  Automatic conversion is also possible.  To go from an integer to a double
//  waveform, just do: 
// 
//  TIntWaveform intWF;
//  TDoubleWaveform doubleWF = intWF;
//
//  One needs to be a little more careful in scripts (ROOT or PyROOT) because
//  ROOT can not resolve the correct function when it parses.  Therefore, to do
//  the same above do:
//
//  TIntWaveform intWF;
//  TDoubleWaveform doubleWF = intWF.Convert();
//
//  This will copy the data into the double waveform which can then be used for
//  calculation.
//
//  For information on how to generate a fourier transform (i.e.
//  TWaveformFT), see TFastFourierTransformFFTW.
//______________________________________________________________________________
ClassImpT(TTemplWaveform, def)

typedef std::complex<double> CDbl;

//______________________________________________________________________________
template<typename _Tp>
TH1D* TTemplWaveform<_Tp>::GimmeHist(const std::string& label, Option_t* opt) const
{
  // Returns a histogram named TWaveformHist_[label/ID]. If the user does not
  // supply a label, the channel of the waveform  is used. The function first
  // searches gROOT for TWaveformHist_[label/ID]. If it is found, that hist
  // is used.  If it is not found, the function returns a newly allocated
  // histogram of the waveform. Either way, it is the user's responsibility to
  // delete the histogram.

  if(label == "") return GimmeHist(GetName(), opt);

  TDirectory* tmpDirectory = gDirectory;
  gROOT->cd();
  TH1D* hist = dynamic_cast< TH1D* >( 
      gROOT->FindObject(("TWaveformHist_" +  label).c_str()) );
  if(hist == NULL) {
    hist = new 
      TH1D(("TWaveformHist_" + label).c_str(), "", 1, 0, 1);
  }
  LoadIntoHist(*hist, opt);
  tmpDirectory->cd();
  return hist;
}			    

//______________________________________________________________________________
template<typename _Tp>
void TTemplWaveform<_Tp>::LoadIntoHist(TH1D& hist, Option_t* /*opt*/) const
{
  // Loads the waveform into a user-supplied hist. This function is safer
  // because it is obvious that the user owns (and therefore must later delete)
  // this histogram.
  hist.Reset();
  hist.SetMaximum(-1111);
  hist.SetMinimum(-1111);
  hist.SetTitle(GetName());
  double bin_width = GetSamplingPeriod()/CLHEP::microsecond;
  hist.SetBins(GetLength(), GetMinTime()/CLHEP::microsecond - 0.5*bin_width, 
                            GetMaxTime()/CLHEP::microsecond - 0.5*bin_width);  
  hist.SetXTitle("t [#mus]");
  hist.SetYTitle("ADC Units");
  hist.SetOption("L");

  for(size_t iSample = 0; iSample < GetLength(); iSample++) {
    hist.SetBinContent(iSample+1, (double)At(iSample));
  }
}

//______________________________________________________________________________
template<typename _Tp>
_Tp TTemplWaveform<_Tp>::Sum( size_t start, size_t stop) const
{
  // Get the sum of the waveform beginning at position start (including start)
  // and ending at stop (*not* including stop).  Default is to sum the entire
  // waveform.  This is equivalent to:
  // 
  // val = 0; 
  // for (i=start;i<stop;i++) val += wf[i];
  if ( stop > GetLength() ) stop = GetLength(); 
  _Tp temp(0);
  for (size_t i=start;i<stop;i++) temp += (*this)[i]; 
  return temp;

}

//______________________________________________________________________________
template<typename _Tp>
_Tp TTemplWaveform<_Tp>::StdDevSquared( size_t start, size_t stop) const
{
  // Get the std dev squared of the waveform beginning at position start and
  // ending at stop.  Default is to sum the entire waveform.  This is
  // equivalent to:
  //
  // val = 0; 
  // avg = Sum(start, stop);
  // for (i=start;i<stop;i++) val += wf[i]*wf[i];
  // return val/(stop - start) - avg*avg;

  if ( stop > GetLength() ) stop = GetLength();
  if (start >= stop) return _Tp(0);
  _Tp temp(0);
  _Tp avg = Sum( start, stop )/(stop-start);
  for (size_t i=start;i<stop;i++) temp += ((*this)[i])*((*this)[i]);
  return temp/(stop-start) - avg*avg;
}
 
//______________________________________________________________________________
template<>
CDbl TTemplWaveform<CDbl>::StdDevSquared( size_t /*start*/, size_t /*stop*/) const
{
  // Disable this for complex numbers.  
  return CDbl();
}

//______________________________________________________________________________
template<>
void TTemplWaveform<CDbl>::LoadIntoHist(TH1D& hist, Option_t* opt) const
{
  // Loads the waveform into a user-supplied hist. This function is safer
  // because it is obvious that the user owns (and therefore must later delete)
  // this histogram.
  hist.Reset();
  hist.SetMaximum(-1111);
  hist.SetMinimum(-1111);
  hist.SetTitle(GetName());
  if (GetLength() <= 1) {
    std::cerr << "Length <= 1, unable to produce histogram" << std::endl;
    return;
  }
  double nyquist_frequency = 0.5*GetSamplingFreq()/CLHEP::megahertz;
  double bin_width = nyquist_frequency/(GetLength() - 1);
  hist.SetBins(GetLength(), -0.5*bin_width, nyquist_frequency + 0.5*bin_width);  
  hist.SetXTitle("f [MHz]");
  hist.SetOption("L");

  TString option = opt;
  option.ToLower();
  if (option.Contains("real")) {
    // Plot reals 
    hist.SetYTitle("Real (ADC Units)");
    for(size_t iSample = 0; iSample < GetLength(); iSample++) {
      hist.SetBinContent(iSample+1, At(iSample).real());
    }
  } else if (option.Contains("imag")) {
    // Plot imag 
    hist.SetYTitle("Imaginary (ADC Units)");
    for(size_t iSample = 0; iSample < GetLength(); iSample++) {
      hist.SetBinContent(iSample+1, At(iSample).imag());
    }
  } else { 
    // Default is abs
    hist.SetYTitle("Abs (ADC Units)");
    for(size_t iSample = 0; iSample < GetLength(); iSample++) {
      hist.SetBinContent(iSample+1, std::abs(At(iSample)));
    }
  }
}

//______________________________________________________________________________
template<typename _Tp>
_Tp TTemplWaveform<_Tp>::GetMaxValue() const
{
  return *(std::max_element(fData.begin(),fData.end()));
}

//______________________________________________________________________________
template<>
CDbl TTemplWaveform<CDbl>::GetMaxValue() const
{
  return CDbl();
}

//______________________________________________________________________________
template<typename _Tp>
_Tp TTemplWaveform<_Tp>::GetMinValue() const
{
  return *(std::min_element(fData.begin(),fData.end()));
}

//______________________________________________________________________________
template<>
CDbl TTemplWaveform<CDbl>::GetMinValue() const
{
  return CDbl();
}

//______________________________________________________________________________
template<typename _Tp>
_Tp TTemplWaveform<_Tp>::InterpolateAtPoint( Double_t time ) const
{
  // Linear interpolation function between two points.  This function returns
  // the value of the first point if the requested time is before the fTOffset
  // or the value of the last point if it's beyond the length of the waveform.
  // If the waveform has 0 length, the function returns 0.
  if ( GetLength() == 0 ) return _Tp(0);
  Double_t frac_entry = ( (time - fTOffset)*fSampleFreq );
  if (frac_entry < 0) return At(0);

  size_t entry = static_cast<size_t>(frac_entry);
  if ( entry >= GetLength()-1 ) return At(GetLength()-1);

  // Now do linear interpolation
  frac_entry -= entry;
  return (_Tp)((1.0-frac_entry)*At(entry) + frac_entry*At(entry+1)); 

} 

//______________________________________________________________________________
template<typename _Tp>
TTemplWaveform<_Tp> TTemplWaveform<_Tp>::SubWaveform(size_t begin, size_t end) const
{
  // get a sub-waveform of this waveform.  begin is the start of the waveform,
  // end is the point *after* which should be saved.  That is, to get a
  // sub-waveform beginning at the 10th point to the end of a 200 point
  // waveform, one would call SubWaveform(10, 200);
  if (begin >= GetLength()) begin = GetLength() - 1;
  if (end > GetLength()) end = GetLength();
  if (begin > end) begin = end;
  TTemplWaveform<_Tp> wf;
  wf.MakeSimilarTo(*this);
  wf.SetData(GetData() + begin, (end-begin));
  return wf;
}

//______________________________________________________________________________
template<typename _Tp>
void TTemplWaveform<_Tp>::Append(const TTemplWaveform<_Tp>& wf) 
{
  if (wf.GetSamplingFreq() != GetSamplingFreq() ) {
    std::cerr << "Cannot append waveforms with different frequencies" << std::endl;
    return;
  }
  fData.insert( fData.end(), wf.GetVectorData().begin(), wf.GetVectorData().end() );
}

//______________________________________________________________________________
// The following are necessary to ensure that the above functions are generated.
template class TTemplWaveform<Int_t>; 
template class TTemplWaveform<UShort_t>; 
template class TTemplWaveform<Double_t>; 
template class TTemplWaveform<Float_t>; 
template class TTemplWaveform<Char_t>; 
template class TTemplWaveform<unsigned long>; 
template class TTemplWaveform<unsigned int>; 
template class TTemplWaveform<std::complex<double> >; 
