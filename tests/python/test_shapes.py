"""Test Shapes."""

import numpy as np

from pypulseg._core._shapes import (
    PulseqShapeArbitrary,
    PulseqShapeTrap,
)


def test_pulseq_shape_arbitrary_serialization():
    """
    Test the serialization and deserialization of PulseqShapeArbitrary.
    """
    # Test data
    samples = np.array([0.1, 0.2, 0.3, 0.4, 0.5], dtype=np.float32)

    # Create PulseqShapeArbitrary object
    obj = PulseqShapeArbitrary(samples)

    # Serialize to bytes (default little-endian)
    serialized_bytes = obj.to_bytes(byteswap=False)

    # Check length of serialized data
    assert len(serialized_bytes) == 4 + 4 * len(
        samples
    )  # 4 bytes for nSamples + 4 bytes for each float

    # Deserialize from bytes
    deserialized_obj = PulseqShapeArbitrary.from_bytes(serialized_bytes, byteswap=False)

    # Check if the deserialized object matches the original
    np.testing.assert_array_equal(deserialized_obj.samples, samples)


def test_pulseq_shape_arbitrary_big_endian_serialization():
    """
    Test the serialization and deserialization of PulseqShapeArbitrary with big-endian.
    """
    # Test data
    samples = np.array([0.1, 0.2, 0.3, 0.4, 0.5], dtype=np.float32)

    # Create PulseqShapeArbitrary object
    obj = PulseqShapeArbitrary(samples)

    # Serialize to bytes (big-endian)
    serialized_bytes = obj.to_bytes(byteswap=True)

    # Check length of serialized data
    assert len(serialized_bytes) == 4 + 4 * len(
        samples
    )  # 4 bytes for nSamples + 4 bytes for each float

    # Deserialize from bytes
    deserialized_obj = PulseqShapeArbitrary.from_bytes(serialized_bytes, byteswap=True)

    # Check if the deserialized object matches the original
    np.testing.assert_array_equal(deserialized_obj.samples, samples)


def test_pulseq_shape_trap_serialization():
    """
    Test the serialization and deserialization of PulseqShapeTrap.
    """
    # Test data
    rise_time = 10
    flat_time = 20
    fall_time = 30

    # Create PulseqShapeTrap object
    obj = PulseqShapeTrap(rise_time, flat_time, fall_time)

    # Serialize to bytes (default little-endian)
    serialized_bytes = obj.to_bytes(byteswap=False)

    # Check length of serialized data
    assert len(serialized_bytes) == 12  # 3 integers (4 bytes each)

    # Deserialize from bytes
    deserialized_obj = PulseqShapeTrap.from_bytes(serialized_bytes, byteswap=False)

    # Check if the deserialized object matches the original
    assert deserialized_obj.riseTime == rise_time
    assert deserialized_obj.flatTime == flat_time
    assert deserialized_obj.fallTime == fall_time


def test_pulseq_shape_trap_big_endian_serialization():
    """
    Test the serialization and deserialization of PulseqShapeTrap with big-endian.
    """
    # Test data
    rise_time = 10
    flat_time = 20
    fall_time = 30

    # Create PulseqShapeTrap object
    obj = PulseqShapeTrap(rise_time, flat_time, fall_time)

    # Serialize to bytes (big-endian)
    serialized_bytes = obj.to_bytes(byteswap=True)

    # Check length of serialized data
    assert len(serialized_bytes) == 12  # 3 integers (4 bytes each)

    # Deserialize from bytes
    deserialized_obj = PulseqShapeTrap.from_bytes(serialized_bytes, byteswap=True)

    # Check if the deserialized object matches the original
    assert deserialized_obj.riseTime == rise_time
    assert deserialized_obj.flatTime == flat_time
    assert deserialized_obj.fallTime == fall_time
