# OSCILLATORS

Oscillators are the source of sound for sqlizer.  The have a real
number output that ranges from minus one to one.  This document
describes the parameters that control the oscillators.

## idx

The idx parameter is a read-only number between 0 and 99 that is
assigned to the 100 oscillators in the system.  Use this to 
uniquely address a particular oscillator.


## otype

Oscillators can have one of five types of output: off, sine, square,
triangle, or noise.  Set otype as follows:
- 0 : off
- 1 : sine
- 2 : square
- 3 : triangle
- 4 : white noise

<img src=osc_sine.jpg width=150> <img src=osc_square.jpg width=150>
<img src=osc_triangle.jpg width=150> <img src=osc_noise.jpg width=150>


## freq

The freq parameter sets the frequency of the output for sine, square, and
triangle waveforms.  It is a floating point number in the range of 0.001
Hertz to 20000 Hertz.

## phaseaccumulator

The phaseaccumulator is a floating point number in the range of zero to one
that tells where the output is in its cycle.  Multiply this by 360 to get
degrees or by 2 Pi to get radians.

## symmetry

A fully symmetric waveform spends half of its cycle above zero and half
below zero and has a symmetry value of 0.5.  The symmetry parameter sets
the fraction of time the output waveform is positive.  A 1000 Hz output
with a symmetry of 0.1 would be positive for one-tenth of its one millisecond
period and would be negative for nine-tenths of its period.  Symmetry
is useful to create pulses and ramp waveforms.  This figure shows a
triangle waveform with symmetry set to 0.1.

<img src=osc_symmetry.jpg width=150>

## phaseoffset

Normally a waveform starts at 0 degrees and with an output value of zero.
You can redefine where in the cycle the waveform starts using phaseoffset.
Setting phaseoffset to 0.25 sets the output to the peak of sine wave.
A phaseoffset of 0.25 combined with a symmetry of 0.01 gives a positive ramp.

<img src=osc_ramp.jpg width=150>

## vibratoosc

You can apply vibrato to an oscillator using the output of another oscillator.
The vibratoosc parameter specifies which oscillator to use.  The default
value of -1 turns off vibrato.

<img src=osc_vibrato.jpg width=150>

## vibratofreq

This parameter specifies the maximum frequency offset using vibrato.  For
example, to get an output frequency that ranges between 400 and 480 Hertz
set the oscillator frequency to 400 and the vibratofreq to 80.  The
frequency of the vibratoosc controls how fast the output goest from
400 to 800 Hertz.

## glidefreq

Glide give you the ability to smoothly transition from the current
oscillator frequency to a new frequency.  The glidefreq parameter is
the oscillator frequency after the glide completes.   Glide is another
term for portamento.

## glidems

The duration of a glide is set by the glidems parameter.  It specifies
the duration of the glide in milliseconds.

## hardsyncosc

If a hard sync oscillator is used the phase of the output oscillator is
forced to zero each time the hardsyncosc completes a cycle.  This can
create rich harmonics in the output.
    int      hardsyncosc;      // Index of osc that controls hardsync.  ==-1 if off


