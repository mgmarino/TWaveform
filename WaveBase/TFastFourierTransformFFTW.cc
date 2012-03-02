#include "TFastFourierTransformFFTW.hh"
#ifdef USE_ROOT_FFTW
// The following is to avoid using ROOT's cludgy interface.  fftw_plan_dft_r2c
// is defined in libFFTW, but we also need the header information (copied
// here).  This will be modified when Vladimir completes the autoconf setup. 
extern "C" {
#if !defined(FFTW_NO_Complex) && defined(_Complex_I) && defined(complex) && defined(I)
#  define FFTW_DEFINE_COMPLEX(R, C) typedef R _Complex C
#else
#  define FFTW_DEFINE_COMPLEX(R, C) typedef R C[2]
#endif

#define FFTW_EXTERN extern
#define FFTW_DEFINE_API(X, R, C)                                           \
                                                                           \
FFTW_DEFINE_COMPLEX(R, C);                                                 \
                                                                           \
typedef struct X(plan_s) *X(plan);                                         \
                                                                           \
FFTW_EXTERN void X(execute)(const X(plan) p);                              \
                                                                           \
FFTW_EXTERN void X(destroy_plan)(X(plan) p);                               \
                                                                           \
FFTW_EXTERN X(plan) X(plan_dft_c2r)(int rank, const int *n,                \
                        C *in, R *out, unsigned flags);                    \
FFTW_EXTERN X(plan) X(plan_dft_r2c)(int rank, const int *n,                \
                        R *in, C *out, unsigned flags);                    

#define FFTW_CONCAT(prefix, name) prefix ## name
#define FFTW_MANGLE_DOUBLE(name) FFTW_CONCAT(fftw_, name)
#define FFTW_ESTIMATE (1U << 6)

FFTW_DEFINE_API(FFTW_MANGLE_DOUBLE, double, fftw_complex)
}
// End include FFTW header
#else
#ifdef HAVE_FFTW
#include "fftw3.h"
#endif
#endif

//______________________________________________________________________________
// This class implements the fftw3 and takes the data from an TWaveform and
// FFT's it into a TWaveformFT.  Given that the data is real, one can take
// advantage of this and improve speed and memory usage.  This class performs
// the FFT operation on the data of the TWaveform and TWaveformFT classes.
//
// To use this class, you can do the following:
//
//   TDoubleWaveform wf; 
//   TWaveformFT wfFT;
//   TFastFourierTransformFFTW::GetFFT(wf.GetLength()).PerformFFT(wf, wfFT);
//
// Of course, you can save a reference to the FFT you want (assuming that all
// remaining FFTs will always use this logical length.  (Logical length means
// the length of the real data.  Complex data is stored in an array of size
// length/2 + 1.)  A reference/pointer can be saved as the follwoing:
//
//   TFastFourierTransformFFTW& fft = TFastFourierTransformFFTW::GetFFT(2048);
//      
// CLASS IMPLEMENTATION:  TFastFourierTransformFFTW.cc
//
// AUTHOR: M. Marino 
// CONTACT: 
// FIRST SUBMISSION: 
// 
// REVISION:
//
//______________________________________________________________________________


TFastFourierTransformFFTW::FFTMap TFastFourierTransformFFTW::fMap;
TFastFourierTransformFFTW::TFastFourierTransformFFTW(size_t length) : 
  fTheForwardPlan(NULL),
  fTheInversePlan(NULL),
  fLength(length)
{
}

//______________________________________________________________________________
TFastFourierTransformFFTW::TFastFourierTransformFFTW(const TFastFourierTransformFFTW& other) : 
  fTheForwardPlan(NULL),
  fTheInversePlan(NULL),
  fLength(other.fLength)
{
  // Copy constructor.  Do not copy the plans of the other FFT
}

//______________________________________________________________________________
TFastFourierTransformFFTW& 
  TFastFourierTransformFFTW::operator=(const TFastFourierTransformFFTW& other) 
{
#ifdef HAVE_FFTW
  if (fTheForwardPlan != NULL) fftw_destroy_plan((fftw_plan)fTheForwardPlan);
  if (fTheInversePlan != NULL) fftw_destroy_plan((fftw_plan)fTheInversePlan);
#endif
  fTheForwardPlan = NULL;
  fTheInversePlan = NULL;
  fLength = other.fLength;
  return *this;
}

