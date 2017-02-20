/*
 * GrandStaffComponent.cpp file header.
 *
 * Chris Laguna
 */

#include "../JuceLibraryCode/JuceHeader.h"
#include "GrandStaffComponent.h"
#include "MainContentComponent.h"

/***** Public members *****/

GrandStaffComponent::GrandStaffComponent() {
  
  // Convert images from binary data to Images. 
  grand_staff_image = ImageFileFormat::loadFrom(BinaryData::Grand_Staff_png, BinaryData::Grand_Staff_pngSize);
  int actual_height = grand_staff_image.getHeight();
  int actual_width = grand_staff_image.getWidth();
  float scale_factor = GRAND_STAFF_HEIGHT / (float) actual_height;
  grand_staff_image = grand_staff_image.rescaled(actual_width * scale_factor, actual_height * scale_factor);
  
  whole_note_image = ImageFileFormat::loadFrom(BinaryData::Whole_Note_png, BinaryData::Whole_Note_pngSize);
  actual_height = whole_note_image.getHeight();
  actual_width = whole_note_image.getWidth();
  scale_factor = WHOLE_NOTE_HEIGHT / (float) actual_height;
  whole_note_image = whole_note_image.rescaled(actual_width * scale_factor, actual_height * scale_factor);
 
  flat_image = ImageFileFormat::loadFrom(BinaryData::Flat_png, BinaryData::Flat_pngSize);
  actual_height = flat_image.getHeight();
  actual_width = flat_image.getWidth();
  scale_factor = FLAT_HEIGHT / (float) actual_height;
  flat_image = flat_image.rescaled(actual_width * scale_factor, actual_height * scale_factor);
 
  sharp_image = ImageFileFormat::loadFrom(BinaryData::Sharp_png, BinaryData::Sharp_pngSize);
  actual_height = sharp_image.getHeight();
  actual_width = sharp_image.getWidth();
  scale_factor = SHARP_HEIGHT / (float) actual_height;
  sharp_image = sharp_image.rescaled(actual_width * scale_factor, actual_height * scale_factor);
}

GrandStaffComponent::~GrandStaffComponent() {
  notes_to_draw.clear();
}

void GrandStaffComponent::paint (Graphics& g) {
  g.fillAll (Colours::white);
  
  // Draw the staff.
  g.drawImageAt(grand_staff_image, 0, STAFF_Y_OFFSET);
  
  // Draw each note individually.
  for(int note_idx = 0; note_idx < notes_to_draw.size(); note_idx++) {
    int cur_note = notes_to_draw[note_idx];
    drawNote(g, cur_note);
  }

}

void GrandStaffComponent::resized() {}

void GrandStaffComponent::addNote(int midi_pitch) {
  // Only add notes within the range of the keyboard.
  if(midi_pitch < MainContentComponent::getMinNote() || midi_pitch > MainContentComponent::getMaxNote()) {
    return;
  }
  
  notes_to_draw.push_back(midi_pitch);
  repaint();
}

void GrandStaffComponent::removeNote(int midi_pitch) {
  notes_to_draw.clear();
  repaint();

  // Only remove notes within the range of the keyboard.
  if(midi_pitch < MainContentComponent::getMinNote() || midi_pitch > MainContentComponent::getMaxNote()) {
    return;
  }
}

void GrandStaffComponent::setAccidentalMode(AccidentalMode at) {
  accidental_mode = at;
}

/***** Private Members *****/

bool GrandStaffComponent::drawOnTrebleClef(int note) {
  // Middle C and above go on the treble clef.
  return note >= 60;
}

void GrandStaffComponent::drawNote(Graphics& g, int note) {
  if(drawOnTrebleClef(note)) {
    drawNoteOnTrebleClef(g, note);
  }
  else {
    drawNoteOnBassClef(g, note);
  }
}

void GrandStaffComponent::drawNoteOnTrebleClef(Graphics& g, int note) {
  int note_x = NOTE_X;
  int distance = getLetterDistance(note, 69);
  int note_y = MIDI_69_Y - (distance * NOTE_DELTA_Y);
  g.drawImageAt(whole_note_image, note_x, note_y);
  
  if(hasAccidental(note)) {
    drawAccidental(g, note_x, note_y);
  }
  
  int num_ledger_lines = getNumberLedgerLines(distance);
  drawLedgerLinesOnTrebleClef(g, num_ledger_lines, distance > 0);
}

void GrandStaffComponent::drawNoteOnBassClef(Graphics& g, int note) {
  int note_x = NOTE_X;
  int distance = getLetterDistance(note, 48);
  int note_y = MIDI_48_Y - (distance * NOTE_DELTA_Y);
  g.drawImageAt(whole_note_image, note_x, note_y);
  
  if(hasAccidental(note)) {
    drawAccidental(g, note_x, note_y);
  }
  
  int num_ledger_lines = getNumberLedgerLines(distance);
  drawLedgerLinesOnBassClef(g, num_ledger_lines, distance > 0);
}

bool GrandStaffComponent::hasAccidental(int note) {
    // Midi pitch 69 is A, so wrapped note 9 is A.
  int wrapped_note = note % 12;
  switch(wrapped_note) {
    case 0:  // C
    case 2:  // D
    case 4:  // E
    case 5:  // F
    case 7:  // G
    case 9:  // A
    case 11: // B
      return false;
    case 1:  // C# / Db
    case 3:  // D# / Eb
    case 6:  // F# / Gb
    case 8:  // G# / Ab
    case 10: // A# / Bb
      return true;
   }
  
  // Other
  return false;
}

void GrandStaffComponent::drawAccidental(Graphics& g, int x_ref, int y_ref) {
  if(accidental_mode == ALL_SHARPS) {
    drawSharp(g, x_ref, y_ref);
  }
  else if(accidental_mode == ALL_FLATS) {
    drawFlat(g, x_ref, y_ref);
  }
}

