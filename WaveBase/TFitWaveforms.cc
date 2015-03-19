#include "TFitWaveforms.hh"
#include "TMinuitMinimizer.h" 
#include "Math/Functor.h" 

//______________________________________________________________________________
// TFitWaveforms
// 
//   Fits two waveforms together 
//   exponential is parameterized by the fAlpha parameter. 
//

void TFitWaveforms::TransformInPlace(TDoubleWaveform& input) const
{
    // Performs the  fit
    //
    fMyWF = &input;
    
    TMinuitMinimizer minimizer;
    ROOT::Math::Functor func = ROOT::Math::Functor(this, 
            &TFitWaveforms::CalculateChiSquare,         
            1); 
    minimizer.SetFunction(func);
    double maxTime =  fMyWF->GetTimeAtIndex(fMyWF->GetLength()-1);
    minimizer.SetLimitedVariable(0, "Time offset", fOffset, 1, -maxTime, maxTime); 
    minimizer.Minimize();
    fOffset = minimizer.X()[0];
    fOffsetError = minimizer.Errors()[0];
}

double TFitWaveforms::CalculateChiSquareWF(const TDoubleWaveform& one, const TDoubleWaveform& two, double sig)
{
    double retVal = 0.0;
    for (size_t i=0;i<one.GetLength();i++) {
      double atime = one.GetTimeAtIndex(i); 
      double aval = one[i] - two.InterpolateAtPoint(atime);
      retVal += aval*aval; 
    }
    return retVal/(sig*sig);
}

double TFitWaveforms::CalculateChiSquare(const double* x) const
{
    fOtherWF.SetTOffset(x[0]); 
    return CalculateChiSquareWF(*fMyWF, fOtherWF, fSigma);
}
