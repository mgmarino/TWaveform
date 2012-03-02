/**                                                            
 *      
 * CLASS DECLARATION:  TVWaveformTransformer.hh
 *
 * DESCRIPTION: 
 *
 * Abstract class handling transformations, processing of waveforms
 *
 * AUTHOR: M. Marino
 * CONTACT: 
 * FIRST SUBMISSION: 
 * 
 * REVISION:
 * 
 */

#ifndef WAVE_TVWaveformTransformer_hh
#define WAVE_TVWaveformTransformer_hh

#include <string> 
#ifndef WAVE_TTemplWaveform_hh
#include "TTemplWaveform.hh" 
#endif

class TVWaveformTransformer
{
  public:
    // We require the derived classes to define their names since this is an important way to distinguish between
    // them.  
    TVWaveformTransformer( const std::string& aTransformationName ) :
      fName(aTransformationName)
      { } 
    virtual ~TVWaveformTransformer() { }
  
    virtual bool IsInPlace() const = 0;
    virtual bool IsOutOfPlace() const { return !IsInPlace(); }

    virtual void Transform(TDoubleWaveform* input, TDoubleWaveform* output = NULL) const;
    const std::string& GetStringName() const { return fName; }
    const char* GetName() const { return fName.c_str(); }
    
  protected:
    virtual void TransformInPlace(TDoubleWaveform& input) const;
    virtual void TransformOutOfPlace(const TDoubleWaveform& input, TDoubleWaveform& output) const;
  
  private:
    // Make the default constructor private to force usage of the other constructor.
    TVWaveformTransformer();

    std::string fName; // Name of the transformation class.
    mutable TDoubleWaveform fTmpWaveform; // Temporary waveform, use only by base class. 
};

#endif /* WAVE_TVWaveformTransformer_hh */
