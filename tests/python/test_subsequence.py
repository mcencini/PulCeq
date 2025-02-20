"""Test subsequence identification."""

from pytest import fixture

import numpy.testing as npt
import pypulseg

# BLOCKS (RF, GX, GY, GZ, ADC)
# Preparation
INV = 1  # (RFinv, None, None, None, None)

# Main Readout
EXC0 = 2  # (RFexc0, None, None, Gslab0, None)
PHS0 = 3  # (None, PHSx0, PHSy0, PHSz0, None)
READ0 = 4  # (None, READx0, None, None, ECHO0)
DUMMY_READ0 = 5  # (None, READx0, None, None, None)
SPOIL = 6  # (None, None, None, Gspoil, None)

# Motion Nav
EXC1 = 7  # (RFexc1, None, None, Gslice1, None)
READ1 = 8  # (None, READx1, READy1, None, ECHO1)

# PI Calibration
EXC2 = 7  # (RFexc2, None, None, Gslice2, None)
PHS2 = 10  # (None, PHSx0, PHSy0, PHSz0, None)
READ2 = 11  # (None, READx2, None, None, ECHO2)

# Noise Scan
EXC3 = 7  # (RFexc2, None, None, Gslice2, None)
READ3 = 12  # (None, None, None, None, ECHO3)


def _mprage():
    return [INV, SPOIL] + 32 * [EXC0, PHS0, READ0, PHS0, SPOIL]


def _mprage_warmup():
    return [INV, SPOIL] + 32 * [EXC0, PHS0, DUMMY_READ0, PHS0, SPOIL]


def _megre():
    return [EXC0, PHS0, READ0, READ0, READ0, READ0, PHS0, SPOIL]


def _megre_warmup():
    return [EXC0, PHS0, DUMMY_READ0, DUMMY_READ0, DUMMY_READ0, DUMMY_READ0, PHS0, SPOIL]


def _pical():
    return [EXC2, PHS2, READ2, PHS2, SPOIL]


def _noisecal():
    return [EXC3, READ3]


def _navscan():
    return [EXC1, READ1]


@fixture
def simple_megre():
    """
    Simple ME-GRE block representation.

    Consists of 32 ME-GRE dummy segment for warmup (acquiring 32 lines)
    followed by 32 * 32 ME-GRE segments, corresponding to a (ky, kz) = (32, 32) kspace size.

    """
    return 32 * _megre_warmup() + 1024 * _megre()


@fixture
def megre():
    """
    Realistic ME-GRE block representation.

    Consists of a single noise calibration readout,
    followed by an 8x8 coil sensitivity map calibration,
    a single MPRAGE dummy segment for warmup (acquiring 32 lines)
    and by 32 * 32 ME-GRE segments, corresponding to a (ky, kz) = (32, 32) kspace size.

    """
    return _noisecal() + 64 * _pical() + 32 * _megre_warmup() + 1024 * _megre()


@fixture
def simple_mprage():
    """
    Simple MPRAGE block representation.

    Consists of a single MPRAGE dummy segment for warmup (acquiring 32 lines)
    followed by the actual acquisition, consisting of 32 blocks of interleaved
    MPRAGE segment + motion nav segment (three orthogonal 2D acquisitions),
    corresponding to a (ky, kz) = (32, 32) kspace size.

    """
    return _mprage_warmup() + 32 * _mprage()


@fixture
def mprage():
    """
    Realistic MPRAGE block representation.

    Consists of a single noise calibration readout,
    followed by an 8x8 coil sensitivity map calibration,
    a single MPRAGE dummy segment for warmup (acquiring 32 lines)
    and the actual acquisition, consisting of 32 blocks of interleaved
    MPRAGE segment + motion nav segment (three orthogonal 2D acquisitions),
    corresponding to a (ky, kz) = (32, 32) kspace size.

    """
    return (
        _noisecal()
        + 64 * _pical()
        + _mprage_warmup()
        + 32 * (_mprage() + 3 * _navscan())
    )


def test_simple_megre(simple_megre):
    subseq_def, subseq_lut = pypulseg.autoseg.segment_sequence(simple_megre)

    # check
    assert len(subseq_def) == 2
    npt.assert_allclose(subseq_def[0], _megre_warmup())
    npt.assert_allclose(subseq_def[1], _megre())
    npt.assert_allclose(
        subseq_lut, 32 * len(_megre_warmup()) * [0] + 1024 * len(_megre()) * [1]
    )


def test_megre(megre):
    subseq_def, subseq_lut = pypulseg.autoseg.segment_sequence(megre)

    # check
    assert len(subseq_def) == 4
    npt.assert_allclose(subseq_def[0], _noisecal())
    npt.assert_allclose(subseq_def[1], _pical())
    npt.assert_allclose(subseq_def[2], _megre_warmup())
    npt.assert_allclose(subseq_def[3], _megre())
    npt.assert_allclose(
        subseq_lut,
        len(_noisecal()) * [0]
        + 64 * len(_pical()) * [1]
        + 32 * len(_megre_warmup()) * [2]
        + 1024 * len(_megre()) * [3],
    )


def test_simple_mprage(simple_mprage):
    subseq_def, subseq_lut = pypulseg.autoseg.segment_sequence(simple_mprage)

    # check
    assert len(subseq_def) == 2
    npt.assert_allclose(subseq_def[0], _mprage_warmup())
    npt.assert_allclose(subseq_def[1], _mprage())
    npt.assert_allclose(
        subseq_lut, len(_mprage_warmup()) * [0] + 32 * len(_mprage()) * [1]
    )


def test_mprage(mprage):
    subseq_def, subseq_lut = pypulseg.autoseg.segment_sequence(mprage)

    # check
    assert len(subseq_def) == 4
    npt.assert_allclose(subseq_def[0], _noisecal())
    npt.assert_allclose(subseq_def[1], _pical())
    npt.assert_allclose(subseq_def[2], _mprage_warmup())
    npt.assert_allclose(subseq_def[3], (_mprage() + 3 * _navscan()))
    npt.assert_allclose(
        subseq_lut,
        len(_noisecal()) * [0]
        + 64 * len(_pical()) * [1]
        + len(_mprage_warmup()) * [2]
        + 32 * (len(_mprage()) + 3 * len(_navscan())) * [3],
    )
