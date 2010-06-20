#if defined(MIDI_LINK)
#include "midi_link_desc.h"
#else
#if defined(ISMART)
#include "midi_link_desc.h"
#else
#if defined(RADIUM)
#include "midi_link_desc.h"
#endif // RADIUM
#endif // ISMART
#endif // MIDI_LINK
