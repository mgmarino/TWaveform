#ifndef WAVE_TTemplWaveform_hh
#define WAVE_TTemplWaveform_hh
#ifndef ROOT_TObject
#include "TObject.h"
#endif
#ifndef HEP_SYSTEM_OF_UNITS_H
#include "SystemOfUnits.hh"
#endif
#include <vector> 
#include <string> 
#include <iostream> 
#include <complex> 

namespace TTemplWFUtil {
    template <typename T> const T& identity(const T& val) { return val; }
}

class TH1D;
template<typename _Tp>
class TTemplWaveform : public TObject {
   public:
  
    typedef std::vector<_Tp> VecType;

    _Tp* GetData() 
    { 
      // Return a pointer to the internal data
      return (fData.size() > 0 ) ? &(fData)[0] : NULL; 
    }

    const _Tp* GetData() const 
    { 
      // Return a const pointer to the internal data
      return (fData.size() > 0 ) ? &(fData)[0] : NULL; 
    }

    VecType& GetVectorData() 
    { 
      // Return the vector data
      return fData; 
    }

    const VecType& GetVectorData() const 
    { 
      // Return the vector data
      return fData; 
    }


    template<typename _Tn, class _Function>
    void SetData( const _Tn* aData, size_t numberOfValues, _Function conversion) 
    { 
      // Set the data by inputting an array
      // The following handles all basic types.
      //fData.assign( aData, aData + numberOfValues ); 
      SetLength(numberOfValues);
      for (size_t i = 0; i < numberOfValues; i++) fData[i] = static_cast<_Tp>(conversion(aData[i]));
    }

    template<typename _Tn>
    void SetData( const _Tn* aData, size_t numberOfValues) 
    {
      SetData(aData, numberOfValues, TTemplWFUtil::identity<_Tn>);
    }

    void SetLength( size_t length ) 
    { 
      // Set the length of the waveform
      fData.resize(length); 
    }

    size_t GetLength() const 
    { 
      // Return the length of the waveform
      return fData.size(); 
    }

    size_t size() const
    {
      // Return the length, this gets mapped to __len__ by pyROOT
      return GetLength();
    }


    _Tp  At(size_t i) const 
    { 
      return fData.at(i); 
    } 

    _Tp& operator[](size_t i) 
    { 
      return fData[i]; 
    }

    const _Tp& operator[](size_t i) const 
    { 
      return fData[i]; 
    }

    void Zero(size_t beg = 0, size_t end = (size_t)-1)
    { 
      // Zero the waveform
      size_t e = (end > GetLength()) ? GetLength() : end;
      if (beg >= e) return;
      if (beg == 0 && e == GetLength()) {
        fData.assign(fData.size(), _Tp(0));
      } else {
        for (size_t i=beg;i<e;i++) fData[i] *= 0; 
      }
    }


    template<typename _Op>
    bool IsSimilarTo(const TTemplWaveform<_Op>& other) const
    {
      // Returns true if waveforms are similar, false if not.  At the
      // moment, this only checks the length of the waveform.
      return ( GetLength() == other.GetLength() && 
               GetSamplingFreq() == other.GetSamplingFreq() &&
               GetTOffset() == other.GetTOffset() );
    }

    template<typename _Op>
    void MakeSimilarTo(const TTemplWaveform<_Op>& other)
    {
      // Make this waveform similar to the input.  
      SetLength(other.GetLength());
      SetSamplingFreq(other.GetSamplingFreq());
      SetTOffset(other.GetTOffset());
    }

    bool operator==(const TTemplWaveform<_Tp>& other) const
    {
      // Compare waveforms, only return true if everything is the same.
      return ( IsSimilarTo(other) and 
               std::equal( fData.begin(), fData.end(), other.fData.begin() ) ); 
    }

    
    TTemplWaveform<_Tp>& operator*=(const TTemplWaveform<_Tp>& other) 
    {
      // Vector multiplication.  Waveforms must be similar (defined by
      // IsSimilarTo()) or this function will return without doing anything.
      if (!IsSimilarTo(other)) {
        std::cout << "Waveforms are not similar" << std::endl;
      } else {
        size_t n = GetLength();
        for(size_t i=0; i<n; i++) fData[i] *= static_cast<_Tp>(other[i]);
      }
      return *this;
    }

    //template<typename _Op>
    TTemplWaveform<_Tp>& operator/=(const TTemplWaveform<_Tp>& other) 
    {
      // Vector division.  Waveforms must be similar (defined by IsSimilarTo())
      // or this function will return without doing anything.  Divide by zero
      // is not checked!
      if (!IsSimilarTo(other)) {
        std::cout << "Waveforms are not similar" << std::endl;
      } else {
        size_t n = GetLength();
        for(size_t i=0; i<n; i++) fData[i] /= static_cast<_Tp>(other[i]);
      }
      return *this;
    }

