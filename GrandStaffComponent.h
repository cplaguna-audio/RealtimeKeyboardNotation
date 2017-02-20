/*
 * Component for drawing the staff notation.
 * Chris Laguna
 */

#ifndef GRANDSTAFFCOMPONENT_H_INCLUDED
#define GRANDSTAFFCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

using namespace std;

/*
 * To the question "Is this a C# or a Db?". This depends on the musical context, which we can't
 * know (or is difficult to know) programatically. However, we can assume that someone is playing
 * in a key, and then use the accidentals used in that key. For now, we handle only the simple cases
 * where a key uses only sharps or only flats, and all white keys are natural.
 * TODO: Extend this to work for the harder keys, e.g. C# major.
 */
enum AccidentalMode {
  ALL_SHARPS,
  ALL_FLATS
};

/*
 * Note letters. The line or space a note is drawn on depends on the note letter, not
 * on the note's MIDI pitch.
 */
enum Letter {
  A = 0,
  B,
  C,
  D,
  E,
  F,
  G,
  MISTAKE  // Sanity.
};

class GrandStaffComponent : public Component {
public:
  GrandStaffComponent();
  virtual ~GrandStaffComponent();

  void paint (Graphics&) override;
  void resized() override;
  
  void addNote(int midi_pitch);
  void removeNote(int midi_pitch);

  void setAccidentalMode(AccidentalMode at);
  
private:
  // Drawing is just placing a bunch of images on top of each other.
  Image grand_staff_image;
  Image whole_note_image;
  Image flat_image;
  Image sharp_image;
  
  /*
   * The notes that should be drawn at any given time. Midi note-on and note-off messages are
   * handled by simply adding and removing notes from this list. Drawing happens in repaint,
   * of course.
   */
  std::vector<int> notes_to_draw;
  
  AccidentalMode accidental_mode = ALL_SHARPS;
  
  // A bunch of hard-coded values to draw the images in the correct spots.
  const float GRAND_STAFF_HEIGHT = 100.f;
  const float WHOLE_NOTE_HEIGHT = 13.f;
  const float SHARP_HEIGHT = 37.f;
  const float FLAT_HEIGHT = 28.f;
  
  const int STAFF_Y_OFFSET = 65;  // Staff y position on the component.
  
  const float NOTE_DELTA_Y = 3.5;            // Number of pixels to the note up or
                                             // down when changing pitch by a semitone.
  const int NOTE_X = 66;                     // X position of every note on the staff.
  const int MIDI_69_Y = 20 + STAFF_Y_OFFSET; // Y position of A440 (midi pitch 69) on treble clef.
  const int MIDI_48_Y = 83 + STAFF_Y_OFFSET; // Y position of C3 on (midi pitch 48) bass clef.
  const int OCTAVE_DELTA = 7;                // Two notes an octave apart are separated by 7
                                             // lines/spaces on the staff.
  
  const int SHARP_X_OFFSET = -22;  // Used to calculate the position of the sharp image relative to the whole note.
  const int SHARP_Y_OFFSET = -12;  // Used to calculate the position of the sharp image relative to the whole note.
  const int FLAT_X_OFFSET = -18;   // Used to calculate the position of the flat image relative to the whole note.
  const int FLAT_Y_OFFSET = -13;   // Used to calculate the position of the flat image relative to the whole note.
  
  // Repaint helpers.
  bool drawOnTrebleClef(int note);  // Should the note be drawn on the treble clef? (or the bass clef)
  void drawNote(Graphics& g, int note);
  void drawNoteOnTrebleClef(Graphics& g, int note);
  void drawNoteOnBassClef(Graphics& g, int note);
  
  bool hasAccidental(int note);
  void drawAccidental(Graphics& g, int x_ref, int y_ref);
  void drawSharp(Graphics& g, int x_ref, int y_ref);
  void drawFlat(Graphics& g, int x_ref, int y_ref);
  
  int getNumberLedgerLines(int distance);
  void drawLedgerLinesOnTrebleClef(Graphics& g, int num_ledger_lines, bool above_clef);
  void drawLedgerLinesOnBassClef(Graphics& g, int num_ledger_lines, bool above_clef);

  /*
   * If you getNoteLetter was trivial (like I did), you forgot about enharmonics. We have to
   * consider things like "is this note F or E#?" (aka the accidental type).
   */
  Letter getNoteLetter(int note);
 
  /*
   * How many lines/spaces apart is the current note from a reference pitch. This tells
   * us where to draw the note.
   */
  int getLetterDistance(int note, int ref_pitch);
  int lettersAboveReference(int note_letter, int ref_letter);
  int lettersBelowReference(int note_letter, int ref_letter);
  
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrandStaffComponent)
};


#endif  // GRANDSTAFFCOMPONENT_H_INCLUDED
