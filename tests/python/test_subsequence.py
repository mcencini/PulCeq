"""Test subsequence identification."""


# BLOCKS (RF, GX, GY, GZ, ADC)
# Preparation
INV = 1   # (RFinv, None, None, None, None)

# Main Readout
EXC = 2   # (RFexc, None, None, Gslab, None)
PHS = 3   # (None, PHSx, PHSy, PHSz, None)
READ = 4  # (None, READx, None, None, ECHO1)
SPOIL = 5 # (None, None, None, Gspoil, None)

# Motion Nav

