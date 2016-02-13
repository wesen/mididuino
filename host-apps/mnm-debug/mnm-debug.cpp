#include "Platform.h"
#include <MNM.h>

void usage() {
  fprintf(stderr, "Usage: ./mnm-debug -i input -o output [-l]\n");
}

class DebugCallbacks : public MNMCallback {
public:
  DebugCallbacks() {
  }

  void onKitMessage() {
    MNMKit kit;
    kit.fromSysex(MidiSysex.data + 5, MidiSysex.recordLen - 5);
    printf("got kit \"%s\"\n", kit.name);
  }
};

int main(int argc, char *argv[]) {
  int outputDevice = -1;
  int inputDevice = -1;

  int c;
  while ((c = getopt(argc, argv, "hli:o:")) != -1) {
    switch (c) {
    case 'l':
      printf("Input MIDI devices:\n");
      MidiUart.listInputMidiDevices();
      printf("\nOutput MIDI devices:\n");
      MidiUart.listOutputMidiDevices();
      exit(0);
      break;

    case 'i':
      inputDevice = atoi(optarg);
      break;

    case 'o':
      outputDevice = atoi(optarg);
      break;

    case 'h':
    default:
      usage();
      exit(0);
      break;
    }
  }

  if ((outputDevice == -1) || (inputDevice == -1)) {
    usage();
    exit(1);
  }

  DebugCallbacks debugCallbacks;

  MidiUart.init(inputDevice, outputDevice);
  MNMSysexListener.setup();
  MNMSysexListener.addOnKitMessageCallback(&debugCallbacks, (mnm_callback_ptr_t)&DebugCallbacks::onKitMessage);

  for (;;) {
    uint8_t track = MNM.getCurrentTrack(1000);
    unsigned int currentKit = MNM.getCurrentKit(1000);
    unsigned int currentTrack = MNM.getCurrentTrack(1000);
//    unsigned int currentPattern = MNM.requestGlobal()
    printf("current mnm track %d, current kit %d\n", currentTrack, currentKit);
    MNM.requestKit(currentKit);
    sleep(2);
  }
}