    template<typename _Op>
    TTemplWaveform<_Tp>& operator-=(const TTemplWaveform<_Op>& other) 
    {
      // Vector subtraction.  Waveforms must be similar (defined by
      // IsSimilarTo()) or this function will return without doing anything.
      if (!IsSimilarTo(other)) {
        std::cout << "Waveforms are not similar" << std::endl;
      } else {
        size_t n = GetLength();
        for(size_t i=0; i<n; i++) fData[i] -= static_cast<_Tp>(other[i]);
      }
      return *this;
    }

    template<typename _Op>
    TTemplWaveform<_Tp>& operator+=(const TTemplWaveform<_Op>& other) 
    {
      // Vector addition.  Waveforms must be similar (defined by IsSimilarTo())
      // or this function will return without doing anything.
      if (!IsSimilarTo(other)) {
        std::cout << "Waveforms are not similar" << std::endl;
      } else {
        size_t n = GetLength();
        for(size_t i=0; i<n; i++) fData[i] += static_cast<_Tp>(other[i]);
      }
      return *this;
    }

    TTemplWaveform<_Tp>& operator+=(double value)
    {
      // Scalar addition
      size_t n = GetLength();
      for(size_t i=0; i<n; i++) {
        fData[i] += static_cast<_Tp>(value);
      }
      return *this;
    }

    TTemplWaveform<_Tp>& operator*=(double value) 
    {
      // Scalar multiplication
      size_t n = GetLength();
      for(size_t i=0; i<n; i++) {
        fData[i] = static_cast<_Tp>(value*fData[i]); 
      }
      return *this;
    }

    TTemplWaveform<_Tp>& operator-=(double value) 
    { 
      // Scalar subtraction 
      return (*this) += -value; 
    }

    TTemplWaveform<_Tp>& operator/=(double value) 
    { 
      // Scalar division 
      return (*this) *= (1.0/value); 
    }

    template<class _Function>
    void ApplyToEach(_Function f) 
    {
      // Apply the function or class overloading the operator() to the waveform
      // data.  This is a similar behavior to std::for_each in algorithm.  For
      // example, to apply a sqrt to the data, do something like: 
      //
      // my_waveform.ApplyToEach(TMath::Sqrt);
      //
      // which will apply the sqrt function to each point and save the result
      // in that point.

      size_t n = GetLength();
      for(size_t i=0; i<n; i++) {
        fData[i] = f(static_cast<_Tp>(fData[i])); 
      }
    }

    _Tp Sum( size_t start = 0, 
             size_t stop = (size_t)-1 ) const;

    _Tp StdDevSquared( size_t start = 0, 
                       size_t stop = (size_t)-1) const;

  public:
    TTemplWaveform() : fSampleFreq(CLHEP::megahertz),
                         fTOffset(0.0) {}
    TTemplWaveform( const _Tp* aData, size_t length ) :
      fSampleFreq(CLHEP::megahertz), fTOffset(0.0) 
    {
      SetData<_Tp>(aData, length); 
    }

    template<typename _Op>
    TTemplWaveform( const TTemplWaveform<_Op>&  aWF ) :
      fSampleFreq(aWF.GetSamplingFreq()), 
      fTOffset(aWF.GetTOffset()) 
    {
      // Copy constructor, make a new waveform based on another waveform
      SetData<_Op>(aWF.GetData(), aWF.GetLength());
    }

    TTemplWaveform( const TObject& aWF );
    TTemplWaveform<_Tp>& operator=( const TObject& aWF);

    template<typename _Op>
    TTemplWaveform<_Tp>& operator=( const TTemplWaveform<_Op>&  aWF ) 
    {
      // Assignment operator, make a new waveform based on another waveform
      SetData<_Op>(aWF.GetData(), aWF.GetLength());
      fSampleFreq = aWF.GetSamplingFreq();
      fTOffset = aWF.GetTOffset();
      return *this;
    }

    virtual ~TTemplWaveform() {}
 
    //virtual Draw(Option_t *opt);
    virtual TH1D* GimmeHist(const std::string& label="", Option_t* opt = "") const;
    virtual void LoadIntoHist(TH1D& hist, Option_t* opt = "") const;

    template<typename _Op>
    TTemplWaveform<_Op> Convert() const
    {
      TTemplWaveform<_Op> wf(*this);
      return wf;
    }

    void ConvertFrom(const TObject& aWF, Option_t* opt = "");
    
    TTemplWaveform<_Tp> SubWaveform(size_t begin = 0, size_t end = (size_t)-1) const;
    void Append(const TTemplWaveform<_Tp>& wf); 