//______________________________________________________________________________
TFastFourierTransformFFTW& TFastFourierTransformFFTW::GetFFT( size_t length )
{
  FFTMap::iterator iter;
  if ( (iter = fMap.find(length)) == fMap.end() ) {
      iter = fMap.insert(std::make_pair(length,TFastFourierTransformFFTW(length))).first;
  }
  return iter->second;
}

//______________________________________________________________________________
TFastFourierTransformFFTW::~TFastFourierTransformFFTW()
{
#ifdef HAVE_FFTW
  if (fTheForwardPlan != NULL) fftw_destroy_plan((fftw_plan)fTheForwardPlan);
  if (fTheInversePlan != NULL) fftw_destroy_plan((fftw_plan)fTheInversePlan);
#endif
}

//______________________________________________________________________________
#ifdef HAVE_FFTW
void TFastFourierTransformFFTW::PerformFFT( const TDoubleWaveform& aWaveform, 
                                            TWaveformFT& aWaveformFT )
#else
void TFastFourierTransformFFTW::PerformFFT( const TDoubleWaveform&, 
                                            TWaveformFT& )
#endif
{
  // Performs an Real-to-complex FFT on aWaveform, returning the data in
  // aWaveformFT.  aWaveformFT will be resized to be aWaveform.GetLength()/2 +
  // 1.  That is, since a DFT of real data generates hermitian data, only half
  // of this data must be stored.  For more details see http://www.fftw.org

#ifdef HAVE_FFTW
  if ( fLength != aWaveform.GetLength() ) {
    std::cerr << "Called without correct length" << std::endl;
    return;
  }
  if ( fTheForwardPlan == NULL ) {
    fWF.SetLength(fLength);
    fFT.SetLength(fLength/2 + 1);
    const int temp = fLength;
    fTheForwardPlan = fftw_plan_dft_r2c( 1, &temp, 
           &(fWF[0]), 
           reinterpret_cast<fftw_complex*>(&(fFT[0])), 
           FFTW_ESTIMATE );
  }
  fWF = aWaveform;
  fftw_execute( (fftw_plan)fTheForwardPlan );
  aWaveformFT = fFT;
  aWaveformFT.SetSamplingFreq(aWaveform.GetSamplingFreq());
#else
  std::cerr << "Compiled without FFTW3" << std::endl;
#endif
  
}

//______________________________________________________________________________
#ifdef HAVE_FFTW
void TFastFourierTransformFFTW::PerformInverseFFT( TDoubleWaveform& aWaveform,  
                                                   const TWaveformFT& aWaveformFT )
#else
void TFastFourierTransformFFTW::PerformInverseFFT( TDoubleWaveform&,  
                                                   const TWaveformFT& )
#endif
{
  // Performs an Complex-to-Real inverse FFT on aWaveformFT, returning the data
  // in aWaveform.  aWaveform will be resized to be the logical size, n,  of
  // this transformation (defined when TFastFourierTransformFFTW::GetFFT was
  // called).  The format of aWaveformFT is such that it contains 0 -> n/2
  // values (DC to nyquist frequency) and not a total array of n complex
  // values.  This is because the complex FT of real data is hermitian, so the
  // latter half of the data (n/2 -> n) would be redundant.  For more details
  // see http://www.fftw.org

#ifdef HAVE_FFTW
  if ( fLength/2 + 1 != aWaveformFT.GetLength() ) {
    std::cerr << "Called without correct length" << std::endl;
    return;
  }
  if ( fTheInversePlan == NULL ) {
    fWF.SetLength(fLength);
    fFT.SetLength(fLength/2 + 1);
    const int temp = fLength;
    fTheInversePlan = fftw_plan_dft_c2r( 1, &temp, 
           reinterpret_cast<fftw_complex*>(&(fFT[0])), 
           &(fWF[0]), 
           FFTW_ESTIMATE );
  }

  fFT = aWaveformFT;
  fftw_execute( (fftw_plan)fTheInversePlan );
  aWaveform = fWF;
  aWaveform.SetSamplingFreq(aWaveformFT.GetSamplingFreq());
#else
  std::cerr << "Compiled without FFTW3" << std::endl;
#endif

}

