# first-alert-co-fa-9b
Custom firmware for the First Alert CO-FA-9B Carbon Monoxide Alarm.
When the button is held, the piezo will play back a midi file embedded into the FLASH.
In this demo the song is All Star by Smashmouth.

# Programmering Instructions
Attatch PICkit programmer to pads on the PCB as shown in the photo.
The solder link next to the the ICSP clock pin marked "PR" must be broken otherwise programming will fail

![Programming pinout](resources/programming-pinout.png)

**DISCLAIMER:** This will overwrite the original firmware on the MCU and the device will no longer function as a Carbon Monoxide Alarm.
Readout protection is enabled on the MCU so it is not possible to make a backup to restore the original firmware.

# MIDI Analyser

To load a custom midi file, a python script [midi-analyser.py](tools/midi-analyser.py) is provided to convert a midi file to an array which can be interpretted by the application.

Usage:

``` sh
# Navigate to the tools directory
cd tools

# Create python virtual environment
python3 -m venv env

# Start python virtual environment
source env/bin/activate

# Install dependencies
pip install -r requirements.txt

# Run script on midi file
python3 midi-analyser.py -f ../resources/smashmouth-all-star.mid
```

If successful, the script will print information about each midi note on a new line.

The reg value represents the 16-bit value that must be loaded into the Timer1 register to play a note of the correct frequency.
The start and end values are millisecond timestamps denoting the start and end of the note relative to the start of playback.

Copy the reg, start and end values into the lookup table in [main.c](project/first-alert.X/main.c), ensuring the size is adjusted accordingly.

``` c
#define NOTE_COUNT  0 /* number of LUT entries */
const note_def_t song_lut[NOTE_COUNT] = {
    /* 
        {timer value, start time, end time},
        ...
        ...
    */
}
```