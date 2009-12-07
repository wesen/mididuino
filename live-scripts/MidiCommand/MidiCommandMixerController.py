import Live
from ParamMap import ParamMap
from MidiCommandComponent import MidiCommandComponent
from consts import *

class MidiCommandMixerController(MidiCommandComponent):
    __module__ = __name__
    __doc__ = 'Mixer parameters of Midi Command'
    __filter_funcs__ = ["update_display", "log"]

    def __init__(self, parent):
	MidiCommandMixerController.realinit(self, parent)

    def realinit(self, parent):
	MidiCommandComponent.realinit(self, parent)
	self.on_track_selected_callback = lambda : self.on_track_selected()
	self.parent.song().view.add_selected_track_listener(self.on_track_selected_callback)
	self.track_selected = 0

    def disconnect(self):
	self.parent.song().view.remove_selected_track_listener(self.on_track_selected_callback)

    def on_track_selected(self):
	self.track_selected = 1
	if self.helper.is_master_track_selected():
            pass
	else:
            pass

    def map_track_params(self, script_handle, midi_map_handle):
        for idx in range(0, 4):
            tracks = self.parent.song().tracks + self.parent.song().return_tracks + (self.parent.song().master_track,)
            if (len(tracks) > idx):
                track = tracks[idx]
                mixer_device = track.mixer_device
                parameter = mixer_device.volume
                ParamMap.map_with_feedback(midi_map_handle, CHANNEL_MIDI_COMMAND, \
                                               PAGE1_CCS[idx], \
                                               parameter, Live.MidiMap.MapMode.absolute)

    def receive_midi_cc(self, channel, cc_no, cc_value):
        def rel_to_val(rel):
	    val = 0
	    if (cc_value >= 64):
		val = cc_value - 128
	    else:
		val = cc_value
            return val
            
        if (channel != CHANNEL_MIDI_COMMAND):
            return
        
        if (cc_no == SCENE_SCROLL_CC):
	    val = rel_to_val(cc_value)
	    idx = self.helper.selected_scene_idx() + val
	    new_scene_idx = min(len(self.parent.song().scenes) - 1, max(0, idx))
	    self.parent.song().view.selected_scene = self.parent.song().scenes[new_scene_idx]
        elif (cc_no == TRACK_SCROLL_CC):
            val = rel_to_val(cc_value)
            current_idx = self.helper.selected_track_idx()
            idx = current_idx + val
            tracks = self.parent.song().tracks + self.parent.song().return_tracks + (self.parent.song().master_track,)
            new_track_idx = min(len(tracks) - 1, max(0, idx))
            track = tracks[new_track_idx]
            if (current_idx != idx):
                self.parent.song().view.selected_track = track

    def build_midi_map(self, script_handle, midi_map_handle):
	def forward_note(chan, note):
	    Live.MidiMap.forward_midi_note(script_handle, midi_map_handle, chan, note)

	def forward_cc(chan, cc):
	    Live.MidiMap.forward_midi_cc(script_handle, midi_map_handle, chan, cc)
	idx = 0

        forward_cc(CHANNEL_MIDI_COMMAND, SCENE_SCROLL_CC)
        forward_cc(CHANNEL_MIDI_COMMAND, TRACK_SCROLL_CC)

	self.map_track_params(script_handle, midi_map_handle)
	
        self.on_track_selected()
        self.helper.send_page({'patch' : 1,
                               'page' : 0,
                               'type': 0,
                               'ccs' : PAGE1_CCS,
                               'channel' : [ CHANNEL_MIDI_COMMAND for i in range(0, 4)],
                               'macros' : [ [128, SCENE_SCROLL_CC],
                                            [0,   TRACK_SCROLL_CC],
                                            [0, 0],
                                            [0, 0]],
                               'names' : ['LV1 ', 'LV2 ', 'LV3 ', 'LV4 ']})

        track = self.parent.song().view.selected_track
        names = ['', '', '', '']
        if track:
            mixer_device = track.mixer_device
            names = ["LEV"]
            parameter = mixer_device.volume
            ParamMap.map_with_feedback(midi_map_handle, CHANNEL_MIDI_COMMAND, \
                                           PAGE2_CCS[0], \
                                           parameter, Live.MidiMap.MapMode.absolute)
            
            sends = mixer_device.sends
            for x in range(0, 1):
                if (len(sends) > x):
                    names.append("SD%s" % (x + 1))
                    parameter = sends[x]
                    ParamMap.map_with_feedback(midi_map_handle, \
                                                   CHANNEL_MIDI_COMMAND, \
                                                   PAGE2_CCS[x + 1], \
                                                   parameter, \
                                                   Live.MidiMap.MapMode.absolute)
                else:
                    names.append("")

            eq = self.helper.track_find_last_eq(track)
            if eq:
                names += ['LP', 'HP']
                params = self.helper.eq_params(eq)
                ParamMap.map_with_feedback(midi_map_handle, \
                                               CHANNEL_MIDI_COMMAND, \
                                               PAGE2_CCS[2], \
                                               params[0],\
                                               Live.MidiMap.MapMode.absolute)
                ParamMap.map_with_feedback(midi_map_handle, \
                                               CHANNEL_MIDI_COMMAND, \
                                               PAGE2_CCS[3], \
                                               params[2],\
                                               Live.MidiMap.MapMode.absolute)
            else:
                names += ['', '']
                
        self.helper.send_page({'patch' : 1,
                               'page' : 1,
                               'type': 0,
                               'ccs' : PAGE2_CCS,
                               'channel' : [ CHANNEL_MIDI_COMMAND for i in range(0, 4)],
                               'macros' : [ [128, SCENE_SCROLL_CC],
                                            [0,   TRACK_SCROLL_CC],
                                            [0, 0],
                                            [0, 0]],
                               'names': names})
