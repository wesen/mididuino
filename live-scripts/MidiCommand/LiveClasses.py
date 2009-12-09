from MidiConversion import *

LiveClassesById = {}
LiveClassesByName = {}

def getClassById(id):
    return LiveClassesById[id]

def getClassByName(name):
    return LiveClassesByName[name]

def getObjectClass(object):
    return getClassByName(type(object).__name__)

def attributeToMidi(object, attr, value):
    _class = getObjectClass(object)
    if _class != None:
        liveattr = _class.getAttribute(attr)
        if liveattr != None:
            return (PRINT_ATTRIBUTE_FUNC_ID, _class.id) + MidiConverter.toMidi(object) + (liveattr.id) + liveattr.toMidi()
    return ()

class LiveClass:
    __module__ = __name__
    __doc__ = 'Generic Introspection Class for Live Objects'

    attributes = []
    listeners = []

    def __init__(self, _id, _name, _listeners, _attributes, _methods):
        self.id = _id
        self.name = _name
        LiveClassesById[self.id] = self
        LiveClassesByName[self.name] = self
        self.listeners = _listeners
        self.attributes = _attributes
        self.methods = _methods

    def getListener(self, name):
        for listener in self.listeners:
            if listener.name == name:
                return listener
        return None

    def getListenerId(self, name):
        for listener in self.listeners:
            if listener.name == name:
                return listener.id
        return None

    def getAttribute(self, name):
        for attr in self.attributes:
            if attr.name == name:
                return attr
        return None

    def getMethod(self, name):
        for method in self.methods:
            if method.name == name:
                return attr

class LiveAttribute:
    __module__ = __name__
    __doc__ = 'Generic introspection class for Live attribute'

    def __init__(self, _name, _type, _id, _hasListener):
        self.name = _name
        self.type = _type
        self.id = _id
        self.hasListener = _hasListener

    def toMidi(self, value):
        return MidiConverter.toMidi(value, self.type)

class LiveListener:
    __module__ = __name__
    __doc__ = 'Generici ntrospection class for Live listeners'

    def __init__(self, _name, _id, _function = None):
        self.name = _name
        self.id = _id
        self.function = _function

class LiveMethod:
    __module__ = __name__
    __doc__ = 'Generic introspection class for Live Methods'

    def __init__(self, _name, _id, _params, _returnType):
        self.name = _name
        self.id = _id
        self.params = _params
        self.returnType = _returnType
        
