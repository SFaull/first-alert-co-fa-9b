import mido
import argparse

def midi_to_freq(midi_note):
    """Convert a MIDI note number to its frequency in Hz."""
    return 440.0 * 2 ** ((midi_note - 69) / 12)

def parse_midi_file(file_path):
    """
    Parse the MIDI file and return a list of note events.
    Each event is a dict with:
      - note: MIDI note number
      - frequency: calculated frequency in Hz
      - start_time_ms: start time in milliseconds
      - end_time_ms: end time in milliseconds
    """
    midi_file = mido.MidiFile(file_path)
    events = []
    active_notes = {}  # Stores active note start times: {note_number: start_time (sec)}
    current_time = 0  # Absolute time in seconds

    # Iterate over messages in merged order so that timing is correct even for multiple tracks
    for msg in midi_file:
        current_time += msg.time  # msg.time is in seconds
        if msg.type == 'note_on' and msg.velocity > 0:
            # Record the start time for this note
            active_notes[msg.note] = current_time
        elif msg.type == 'note_off' or (msg.type == 'note_on' and msg.velocity == 0):
            if msg.note in active_notes:
                start_time = active_notes.pop(msg.note)
                end_time = current_time
                freq = midi_to_freq(msg.note)
                reg = 65535 - (((1.0/freq)/(4.0/8000000)) - 1)
                event = {
                    'note': msg.note,
                    'frequency': freq,
                    'start_time_ms': start_time * 1000,
                    'end_time_ms': end_time * 1000,
                    'reg': round(reg)
                }
                events.append(event)
    return events

def main():
    parser = argparse.ArgumentParser(
        description='Process a MIDI file and extract note frequencies and timing information.'
    )
    parser.add_argument(
        '-f', '--file', required=True,
        help='Path to the MIDI file'
    )
    args = parser.parse_args()
    
    events = parse_midi_file(args.file)
    
    for event in events:
        print(
            f"Note {event['note']}, Frequency: {event['frequency']:.2f}, Reg: {event['reg']}, "
            f"Start: {event['start_time_ms']:.0f}, End: {event['end_time_ms']:.0f}"
        )

if __name__ == '__main__':
    main()
