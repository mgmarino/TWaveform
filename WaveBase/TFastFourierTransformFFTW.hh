/**        
 *      
 * CLASS DECLARATION:  TFastFourierTransformFFTW.hh
 *
 * DESCRIPTION: 
 *
 * TFastFourierTransformFFTW handles the FT of a waveform.  This is very useful especially when it 
 * comes to the convolution of different functions with a waveform.  The current
 * library used is the fftw3 library: see http://www.fftw.org/
 *
 * AUTHOR: M. Marino 
 * CONTACT: 
 * FIRST SUBMISSION: 
 * 
 * REVISION:
 * 
 */

#ifndef _WAVE_TFastFourierTransformFFTW_HH
#define _WAVE_TFastFourierTransformFFTW_HH

#include "TTemplWaveform.hh"
#include <map>

class TFastFourierTransformFFTW 
{
  public:
    
    typedef std::map<size_t, TFastFourierTransformFFTW> FFTMap;
    // Following are necessary to store TFastFourierTransformFFTW in a map
    friend struct std::pair<const size_t, TFastFourierTransformFFTW>;
    friend struct std::pair<size_t, TFastFourierTransformFFTW>;

    // Perform a Fourier Transform on the data in aWaveform, storing it in 
    // aWaveformFT. 
    virtual void PerformFFT( const TDoubleWaveform& aWaveform, TWaveformFT& aWaveformFT );

    // Perform an inverse Fourier Transform on the data in aWaveformFT, storing 
    // it in  aWaveformFT. 
    virtual void PerformInverseFFT( TDoubleWaveform& aWaveform,  const TWaveformFT& aWaveformFT );
    static TFastFourierTransformFFTW& GetFFT(size_t length); 
    
  protected:
    void *fTheForwardPlan; 
    void *fTheInversePlan; 
    TWaveformFT fFT;
    TDoubleWaveform fWF;
    size_t fLength;
    static FFTMap fMap;
    TFastFourierTransformFFTW(size_t length);
    TFastFourierTransformFFTW(const TFastFourierTransformFFTW&);
    TFastFourierTransformFFTW& operator=(const TFastFourierTransformFFTW&);
    virtual ~TFastFourierTransformFFTW();
  private:
    TFastFourierTransformFFTW();
    
 
};

#endif /* _WAVE_TFastFourierTransformFFTW_HH */
