/*
 * MidiCtrl - Host implementation of hardware interfacing stubs
 *
 * (c) July 2011 - Manuel Odendahl - wesen@ruinwesen.com
 */
#include "GUI_private.h"

EncodersClass Encoders;
ButtonsClass Buttons;

#define ENCODER_NORMAL(i) (encoders[(i)].normal)
#define ENCODER_SHIFT(i)  (encoders[(i)].shift)
#define ENCODER_BUTTON(i) (encoders[(i)].button)
#define ENCODER_BUTTON_SHIFT(i) (encoders[(i)].button_shift)

EncodersClass::EncodersClass() {
}
