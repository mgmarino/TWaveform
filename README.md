
TWaveform
=========

[ROOT](root.cern.ch)-based framework to save waveforms.  This is work based on my experience
with Majorana and EXO in developing how to save and deal with waveforms in
[ROOT](root.cern.ch).   

M. Marino, 2012

Installation:

Generally,

./configure
make [-j#]

Usage:
------

```python
# In python:
import ROOT
ROOT.gSystem.Load("/path/to/libWaveWaveBase")
# numpy is important when converting to/from numpy
import numpy

# create a waveform
wf = ROOT.TDoubleWaveform()

# generate from a numpy waveform
arr =  numpy.array([0.1*x for x in range(1000)])
wf = ROOT.TDoubleWaveform(arr, len(arr))

# convert to numpy waveform 
# following is SLOW
arr = numpy.array(wf)

# following is MUCH faster, requires no copy
arr = numpy.frombuffer(wf.GetData(),count=len(wf))

# Save to disk
afile = ROOT.TFile.OpenFile("new_file.root", "recreate")
wf.SetNameTitle("mywf", "A waveform")
wf.Write()
```


