import Live
from Tracing import Traced

#class MidiCommandListener(Traced):
class MidiCommandListener:
    __module__ = __name__
    __doc__ = 'MidiCommand Listener'

    songListeners = [
        "appointed_device",
        "back_to_arranger",
        "can_jump_to_next_cue",
        "can_jump_to_prev_cue",
        "clip_trigger_quantization",
        "cue_points",
        "current_song_time",
        "groove_amount",
        "is_playing",
        "loop_length",
        "loop",
        "loop_start",
        "metronome",
        "midi_recording_quantization",
        "nudge_down",
        "nudge_up",
        "overdub",
        "punch_in",
        "punch_out",
        "record_mode",
        "scenes",
        "signature_denominator",
        "signature_numerator",
        "song_length",
        "tempo",
        "tracks",
        "visible_tracks"
        ]

    trackListeners = [
        "arm",
        "clip_slots",
        "current_input_routing",
        "current_input_sub_routing",
        "current_monitoring_state",
        "current_output_routing",
        "current_output_sub_routing",
        "devices",
        "fire_slod_index",
        "has_audio_input",
        "has_audio_output",
        "has_midi_input",
        "has_midi_output",
        "input_meter_left",
        "input_meter_level",
        "input_meter_right",
        "mute",
        "name",
        "output_meter_left",
        "output_meter_level",
        "output_meter_right",
        "playing_slot_index",
        "solo"
        ]

    sceneListeners = [
        "clip_slots",
        "color",
        "is_triggered",
        "name"
        ]

    
       

    listener_list = {}
    
    def __init__(self, parent, obj, listeners):
        self.parent = parent
        self.obj = obj
        self.listeners = listeners
        MidiCommandListener.realinit(self)

    def add_listener(self, listener):
        if listener in self.listener_list:
            self.remove_listener(listener)

        cb_function = (lambda (name): lambda : self.print_attr(name))(listener)
        add_function = getattr(self.obj, "add_" + listener + "_listener")
        if add_function:
            add_function(cb_function)
            self.listener_list[listener] = cb_function
            self.parent.log("added listener " + listener)

    def remove_listener(self, listener):
        remove_function = getattr(self.obj, "remove_" + listener + "_listener")
        if remove_function:
            remove_function(self.listener_list[listener])
            del self.listener_list[listener]
            self.parent.log("removed listener " + listener)
            
    def realinit(self):
        for listener in self.listeners:
            self.add_listener(listener)
        self.parent.log("listener list: %s" % (self.listener_list))

    def print_attr(self, name):
        self.parent.log(name + (" callback: %s" % (getattr(self.obj, name))))

        
