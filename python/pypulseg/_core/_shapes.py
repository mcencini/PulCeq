"""Pulseq shapes."""

__all__ = ["PulseqShapeArbitrary", "PulseqShapeTrap"]

import numpy as np
import struct
from dataclasses import dataclass


@dataclass
class PulseqShapeArbitrary:
    """
    Represents an arbitrary waveform shape.

    Attributes
    ----------
    samples : np.ndarray
        Array of waveform samples (dtype=np.float32).
    """

    samples: np.ndarray

    def to_bytes(self, byteswap: bool = False) -> bytes:
        """
        Serialize PulseqShapeArbitrary to bytes.

        Parameters
        ----------
        byteswap : bool, optional
            Whether to byteswap (True -> big-endian, False -> little-endian), by default False

        Returns
        -------
        bytes
            Serialized byte representation of the object.
        """
        n_samples = len(self.samples)
        endian = ">" if byteswap else "<"

        return (
            struct.pack(endian + "i", int(n_samples))
            + self.samples.astype(endian + "f4").tobytes()
        )

    @classmethod
    def from_bytes(cls, data: bytes, byteswap: bool = False) -> "PulseqShapeArbitrary":
        """
        Deserialize PulseqShapeArbitrary from bytes.

        Parameters
        ----------
        data : bytes
            Byte data to deserialize.
        byteswap : bool, optional
            Whether to byteswap (True -> big-endian, False -> little-endian), by default False

        Returns
        -------
        PulseqShapeArbitrary
            Deserialized object.
        """
        endian = ">" if byteswap else "<"
        n_samples = struct.unpack(endian + "i", data[:4])[0]
        samples = np.frombuffer(
            data[4 : 4 + (n_samples * 4)], dtype=np.float32, count=n_samples
        )
        if byteswap:
            samples = samples.byteswap()

        return cls(samples)


@dataclass
class PulseqShapeTrap:
    """
    Represents a trapezoid shape.

    Attributes
    ----------
    riseTime : int
        Ramp-up time of the trapezoid (in microseconds).
    flatTime : int
        Flat-top time of the trapezoid (in microseconds).
    fallTime : int
        Ramp-down time of the trapezoid (in microseconds).
    """

    riseTime: int
    flatTime: int
    fallTime: int

    def to_bytes(self, byteswap: bool = False) -> bytes:
        """
        Serialize PulseqShapeTrap to bytes.

        Parameters
        ----------
        byteswap : bool, optional
            Whether to byteswap (True -> big-endian, False -> little-endian), by default False

        Returns
        -------
        bytes
            Serialized byte representation of the object.
        """
        endian = ">" if byteswap else "<"
        return (
            struct.pack(endian + "i", self.riseTime)
            + struct.pack(endian + "i", self.flatTime)
            + struct.pack(endian + "i", self.fallTime)
        )

    @classmethod
    def from_bytes(cls, data: bytes, byteswap: bool = False) -> "PulseqShapeTrap":
        """
        Deserialize PulseqShapeTrap from bytes.

        Parameters
        ----------
        data : bytes
            Byte data to deserialize.
        byteswap : bool, optional
            Whether to byteswap (True -> big-endian, False -> little-endian), by default False

        Returns
        -------
        PulseqShapeTrap
            Deserialized object.
        """
        endian = ">" if byteswap else "<"
        rise_time = struct.unpack(endian + "i", data[:4])[0]
        flat_time = struct.unpack(endian + "i", data[4:8])[0]
        fall_time = struct.unpack(endian + "i", data[8:])[0]

        return cls(rise_time, flat_time, fall_time)
