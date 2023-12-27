# VOICES

Voices are the source of sound for sqlizer.  The have a real
number output that ranges from minus one to one.  This document
describes the parameters that control voices

### idx

The idx parameter is a read-only number between 0 and 19 that is
assigned to the 20 oscillators in the system.  Use idx to uniquely
address a particular voice.  Change VOICE_COUNT in sqlizer.h to
change the number of available voices.

### noteid

Note ID is a 20 character string assigned by the user interface
that is using the voice.  This has no specific meaning and is
intended to let the UI identify groups of voices by name.

### chordid

Chord ID is a 20 character string that can identify the voices
that make up a chord.  The idea is that the UI would allocate and
configure a set of voices without starting them.  Then, when all
the voices are ready the UI could issue an SQL command to turn
them on at the same time. For example: 
    UPDATE voices SET vstate=2 WHERE chorid="mychord"

### vstate

Use vstate to free(0), allocate(1), start(2), sustain(3), or go to
ADSR release(4).


### ontime

Ontime has the number of milliseconds that the note has played,
This value controls the ADSR transitions.  It is set to zero at
the start of each step in the ADSR envelope.  You can set this to
zero when starting a voice but you should probably not edit it
otherwise.

## Oscillators

Each voice has four oscillators: two main oscillators, a vibrato
oscillator and a tremelo oscillator.  This section describes the
voice fields that share a common meaning in an oscillator.

### o1type, o2type, vibtype, tremtype

Type specifies the output waveform of the oscillator.  The type
can be off(0), sine(1), square(2), triangle(3), or noise(4).

<img src=osc_sine.jpg width=150> <img src=osc_square.jpg width=150>
<img src=osc_triangle.jpg width=150> <img src=osc_noise.jpg width=150>

### o1freq, o2freq, vibfreq, tremfreq

The freq parameter sets the frequency of the output for sine, square, and
triangle waveforms.  It is a floating point number in the range of 0.001
Hertz to 20000 Hertz.

### o1phaseacc, o2phaseacc, vibphaseacc, tremphaseacc

The phase accumulator is a floating point number in the range of zero to one
that tells where the output is in its cycle.  Multiply this by 360 to get
degrees or by 2 Pi to get radians or by 360 to get degrees.   The phase
accumulator has no meaning for noise output.  The phase accumulator is
set to zero at the start of a node and is usually not modified while the
note is playing.

### o1symmetry, o2symmetry, vibsymmetry, tremsymmetry

A fully symmetric waveform spends half of its cycle above zero and half
below zero and has a symmetry value of 0.5.  The symmetry parameter sets
the fraction of time the output waveform is positive.  A 1000 Hz output
with a symmetry of 0.1 would be positive for one-tenth of its one millisecond
period and would be negative for nine-tenths of its period.  Symmetry
is useful to create pulses and ramp waveforms.  This figure shows a
triangle waveform with symmetry set to 0.1.

<img src=osc_symmetry.jpg width=150>

### o1phaseoffset, o2phaseoffset, vibphaseoffset, tremphaseoffset

Normally a waveform starts at 0 degrees and an output value of zero.
You can redefine where in the cycle the waveform starts using phaseoffset.
Setting phaseoffset to 0.25 sets the output to the peak of sine wave.
A phaseoffset of 0.25 combined with a symmetry of 0.01 gives a positive ramp.

<img src=osc_ramp.jpg width=150>


### o1gain, o2gain

Gain is a real value between 0 and 1 and is multiplied by the oscillator
output before going to the o1/o2 mixer.  Note that this is an attenuation
and not really a 'gain'.

### vibdepth

This is the maximum frequency that is added to o1 as part of vibrato.
The o1 output frequency varies between o1freq and (o1freq + vibdepth).

### tremdepth

This is the maximum gain that is added to o1gain as part of tremolo.
The o1 output gain varies between o1gain and (o1gain + tremdepth).
The combination of o1gain and tremdepth can make the gain applied to
o1 greater than 1.

### glidefreq

Glide give you the ability to smoothly transition from the current
o1 frequency to a new frequency.  The glidefreq parameter is the o1
frequency after the glide completes.   Glide is another term for
portamento.

### glidems

The duration of a glide is set by the glidems parameter.  It specifies
the duration of the glide in milliseconds.

### mixmode

The outputs of o1 and o2 are combined with a function that is set by
mixmode.  A value of off() causes the o2 output value to be ignored.
A value of sum(1) adds the two outputs.  (Set the o1/o2 gains carefully
if summing the outputs.)  A value AM(2) uses o2 to amplitude modulate
the o1 output.  A value of FM(3) uses o2 to frequency modulate the o1
frequency.  A value of (4) ring modulates o1 by o2, and a value of (5)
does a hardsync of o1 by o2.  
 In hardsync the phase of o1 is forced to zero every time the phase
of o2 crosses zero.

## Filters

The mixed signal of oscillator #1 and oscillator #2 is passed to two
second-order digital filters that can be configured as low pass, high
pass, band pass, or band reject.  Low and high pass filters can have
either 6 or 12 rolloff.

### flttype

The type of filter as one of off(0), low-pass(1), high-pass(2),
band-pass(3), or band-reject(4).

### fltfreq1

Cutoff frequency for the first filter.  This should be the lower
frequency for band-pass and band-reject filters.  The filter range
is from 1 to 20000 Hertz.

### fltfreq2

Cutoff frequency for the second filter.  This should be the higher
frequency for band-pass and band-reject filters.  The filter range
is from 1 to 20000 Hertz.

### fltrolloff

This is the output filter rolloff in dB for low and high pass filters.
The rolloff must be set to an integer value of either 6 or 12.  This
parameter is ignored for band-pass and band-reject filters.

### fltQ

The Q of the filter in the range of 0.1 to 25.0 with typical values
in the range of 0.5 to 1.5.  High Q filters start to look like a narrow
band-pass filter.

## ADSR envelope

The ADSR envelope has eight steps each with a duration and an end gain.
The first step starts at a gain of 0.0 and will increase to step0gain
over the course of step0time milliseconds.  The change in gain is
linear over the step duration.

To end a note set the gain at that step to a value of zero.  To go
to sustain set the time to 60000 milliseconds.  A step duration of one
minute indicates a sustain note.  Setting vstate to forced release(4)
causes the ADSR to go to the next step in the envelope.

### stepXgain (X in range of 0 to 7)

The desired gain at the end of the step.  Gain is actually attenuation
and must be in between 0 and 1.0 inclusive.

### stepXtime (X in range of 0 to 7)

The duration of the step in milliseconds.  A value of 60000 indicated
that the note is to be sustained indefinely at the gain set by the
previous step.  When exiting sustain the gain if set immediately to
the gain specified by the step.  In practice is it a good idea to
set the gain of a sustain to the same gain as in the previous step.

## Output control

### outputclipping

Either a zero or a one to indicate if voice output should be clipped
to the range of -1 to 1 before going to the final gain stage. Default
is 1 to have clipping enabled.

### outputgain

The final gain to apply to the voice before the signal goes to the
output channel summer.  This is in the range of 0.0 to 1.0 and is
actually an attenuation of the voice output.

### outputchannel

The output of SQLizer can have either one or two channel for monophonic
or stereo sound.  If stereo output is enabled this parameter specifies
which channel receives the voice.  This parameter is ignored for one
channel output.  A value of 1 routes the output to the left channel,
a value of 2 routes it to the right channel, and a value of 3 routes
it to both channels.


