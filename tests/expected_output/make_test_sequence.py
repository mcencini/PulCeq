import numpy as np
import pypulseq as pp

seq = pp.Sequence()

# RF event
rf, _, _ = pp.make_sinc_pulse(flip_angle=90 * np.pi / 180, duration=2e-3, slice_thickness=5e-3, apodization=0.5, time_bw_product=4, system=pp.opts())

# ADC event (no phase mod)
adc = pp.make_adc(num_samples=128, dwell=10e-6, delay=0, phase_offset=0)

# ADC event (with phase mod)
adc_phase = pp.make_adc(num_samples=128, dwell=10e-6, delay=0, phase_offset=np.pi/2)

# Trapezoid gradient (standard)
gx = pp.make_trapezoid(channel='x', amplitude=10, duration=2e-3, system=pp.opts())

# Trapezoid gradient (extended, with area)
gy = pp.make_trapezoid(channel='y', area=5, duration=3e-3, system=pp.opts())

# Arbitrary gradient
gz = pp.make_arbitrary_grad(channel='z', waveform=np.array([0, 1, 0, -1, 0]), duration=2e-3, system=pp.opts())

# All label types (SET and INC)
for label in ['SLC', 'SEG', 'REP', 'AVG', 'SET', 'ECO', 'PHS', 'LIN', 'PAR', 'ACQ', 'TRID', 'NAV', 'REV', 'SMS', 'REF', 'IMA', 'NOISE', 'PMC', 'NOROT', 'NOPOS', 'NOSCL', 'ONCE']:
    seq.add_block(pp.LabelOp(label, 'SET'))
    seq.add_block(pp.LabelOp(label, 'INC'))

# Trigger event
seq.add_block(pp.make_trigger(channel=1, system=pp.opts()))

# Rotation event
seq.add_block(pp.make_rotation(angle=np.pi/4, axis='z', system=pp.opts()))

# Soft delays (TE, TR, TI, ESP, RECTIME, T2PREP, TE2, TR2)
for hint in ['TE', 'TR', 'TI', 'ESP', 'RECTIME', 'T2PREP', 'TE2', 'TR2']:
    seq.add_block(pp.make_delay(1e-3, hint=hint, system=pp.opts()))

# Add all events to sequence
seq.add_block(rf)
seq.add_block(adc)
seq.add_block(adc_phase)
seq.add_block(gx)
seq.add_block(gy)
seq.add_block(gz)

# Write to file
seq.write('seq2.seq')