class LiveClasses:
    __module__ = __name__
    __doc__ = 'Generic Introspection Class for Live Objects'

    classes = {}
    classesByName = {}

    DeviceClass = LiveClass(
        1,
        "Device",
        
        [ LiveListener("name",       1),
          LiveListener("parameters", 2) ],
        [ LiveAttribute("class_name", "string", 1, False),
          LiveAttribute("name", "string", 2, True),
          LiveAttribute("parameters", "paramList", 3, True) ],
        [ LiveMethod("store_chosen_bank", 1, ["ui7", "ui7"], []) ])


    DeviceParameterClass = LiveClass(
        2,
        "DeviceParameter",
        
        [ LiveListener("value", 1) ],
        [ LiveAttribute("default_value", "float",  1, False),
          LiveAttribute("is_enabled",    "bool",   2, False),
          LiveAttribute("is_quantized",  "bool",   3, False),
          LiveAttribute("max",           "float",  4, False),
          LiveAttribute("min",           "float",  5, False),
          LiveAttribute("name",          "string", 6, False),
          LiveAttribute("original_name", "string", 7, False),
          LiveAttribute("value",         "float",  8, True),
          ],
        [])

    MixerDeviceClass = LiveClass(
        3,
        "MixerDevice",
        
        [ LiveListener("crossfade_assign", 1),
          LiveListener("sends", 2) ],
        [ LiveAttribute("crossfade_assign", "ui7", 1, True),
          LiveAttribute("crossfader", "param", 2, False),
          LiveAttribute("cue_volume", "param", 3, False),
          LiveAttribute("panning", "param", 4, False),
          LiveAttribute("sends", "paramList", 5, True),
          LiveAttribute("volume", "param", 6, False) ],
        [])

    SceneClass = LiveClass(
        4,
        "Scene",
        
        [ LiveListener("clip_slots", 1),
          LiveListener("color", 2),
          LiveListener("is_triggered", 3),
          LiveListener("name", 4) ],
        [ LiveAttribute("clip_slots", "slotList", 1, False),
          LiveAttribute("color", "ui28", 2, True),
          LiveAttribute("is_triggered", "bool", 3, True),
          LiveAttribute("name", "string", 4, True),
          LiveAttribute("tempo", "float", 5, False) ],
        [ LiveMethod("fire", 1, [], []),
          LiveMethod("fire_as_selected", 2, [], []),
          LiveMethod("set_fire_button_state", 3, ["bool"], []) ])

    ClipSlotClass = LiveClass(
        5,
        "ClipSlot",
        
        [ LiveListener("controls_other_clips", 1),
          LiveListener("has_clip", 2),
          LiveListener("has_stop_button", 3),
          LiveListener("is_triggered", 4),
          LiveListener("playing_status", 5) ],
        [ LiveAttribute("clip", 1, "Clip", False),
          LiveAttribute("controls_other_clips", 2, "bool", True),
          LiveAttribute("has_clip", 3, "bool", True),
          LiveAttribute("is_playing", 4, "bool", False),
          LiveAttribute("is_recording", 5, "bool", False),
          LiveAttribute("is_triggered", 6, "bool", True),
          LiveAttribute("playing_status", 7, "XXX", True),
          LiveAttribute("will_record_on_start", 8, "bool", False) ],
        [ LiveMethod("fire", 1, [], []),
          LiveMethod("set_fire_button_state", 2, ["bool"], []),
          LiveMethod("stop", 3, [], []) ])

    SongClass = LiveClass(
        6,
        "Song",
        
        [ LiveListener("appointed_device", 1),
          LiveListener("back_to_arranger", 2),
          LiveListener("can_jump_to_next_cue", 3),
          LiveListener("can_jump_to_prev_cue", 4),
          LiveListener("can_redo", 5),
          LiveListener("can_undo", 6),
          LiveListener("clip_trigger_quantization", 7),
          LiveListener("cue_points", 8),
          LiveListener("current_song_time", 9),
          LiveListener("groove_amount", 10),
          LiveListener("is_playing", 11),
          LiveListener("loop", 12),
          LiveListener("loop_length", 13),
          LiveListener("loop_start", 14),
          LiveListener("metronome", 15),
          LiveListener("midi_recording_quantization", 16),
          LiveListener("nudge_down", 17),
          LiveListener("nudge_up", 18),
          LiveListener("overdub", 19),
          LiveListener("punch_in", 20),
          LiveListener("punch_out", 21),
          LiveListener("record_mode", 22),
          LiveListener("scenes", 23),
          LiveListener("signature_denominator", 24),
          LiveListener("signature_numerator", 25),
          LiveListener("song_length", 26),
          LiveListener("tempo", 27),
          LiveListener("tracks", 28),
          LiveListener("visible_tracks", 29) ],

        [ LiveAttribute("appointed_device", 1, "device", True),
          LiveAttribute("back_to_arranger", 2, "bool", True),
          LiveAttribute("can_jump_to_next_cue", 3, "bool", True),
          LiveAttribute("can_jump_to_prev_cue", 4, "bool", True),
          LiveAttribute("can_redo", 5, "bool", False),
          LiveAttribute("can_undo", 6, "bool", False),
          LiveAttribute("clip_trigger_quantization", 7, "XXX", True),
          LiveAttribute("cue_points", 8, "XXX", True),
          LiveAttribute("current_song_time", 9, "float", True),
          LiveAttribute("exclusive_arm", 10, "bool", False),
          LiveAttribute("exclusive_solo", 11, "bool", False),
          LiveAttribute("groove_amount", 12, "ui7", True),
          LiveAttribute("is_playing", 13, "bool", True),
          LiveAttribute("last_event_time", 14, "float", False),
          LiveAttribute("loop", 15, "bool", True),
          LiveAttribute("loop_length", 16, "float", True),
          LiveAttribute("loop_start", 17, "float", True),
          LiveAttribute("master_track", 18, "Track", False),
          LiveAttribute("metronome", 19, "bool", True),
          LiveAttribute("midi_recording_quantization", 20, "XXX", True),
          LiveAttribute("nudge_down", 21, "bool", True),
          LiveAttribute("nudge_up", 22, "bool", True),
          LiveAttribute("overdub", 23, "bool", True),
          LiveAttribute("punch_in", 24, "bool", True),
          LiveAttribute("punch_out", 25, "bool", True),
          LiveAttribute("record_mode", 26, "bool", True),
          LiveAttribute("scenes", 27, "sceneList", True),
          LiveAttribute("select_on_launch", 28, "bool", False),
          LiveAttribute("signature_denominator", 29, "ui7", True),
          LiveAttribute("signature_numerator", 30, "ui7", True),
          LiveAttribute("song_length", 31, "float", True),
          LiveAttribute("tempo", 32, "float", True),
          LiveAttribute("tracks", 33, "trackList", True),
          LiveAttribute("view", 34, "view", False),
          LiveAttribute("visible_tracks", 35, "trackList", True) ],

        [ LiveMethod("continue_playing", 1, [], []),
          LiveMethod("get_beats_loop_length", 2, [], ["BeatTime"]),
          LiveMethod("get_beats_loop_start", 3, [], ["BeatTime"]),
          LiveMethod("get_current_beats_song_time", 4, [], ["BeatTime"]),
          LiveMethod("get_current_smpte_song_time", 5, [], ["BeatTime"]),
          LiveMethod("is_cue_point_selected", 6, [], ["bool"]),
          LiveMethod("jump_by", 7, ["float"], []),
          LiveMethod("jump_to_next_cue", 8, [], []),
          LiveMethod("jump_to_prev_cue", 9, [], []),
          LiveMethod("play_selection", 10, [], []),
          LiveMethod("redo", 11, [], []),
          LiveMethod("scrub_by", 12, ["float"], []),
          LiveMethod("set_or_delete_cue", 13, [], []),
          LiveMethod("start_playing", 14, [], []),
          LiveMethod("stop_all_clips", 15, [], []),
          LiveMethod("stop_playing", 16, [], []),
          LiveMethod("tap_tempo", 17, [], []),
          LiveMethod("undo", 18, [], []),
          ] )

    TrackClass = LiveClass(
        7,
        "Track",
        
        [ LiveListener("arm", 1),
          LiveListener("clip_slots", 2),
          LiveListener("current_input_routing", 3),
          LiveListener("current_input_sub_routing", 4),
          LiveListener("current_monitoring_state", 5),
          LiveListener("current_output_routing", 6),
          LiveListener("current_output_sub_routing", 7),
          LiveListener("devices", 8),
          LiveListener("fired_slot_index", 9),
          LiveListener("has_audio_input", 10),
          LiveListener("has_audio_output", 11),
          LiveListener("has_midi_input", 12),
          LiveListener("has_midi_output", 13),
          LiveListener("input_meter_left", 14),
          LiveListener("input_meter_right", 15),
          LiveListener("mute", 16),
          LiveListener("name", 17),
          LiveListener("output_meter_left", 18),
          LiveListener("output_meter_level", 19),
          LiveListener("output_meter_right", 20),
          LiveListener("playing_slot_index", 21),
          LiveListener("solo", 22) ],
        [ LiveAttribute("arm", 1, "bool", True),
          LiveAttribute("can_be_armed", 2, "bool", False),
          LiveAttribute("clip_slots", 3, "clipSlotList", True),
          LiveAttribute("current_input_routing", 4, "", True),
          LiveAttribute("current_input_sub_routing", 5, "", True),
          LiveAttribute("current_monitoring_state", 6, "", True),
          LiveAttribute("current_output_routing", 7, "", True),
          LiveAttribute("current_output_sub_routing", 8, "", True),
          LiveAttribute("devices", 9, "deviceList", True),
          LiveAttribute("fired_slot_index", 10, "ui14", True),
          LiveAttribute("fold_state", 11, "bool", False),
          LiveAttribute("has_audio_input", 12, "bool", True),
          LiveAttribute("has_audio_output", 13, "bool", True),
          LiveAttribute("has_midi_input", 14, "bool", True),
          LiveAttribute("has_midi_output", 15, "bool", True),
          LiveAttribute("input_meter_left", 16, "float", True),
          LiveAttribute("input_meter_level", 17, "float", True),
          LiveAttribute("input_meter_right", 18, "float", True),
          LiveAttribute("input_routings", 19, "", False),
          LiveAttribute("input_sub_routings", 20, "", False),
          LiveAttribute("is_foldable", 21, "bool", False),
          LiveAttribute("is_part_of_selection", 22, "bool", False),
          LiveAttribute("is_visible", 23, "bool", False),
          LiveAttribute("mixer_device", 24, "MixerDevice", False),
          LiveAttribute("mute", 25, "bool", True),
          LiveAttribute("name", 26, "string", True),
          LiveAttribute("output_meter_left", 27, "float", True),
          LiveAttribute("output_meter_level", 28, "float", True),
          LiveAttribute("output_meter_right", 29, "float", True),
          LiveAttribute("output_routings", 30, "XXX", False),
          LiveAttribute("output_sub_routings", 31, "XXX", False),
          LiveAttribute("playing_slot_index", 32, "ui14", True),
          LiveAttribute("solo", 33, "bool", True),
          LiveAttribute("view", 34, "View", False) ],
        [ LiveMethod("jump_in_running_session_clip", 1, ["float"], []),
          LiveMethod("stop_all_clips", 2, [], []),
          ])

    ClipClass = LiveClass(
        8,
        "Clip",
        
        [ LiveListener("color",                 1),
          LiveListener("is_recording",          2),
          LiveListener("loop_end",              3),
          LiveListener("loop_jump",             4),
          LiveListener("loop_start",            5),
          LiveListener("looping",               6),
          LiveListener("muted",                 7),
          LiveListener("name",                  8),
          LiveListener("notes",                 9, "get_selected_notes"),
          LiveListener("pitch_coarse",          10),
          LiveListener("pitch_fine",            11),
          LiveListener("playing_position",      12),
          LiveListener("playing_status",        13, "is_playing"),
          LiveListener("signature_denominator", 14),
          LiveListener("signature_numerator",   15),
          LiveListener("warping",               16),
          ],
        
        [ LiveAttribute("color",                 "ui28",   1,  True),
          LiveAttribute("is_audio_clip",         "bool",   2,  False), 
          LiveAttribute("is_midi_clip",          "bool",   3,  False),
          LiveAttribute("is_playing",            "bool",   4,  False),
          LiveAttribute("is_recording",          "bool",   5,  True),
          LiveAttribute("is_triggered",          "bool",   6,  False),
          LiveAttribute("length",                "float",  7,  False),
          LiveAttribute("loop_end",              "float",  8,  True),
          LiveAttribute("loop_start",            "float",  9,  True),
          LiveAttribute("looping",               "bool",   10, True),
          LiveAttribute("muted",                 "bool",   11, True),
          LiveAttribute("name",                  "string", 12, True),
          LiveAttribute("pitch_coarse",          "ui7",    13, True),
          LiveAttribute("pitch_fine",            "float",  14, True),
          LiveAttribute("playing_position",      "float",  15, True),
          LiveAttribute("signature_denominator", "ui7",    16, True),
          LiveAttribute("signature_numerator",   "ui7",    17, True),
          LiveAttribute("warping",               "bool",   18, True),
          LiveAttribute("will_record_on_start",  "bool",   19, False) ],

        [ LiveMethod("deselect_all_notes",     1, [],           []),
          LiveMethod("fire",                   2, [],           []),
          LiveMethod("get_selected_notes",     3, [],           ["noteList"]),
          LiveMethod("move_playing_pos",       4, ["float"],    []),
          LiveMethod("replace_selected_notes", 5, ["noteList"], []),
          LiveMethod("select_all_notes",       6, [],           []),
          LiveMethod("set_fire_button_state",  7, ["bool"],     []),
          LiveMethod("stop",                   8, [],           []) ])