    Double_t GetSamplingFreq() const 
    { 
      // Returns sampling frequency (CLHEP units)
      return fSampleFreq;
    }
    Double_t GetSamplingPeriod() const 
    { 
      // Returns sampling period (CLHEP units)
      return 1./fSampleFreq;
    }
    void SetSamplingFreq(double freq) 
    { 
      // Sets sampling frequency (CLHEP units)
      fSampleFreq = freq;
    }
    void SetSamplingPeriod(double per) 
    { 
      // Sets sampling period (CLHEP units)
      fSampleFreq = 1./per;
    }

    Double_t GetTOffset() const 
    { 
      // Return the offset time
      return fTOffset;
    }
    void SetTOffset(double time) 
    { 
      // Sets the offset time 
      fTOffset = time;
    }
    Double_t GetMaxTime() const
    {
      // Get maximum time of the waveform
      return GetTimeAtIndex(GetLength()); 
    }
    Double_t GetMinTime() const
    {
      // Get the minimum time of the waveform
      return GetTOffset();
    }
    virtual _Tp GetMaxValue() const; // Get the maximum of the waveform. Returns zero for complex waveforms
    virtual _Tp GetMinValue() const; // Get the minimum of the waveform. Returns zero for complex waveforms

    virtual _Tp InterpolateAtPoint( Double_t time ) const;  
    virtual TTemplWaveform<_Tp> Refine(Double_t NewFrequency) const;

    virtual size_t GetIndexAtTime(Double_t Time) const;
    virtual Double_t GetTimeAtIndex(size_t Index) const;

    //////////////////////////////////////////////////////////
    // Iterators
    typedef typename VecType::iterator Iter;
    typedef typename VecType::const_iterator CIter;
    Iter  begin()       { return fData.begin(); }
    CIter begin() const { return fData.begin(); }
    Iter  end()         { return fData.end(); }
    CIter end()   const { return fData.end(); }


  protected:
    VecType   fData;              // data  : Waveform data 
    Double_t  fSampleFreq;        // Sampling frequency
    Double_t  fTOffset;           // Time offset 

  ClassDefT(TTemplWaveform,1)

};

typedef TTemplWaveform<Double_t> TDoubleWaveform; 
typedef TTemplWaveform<Int_t>    TIntWaveform;
typedef TTemplWaveform<UShort_t> TUShortWaveform;
typedef TTemplWaveform<Short_t>  TShortWaveform;
typedef TTemplWaveform<Float_t>  TFloatWaveform;
typedef TTemplWaveform<size_t>   TLocationWaveform;   // Should not be saved because size_t is system dependent
typedef TTemplWaveform<Char_t>   TBoolWaveform;   
typedef TTemplWaveform<std::complex<double> >  TWaveformFT;

template <typename _Tp>
size_t TTemplWaveform<_Tp>::GetIndexAtTime(Double_t Time) const
{
  // Return the index of the element corresponding to time Time.  If Time lies
  // between two elements of the waveform, we return the index of the earlier
  // one.  If Time does not lie within the waveform, log an error and return
  // GetLength().
  if(Time < GetTOffset()) {
    std::cerr << "Input time precedes the waveform.  Returning GetLength()." << std::endl;
    return GetLength();
  }
  if(Time >= GetSamplingPeriod()*GetLength() + GetTOffset()) {
    std::cerr << "Input time follows the waveform.  Returning GetLength()." << std::endl;
    return GetLength();
  }
  return static_cast<size_t>( (Time - GetTOffset())*GetSamplingFreq() );
}

template <typename _Tp>
Double_t TTemplWaveform<_Tp>::GetTimeAtIndex(size_t Index) const
{
  // Return the time corresponding to the element at index Index.  If Index >
  // GetLength(), log an error and return the time that would correspond to
  // index GetLength().
  if(Index > GetLength()) {
    std::cerr << "Input index follows the waveform.  Returning a time that follows the waveform as well." << std::endl;
    return GetSamplingPeriod()*GetLength() + GetTOffset();
  }
  return GetSamplingPeriod()*Index + GetTOffset();
}

template <typename _Tp>
TTemplWaveform<_Tp> TTemplWaveform<_Tp>::Refine(Double_t NewFrequency) const
{
  // Return a new waveform where we have interpolated between points (by calling InterpolateAtPoint)
  // to get a waveform of frequency Frequency.  This can be either more or less than the frequency of this waveform.
  // If the new frequency doesn't fit cleanly into the length of this waveform, the end is truncated.
  // Frequency should be given in CLHEP units.
  TTemplWaveform<_Tp> refinement;
  refinement.SetTOffset(GetTOffset());
  refinement.SetSamplingFreq(NewFrequency);
  refinement.SetLength(static_cast<size_t>(NewFrequency * GetLength()/GetSamplingFreq()));

  for(size_t i = 0; i < refinement.GetLength(); i++) {
    refinement[i] = InterpolateAtPoint(refinement.GetTimeAtIndex(i));
  }

  return refinement;
}

#endif /* WAVE_TTemplWaveform_hh */
