#import Live
from consts import *
from Tracing import Traced

#class MidiCommandComponent(Traced):
class MidiCommandComponent:
    __module__ = __name__
    __doc__ = 'Baseclass for a subcomponent for MidiCommand controllers.'
    __filter_funcs__ = ["update_display", "log"]

    def __init__(self, parent):
	MidiCommandComponent.realinit(self, parent)

    def realinit(self, parent):
	self.parent = parent
	self.helper = parent.helper
	self.param_map = parent.param_map

    def log(self, string):
	self.parent.log(string)

    def logfmt(self, fmt, *args):
	args2 = []
	for i in range(0, len(args)):
	    args2 += [args[i].__str__()]
#	    if (isinstance(args2[i], unicode)):
#		args2[i] = args2[i].encode("latin-1")
	str = fmt % tuple(args2)
	return self.log(str)
	
    def application(self):
	return self.parent.application()

    def song(self):
	return self.parent.song()

    def send_midi(self, midi_event_bytes):
	self.parent.send_midi(midi_event_bytes)

    def request_rebuild_midi_map(self):
	self.parent.request_rebuild_midi_map()

    def disconnect(self):
	pass

    def build_midi_map(self, script_handle, midi_map_handle):
	pass

    def receive_midi_cc(channel, cc_no, cc_value):
	pass

    def receive_midi_note(channel, status, note, velocity):
	pass

    def refresh_state(self):
	pass

    def update_display(self):
	pass

