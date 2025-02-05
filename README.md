
![logo](doc/logo.svg)

THIS REPO IS IN DEVELOPMENT

A C representation of arbitrary magnetic resonance pulse sequences, 
based on a "scaled parent block" description.

The idea is to represent the sequence as a (typically small) collection of parent/prototype
[Pulseq](https://pulseq.github.io/)
blocks, that are played out repeatedly during the scan with different
RF/gradient amplitudes, RF/DAQ frequency/phase offsets, etc:

![model](doc/model.svg)

The goal of this repository is to provide an **open standard** specification
for encapsulating this sequence description in C code.

This specification is contained in the ./csrc/ folder, which contains the following:
* **pulSeg.h**: defines `SegmentedSequence`, a nested struct containing the entire sequence, and
function declarations (interfaces) for various tasks including
   * serializing to file
   * allocating and freeing up memory
   * <...>
* **pulSeg.c**: implementation

## Example usage

* Planned: PulSeq.h will be used in the upcoming version of the Pulseq sequence interpreter for GE scanners (PGE2)

