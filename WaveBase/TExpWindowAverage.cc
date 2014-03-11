#include "TExpWindowAverage.hh"
#include "TMath.h"

//______________________________________________________________________________
// TExpWindowAverage
// 
//   Generates an exponential average window on a waveform.  The length of the
//   exponential is parameterized by the fAlpha parameter. 
//
void TExpWindowAverage::TransformInPlace(TDoubleWaveform& input) const
{
    // Performs the following recursive algorithm:
    //
    // y[i] = alpha*abs(y[i]-avg) + (1-alpha)*y[i-1]
    //

    double average = input.Sum()/input.GetLength();  
    input -= average;
    input[0] = fAlpha*TMath::Abs(input[0]); 
    for (size_t i=1;i<input.size();i++) {
        input[i] = fAlpha*TMath::Abs(input[i]) + (1-fAlpha)*input[i-1]; 
    }
}

