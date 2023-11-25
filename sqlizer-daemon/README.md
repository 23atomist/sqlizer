# sqlizer-daemon

Sqlizer-daemon is a Linux software music synthesizer.

The API consists of SQL statement from a Postgres client.  The
internal tables are rendered visible as Postgres DB tables 
using a library called librta.  Librta is described in detail
on its web site (https://librta.org).  The librta web site
has a link to a free book (PDF, EPUB, MOBI) that describes 
the library in greater detail.


## Sqlizer-daemon installation
Install librta with the following:
```
  sudo apt-get install byacc flex postgresql-client-common
  git clone --depth=1 https://github.com/librta/librta.git
  cd librta/src
  make
  sudo make install
```

Install sqlizer-daemon with the following:
```
  git clone https://github.com/AtomlabLLC/sqlizer.git
  cd sqlizer/sqlizer-daemon
  make
```

## Test
Currently only part of the voices are working.  The output of
voice #0 it routed to standard out of the program.
Test the program by starting it with output piped to aplay
so you can hear the output.
```
  ./sqlizer-daemon | aplay -c 1 -f S16_BE -r 44100 &
```

Use the Postgres Bash client to test the oscillators table.
```
  psql -h localhost -p 8889
```
You should see an introductory message and hash mark as a prompt.
Enter the following, one at a time, at the prompt.  You should
be able to cut and paste these commands.
```
  SELECT name, help FROM rta_columns WHERE table=voices;  -- lots of columns!

  UPDATE voices SET vstate=1 WHERE idx=0;   -- reserve this voice
  UPDATE voices SET o1type=1, o1freq=400.0, o1symmetry=0.5, o1phaseoffset=0.0,
         o1gain=0.80, o2type=1, o2freq=50.000, o2symmetry=0.500, o2phaseoffset=0.000,
         o2gain=0.000, mixmode=3, vibtype=1, vibfreq=10.000, vibsymmetry=0.500,
         vibphaseoffset=0.000, vibdepth=55, glidefreq=0.000, glidems=0, tremtype=1,
         tremfreq=600.000, tremsymmetry=0.500, tremphaseoffset=0.000, tremdepth=0.100,
         step0time=1000, step1time=1000, step2time=1000, step3time=1000, step4time=1,
         step5time=1, step6time=1, step7time=1, step0gain=1.000, step1gain=0.7000,
         step2gain=0.6000, step3gain=0.01, step4gain=0, step5gain=1.000, step6gain=1.000,
         step7gain=1.000, o1phaseacc=0, o2phaseacc=0
  WHERE idx = 0;
  UPDATE voices SET vstate=2 WHERE idx=0;   -- play the note
```

