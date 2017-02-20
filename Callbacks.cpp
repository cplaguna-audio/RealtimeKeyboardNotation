/*
 * A bunch of callbacks, allowing us to call functions on the Message thread from other threads.
 * Chris Laguna
 */

#ifndef CALLBACKS_CPP_INCLUDED
#define CALLBACKS_CPP_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "GrandStaffComponent.h"

// To add a note to the GrandStaffComponent.
class AddNoteCallback : public CallbackMessage {
public:
  AddNoteCallback (GrandStaffComponent* o, int m) : owner(o), midi_pitch(m) {}

  void messageCallback() override {
    if (owner != nullptr)
    owner->addNote(midi_pitch);
  }

  Component::SafePointer<GrandStaffComponent> owner;
  int midi_pitch;
};

// To remove a note from the grand staff component.
class RemoveNoteCallback : public CallbackMessage {
public:
  RemoveNoteCallback (GrandStaffComponent* o, int m) : owner(o), midi_pitch(m) {}

  void messageCallback() override {
    if (owner != nullptr)
    owner->removeNote(midi_pitch);
  }

  Component::SafePointer<GrandStaffComponent> owner;
  int midi_pitch;
};

#endif // CALLBACKS_CPP_INCLUDED
