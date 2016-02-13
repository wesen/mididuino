from _Framework.ControlSurface import ControlSurface
from _Framework.TransportComponent import TransportComponent
from _Framework.ButtonElement import ButtonElement

ABLETON = {
    "GENERAL_CHANNEL": 0,
    
    "START_NOTE": 1,
    "STOP_NOTE":  2,
    
    "GET_TRACK_NAME": 1,
    "SEND_TRACK_NAME": 2,

    "GET_ALL_TRACK_NAMES": 3,
}

RW_HEADER = (0xf0, 0x00, 0x13, 0x79)

class MiniCommand(ControlSurface):
    def __init__(self, c_instance):
        ControlSurface.__init__(self, c_instance)
        transport = TransportComponent()
        transport.set_play_button(ButtonElement(True, 0, ABLETON["GENERAL_CHANNEL"], ABLETON["START_NOTE"]))
        transport.set_stop_button(ButtonElement(True, 0, ABLETON["GENERAL_CHANNEL"], ABLETON["STOP_NOTE"]))
        self.log_message("MiniCommand surface")
        self.get_track_name(0)

    def get_track_name(self, idx):
        tracks = self.song().tracks
        if len(tracks) > idx:
            track = tracks[idx]
            name_bytes = map(lambda x: ord(x), track.name)
            self._send_midi(RW_HEADER +
                            (ABLETON["SEND_TRACK_NAME"], idx) + tuple(name_bytes) + (0,) +
                            (0xf7,))
            

    def handle_sysex(self, midi_bytes):
        if not ((len(midi_bytes) > 5) and
                (midi_bytes[1:4] == (0, 19, 121))):
            return
        cmd = midi_bytes[4]
        if (cmd == ABLETON["GET_TRACK_NAME"]):
            self.get_track_name(midi_bytes[5])
        self.log_message("received sysex %s" % list(midi_bytes))
