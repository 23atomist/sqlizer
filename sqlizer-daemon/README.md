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
Currently only the oscillators are working.  The output of
oscillator #0 it routed to standard out of the program.
Test the program by starting it with output piped to aplay
so you can hear the output.
```
  ./sqlizer-daemon | aplay -c 1 -f S16_BE -r 44100
```

Use the Postgres Bash client to test the oscillators table.
```
  psql -h localhost -p 8889
```
You should see a introduction message and hash mark as a prompt.
Enter the following, one at a time, at the prompt.  You should
be able to cut and paste these commands.
```
  UPDATE oscillators SET otype=1 WHERE idx=0;   # sine
  UPDATE oscillators SET otype=2 WHERE idx=0;   # square
  UPDATE oscillators SET otype=3 WHERE idx=0;   # triangle
  UPDATE oscillators SET otype=4 WHERE idx=0;   # noise
  UPDATE oscillators SET otype=3, freq=220 WHERE idx=0;
  UPDATE oscillators SET glidefreq=880, glidems=4000 WHERE idx=0;
  SELECT * FROM oscillators WHERE idx=0;
  UPDATE oscillators SET otype=3, freq=0.5 WHERE idx=1;
  UPDATE oscillators SET vibratoosc=1, vibratodepth=0.005 WHERE idx=0;
  UPDATE oscillators SET symmetry=0.01, phaseoffset=0.25 WHERE idx=1;
  UPDATE oscillators SET otype=1 WHERE idx=0;   # off
```

