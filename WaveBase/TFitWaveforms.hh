/**                                                            
 *      
 * CLASS DECLARATION:  TFitWaveforms.hh
 *
 * DESCRIPTION: 
 *
 * Fit waveforms by getting the best toffset
 *
 * AUTHOR: M. Marino
 * CONTACT: 
 * FIRST SUBMISSION: 
 * 
 * REVISION:
 * 
 */

#ifndef WAVE_TFitWaveforms_hh
#define WAVE_TFitWaveforms_hh

#ifndef WAVE_TVWaveformTransformer_hh
#include "TVWaveformTransformer.hh" 
#endif

class TFitWaveforms : public TVWaveformTransformer
{
  public:
    TFitWaveforms() : TVWaveformTransformer("TFitWaveforms"), 
      fOffset(0.0), fOffsetError(0.0), fSigma(1.0), 
      fMyWF(0) {}
  
    virtual bool IsInPlace() const { return true; }

    void SetFitWaveform(const TDoubleWaveform& otherWf) { fOtherWF = otherWf; }
    const TDoubleWaveform& GetFitWaveform() const { return fOtherWF; }

    void SetInitialOffset(double offset) { fOffset = offset; }
    void SetSigma(double sigma) { fSigma = sigma; }
    double GetOffset() const { return fOffset; }
    double GetOffsetError() const { return fOffsetError; }

    static double CalculateChiSquareWF(const TDoubleWaveform& one, const TDoubleWaveform& two, double sigma=1.0);
    
  protected:
    virtual void TransformInPlace(TDoubleWaveform& input) const;
    double CalculateChiSquare(const double* x) const;
    mutable double fOffset;
    mutable double fOffsetError;
    double fSigma;
    mutable TDoubleWaveform fOtherWF;
    mutable const TDoubleWaveform* fMyWF;
  
};

#endif /* WAVE_TFitWaveforms_hh */
