/**                                                            
 *      
 * CLASS DECLARATION:  TExpWindowAverage.hh
 *
 * DESCRIPTION: 
 *
 * Exponential window average.  C++ implementation of python code written by M.
 * Sturm and Christian. 
 *
 * AUTHOR: M. Marino
 * CONTACT: 
 * FIRST SUBMISSION: 
 * 
 * REVISION:
 * 
 */

#ifndef WAVE_TExpWindowAverage_hh
#define WAVE_TExpWindowAverage_hh

#ifndef WAVE_TVWaveformTransformer_hh
#include "TVWaveformTransformer.hh" 
#endif

class TExpWindowAverage : public TVWaveformTransformer
{
  public:
    TExpWindowAverage() : TVWaveformTransformer("TExpWindowAverage"), fAlpha(0.5) {}
  
    virtual bool IsInPlace() const { return true; }

    void SetAlpha(double alpha) { fAlpha = alpha; }
    
  protected:
    virtual void TransformInPlace(TDoubleWaveform& input) const;
    double fAlpha;
  
};

#endif /* WAVE_TExpWindowAverage_hh */