void GrandStaffComponent::drawSharp(Graphics& g, int x_ref, int y_ref) {
  int x = x_ref + SHARP_X_OFFSET;
  int y = y_ref + SHARP_Y_OFFSET;
  g.drawImageAt(sharp_image, x, y);
}

void GrandStaffComponent::drawFlat(Graphics& g, int x_ref, int y_ref) {
  int x = x_ref + FLAT_X_OFFSET;
  int y = y_ref + FLAT_Y_OFFSET;
  g.drawImageAt(flat_image, x, y);
}

// The cases for bass clef and treble clef are only the same because of the chosen reference notes.
int GrandStaffComponent::getNumberLedgerLines(int distance) {
  int num_lines = 0;
  if(distance > 0) {
    num_lines = (distance - 5) / 2;
  }
  else {
    num_lines = ((distance * -1) -  3) / 2;
  }
  return max(num_lines, 0);
}

void GrandStaffComponent::drawLedgerLinesOnTrebleClef(Graphics& g, int num_ledger_lines, bool above_clef) {
  int start_x = NOTE_X - 1;
  int end_x = start_x + 15;
  int y = 0;
  int y_delta = 0;
  if(above_clef) {
    y = MIDI_69_Y + (NOTE_DELTA_Y * -7) + 7;
    y_delta = NOTE_DELTA_Y * -2;
  }
  else {
    y = MIDI_69_Y + (NOTE_DELTA_Y * 5) + 6;
    y_delta = NOTE_DELTA_Y * 2;
  }
  
  for(int i = 0; i < num_ledger_lines; i++) {
    g.drawLine(start_x, y, end_x, y);
    y = y + y_delta;
  }
}

void GrandStaffComponent::drawLedgerLinesOnBassClef(Graphics& g, int num_ledger_lines, bool above_clef) {
  int start_x = NOTE_X - 1;
  int end_x = start_x + 15;
  int y = 0;
  int y_delta = 0;
  if(above_clef) {
    y = MIDI_48_Y + (NOTE_DELTA_Y * -7) + 7;
    y_delta = NOTE_DELTA_Y * -2;
  }
  else {
    y = MIDI_48_Y + (NOTE_DELTA_Y * 5) + 6;
    y_delta = NOTE_DELTA_Y * 2;
  }
  
  for(int i = 0; i < num_ledger_lines; i++) {
    g.drawLine(start_x, y, end_x, y);
    y = y + y_delta;
  }
}

// Positive when the note is above the reference.
int GrandStaffComponent::getLetterDistance(int note, int ref_pitch) {
  if(note == ref_pitch) {
    return 0;
  }
  
  int note_letter = getNoteLetter(note);
  int ref_letter = getNoteLetter(ref_pitch);
  
  // This allows Ab/A to count as an octave in the treble clef, and C#/C to be an octave in bass cleff.
  int dst = abs(note - ref_pitch);
  if(ref_letter == note_letter) {
    dst = dst + 1;
  }
  
  int distance_octaves = dst / 12;

  int distance_within_octave = 0;
  if(note > ref_pitch) {
    distance_within_octave = lettersAboveReference(note_letter, ref_letter);
  }
  else {
    distance_within_octave = lettersBelowReference(note_letter, ref_letter);
  }
  
  int distance = (distance_octaves * OCTAVE_DELTA) + distance_within_octave;
  return note > ref_pitch ? distance : -1 * distance;
}

Letter GrandStaffComponent::getNoteLetter(int note) {
  // Midi pitch 69 is A, so wrapped note 9 is A.
  int wrapped_note = note % 12;
  switch(wrapped_note) {
    // C
    case 0: {
      return C;
    }
    // C# / Db
    case 1: {
      if(accidental_mode == ALL_SHARPS) {
        return C;
      }
      if(accidental_mode == ALL_FLATS) {
        return D;
      }
      break;
    }
    // D
    case 2: {
      return D;
    }
    // D# / Eb
    case 3: {
      if(accidental_mode == ALL_SHARPS) {
        return D;
      }
      if(accidental_mode == ALL_FLATS) {
        return E;
      }
      break;
    }
    // E
    case 4: {
      return E;
    }
    // F
    case 5: {
      return F;
    }
    // F# / Gb
    case 6: {
      if(accidental_mode == ALL_SHARPS) {
        return F;
      }
      if(accidental_mode == ALL_FLATS) {
        return G;
      }
      break;
    }
    // G
    case 7: {
      return G;
    }
    // G# / Ab
    case 8: {
      if(accidental_mode == ALL_SHARPS) {
        return G;
      }
      if(accidental_mode == ALL_FLATS) {
        return A;
      }
      break;
    }
    // A
    case 9: {
      return A;
    }
    // A# / Bb
    case 10: {
      if(accidental_mode == ALL_SHARPS) {
        return A;
      }
      if(accidental_mode == ALL_FLATS) {
        return B;
      }
      break;
    }
    // B
    case 11: {
      return B;
    }
  }
  return MISTAKE;
}

// Always positive. E.g. note_letter = C, ref_letter = G, returns 3.
int GrandStaffComponent::lettersAboveReference(int note_letter, int ref_letter) {
  if(note_letter < ref_letter) {
    note_letter = note_letter + OCTAVE_DELTA;
  }
  return note_letter - ref_letter;
}

// Always positive. E.g. note_letter = C, ref_letter = G, returns 4.
int GrandStaffComponent::lettersBelowReference(int note_letter, int ref_letter) {
  if(note_letter > ref_letter) {
    note_letter = note_letter - OCTAVE_DELTA;
  }
  return ref_letter - note_letter;
}
