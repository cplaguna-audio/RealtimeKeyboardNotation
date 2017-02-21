/*
 * MainComponent.cpp file header.
 *
 * Chris Laguna
 */

#include "MainContentComponent.h"
#include "Callbacks.cpp"

/***** Public members *****/

MainContentComponent::MainContentComponent() : last_input_index(0),
    keyboard_component(keyboard_state, MidiKeyboardComponent::horizontalKeyboard) {
  setOpaque(true);

  addAndMakeVisible(midi_input_list);
  midi_input_list.setTextWhenNoChoicesAvailable("No MIDI Inputs Enabled");
  const StringArray midi_inputs(MidiInput::getDevices());
  midi_input_list.addItemList(midi_inputs, 1);
  midi_input_list.addListener(this);
  
  addAndMakeVisible(accidental_mode_list);
  accidental_mode_list.addItem("All Sharps", ALL_SHARP_ID);
  accidental_mode_list.addItem("All Flats", ALL_FLAT_ID);
  accidental_mode_list.setText("All Sharps");
  accidental_mode_list.addListener(this);
  
  
  // First enabled device is selected by default.
  for (int i = 0; i < midi_inputs.size(); i++) {
    if (device_manager.isMidiInputEnabled(midi_inputs[i])) {
      setMidiInput(i);
      break;
    }
  }
  
  // Select first device if none are enabled.
  if (midi_input_list.getSelectedId() == 0) {
    setMidiInput(0);
  }
  
  keyboard_component.setAvailableRange(MIN_NOTE, MAX_NOTE);
  keyboard_component.setColour(MidiKeyboardComponent::keyDownOverlayColourId, Colour(41, 180, 51));
  addAndMakeVisible(keyboard_component);
  keyboard_state.addListener(this);

  addAndMakeVisible(grand_staff_component);

  setSize(848, 400);
}

MainContentComponent::~MainContentComponent() {
  keyboard_state.removeListener(this);
  device_manager.removeMidiInputCallback(MidiInput::getDevices()[midi_input_list.getSelectedItemIndex()], this);
  midi_input_list.removeListener(this);
}

int MainContentComponent::getMinNote() {
  return MIN_NOTE;
}

int MainContentComponent::getMaxNote() {
  return MAX_NOTE;
}

void MainContentComponent::paint(Graphics& g) {
    g.fillAll(Colours::grey);
}

void MainContentComponent::resized() {
    Rectangle<int> area(getLocalBounds());
    accidental_mode_list.setBounds(5, 5, 280, 25);
    midi_input_list.setBounds(300, 5, 500, 25);
    keyboard_component.setBounds(5, 35, 832, 80);
    grand_staff_component.setBounds(5, 130, 832, 250);
}

/***** Private members *****/

void MainContentComponent::setMidiInput(int index) {
  const StringArray list(MidiInput::getDevices());
  
  device_manager.removeMidiInputCallback(list[last_input_index], this);
  const String new_input(list[index]);
  
  if (!device_manager.isMidiInputEnabled(new_input)) {
    device_manager.setMidiInputEnabled(new_input, true);
  }
  
  device_manager.addMidiInputCallback(new_input, this);
  midi_input_list.setSelectedId(index + 1, dontSendNotification);
  
  last_input_index = index;
}

void MainContentComponent::comboBoxChanged(ComboBox* box) {
  if(box == &midi_input_list) {
    setMidiInput(midi_input_list.getSelectedItemIndex());
  }
  if(box == &accidental_mode_list) {
    int selected_id = accidental_mode_list.getSelectedId();
    if(selected_id == ALL_FLAT_ID) {
      grand_staff_component.setAccidentalMode(ALL_FLATS);
    }
    if(selected_id == ALL_SHARP_ID) {
      grand_staff_component.setAccidentalMode(ALL_SHARPS);
    }
  }
}

void MainContentComponent::handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message) {
  keyboard_state.processNextMidiEvent(message);
}

void MainContentComponent::handleNoteOn(MidiKeyboardState*, int midi_channel, int midi_note_number, float velocity) {
  (new AddNoteCallback(&grand_staff_component, midi_note_number))->post();
}

void MainContentComponent::handleNoteOff(MidiKeyboardState*, int midi_channel, int midi_note_number, float velocity) {
  (new RemoveNoteCallback(&grand_staff_component, midi_note_number))->post();
}
