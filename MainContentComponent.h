/*
 * MainContentComponent: The highest level component. Also accepts MIDI messages.
 *
 * Chris Laguna
 */

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "GrandStaffComponent.h"

class MainContentComponent : public Component,
                             private ComboBox::Listener,
                             private MidiInputCallback,
                             private MidiKeyboardStateListener {
public:
  MainContentComponent();
  virtual ~MainContentComponent();

  void paint (Graphics&) override;
  void resized() override;
  
  static int getMinNote();
  static int getMaxNote();

private:
  // Accidental types (see GrandStaffComponent.h)
  static const int ALL_SHARP_ID = 1;
  static const int ALL_FLAT_ID = 2;
  ComboBox accidental_mode_list;
  
  // For managing MIDI input device.
  AudioDeviceManager device_manager;
  ComboBox midi_input_list;
  Label midi_input_list_label;
  int last_input_index;
  
  // JUCE has a built-in keyboard component!
  MidiKeyboardState keyboard_state;
  MidiKeyboardComponent keyboard_component;
  
  // 88-key keyboard range.
  static const int MIN_NOTE = 21;
  static const int MAX_NOTE = 108;
  
  // For displaying staff notation.
  GrandStaffComponent grand_staff_component;
  
  // Select which MIDI input should we be listening to.
  void setMidiInput(int index);

  // Combobox event callbacks.
  void comboBoxChanged(ComboBox* box) override;
  
  // MIDI event callbacks.
  void handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message) override;
  void handleNoteOn(MidiKeyboardState*, int midi_channel, int midi_note_number, float velocity) override;
  void handleNoteOff(MidiKeyboardState*, int midi_channel, int midi_note_number, float velocity) override;
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


#endif  // MAINCOMPONENT_H_INCLUDED
