import Live
from consts import *
import sys
from MidiCommandHelper import MidiCommandHelper
from ParamMap import ParamMap
from Devices import *
from Tracing import Traced
from DumpXML import dumpXML

#class MidiCommandScript(Traced):
class MidiCommandScript:
    __filter_funcs__ = ["update_display", "exec_commands", "log", "song"]
    __module__ = __name__
    __doc__ = 'Automap script for MidiCommand controllers'
#    __myDebug__ = False
    __myDebug__ = True
    __version__ = "V1.1"
    __name__ = "Generic MidiCommand Script"
    
    def __init__(self, c_instance):
	self.suffix = ""
        self.is_lv1 = False
	MidiCommandScript.realinit(self, c_instance)
        
    def realinit(self, c_instance):
	if self.__myDebug__:
            if sys.platform == "win32":
                self.file = open("C:/MidiCommand" + self.suffix + ".txt", "a")
                self.commandfile = "C:/MidiCommand-cmd" + self.suffix + ".txt"
            else:
                self.file = open("/tmp/MidiCommand" + self.suffix, "a")
                self.commandfile = ("/tmp/MidiCommand-cmd" + self.suffix)
	    self.log("MidiCommand")
	    self.log("command file %s" % self.commandfile)
            if (self.is_live_5()):
                self.log("live 5")
            else:
                self.log("live 6")
	    
	self.c_instance = c_instance
	self.helper = MidiCommandHelper(self)
	self.param_map = ParamMap(self)
	self.mixer_controller = None
	self.device_controller = None
	self.transport_controller = None
	self.components = []
        live = "Live 6 & 7"
        if self.is_live_5():
            live = "Live 5"
        self.show_message(self.__name__ + " " + self.__version__ + " for " + live)

    def dump_documentation(self):
        for i in ["Application.Application", "Clip.Clip", "ClipSlot.ClipSlot", "Device.Device", "DeviceParameter.DeviceParameter", "MidiMap", "MixerDevice.MixerDevice", "Scene.Scene", "Song.Song", "Song.BeatTime", "Song.CuePoint", "Song.Quantization", "Song.SmptTime", "Song.TimeFormat", "Track.Track"]:
            dumpXML(eval("Live." + i), "/tmp/" + i + ".xml")

    def is_live_5(self):
        return hasattr(Live, 'is_live_5')
        
    def dump_track(self, track):
        file = open("/tmp/trackdump.txt", "w")
        for device in track.devices:
            self.log("device: %s" % device.name)
            file.write("device: %s\n" % device.name)
            if (hasattr(device, "class_name")):
                file.write("classname: %s\n" % device.class_name)
            i = 0
            for param in device.parameters:
                self.log("param: %s: %s" % (i, param.name))
                file.write("param %s: %s\n" % (i, param.name))
                i += 1
            file.write("\n")
        file.close()

    def exec_commands(self):
	if not self.__myDebug__:
	    return
	filename = self.commandfile
	file = open(filename, "r")
	commands = file.readlines()
	for command in commands:
	    command = command.strip()
	    try:
                exec(command)
		result = eval(command).__str__()
		self.log("%s = %s" % (command, result))
	    except Exception, inst:
		self.log("exception: %s" % inst)
		try:
		    self.log("execing %s" % command)
		except Exception, inst:
		    self.log("Exception while executing %s: %s" % (command, inst))
	file.close()
	file = open(filename, "w")
	file.write("")
	file.close()
	
    def log(self, string):
	if self.__myDebug__:
#	    self.file.write("string type %s" % type(string))
#	    if (isinstance(string, unicode)):
#		self.file.write(string.encode("latin-1") + "\n")
#	    else:
            self.file.write(string + "\n")
	    self.file.flush()

    def logfmt(self, fmt, *args):
	args2 = []
	for i in range(0, len(args)):
	    args2 += [args[i].__str__()]
#	    if (isinstance(args2[i], unicode)):
#		args2[i] = args2[i].encode("latin-1")
	str = fmt % tuple(args2)
	return self.log(str)
	
	
    def disconnect(self):
#	self.song().remove_tracks_listener(self.on_tracks_changed)
	for c in self.components:
	    c.disconnect()

    def application(self):
	return Live.Application.get_application()

    def song(self):
	return self.c_instance.song()

    def suggest_input_port(self):
	return str('')

    def suggest_output_port(self):
	return str('')

    def can_lock_to_devices(self):
        self.log("can lock to devices")
	return True

    def can_lock_to_device(self):
        self.log("can lock to device")
	return True

    def lock_to_device(self, device):
        self.log("parent lock to device")
	if self.device_controller:
	    self.device_controller.lock_to_device(device)

    def unlock_to_device(self, device):
        self.log("unlock to device")
	if self.device_controller:
	    self.device_controller.unlock_from_device(device)
	    
    def set_appointed_device(self, device):
	if self.device_controller:
	    self.device_controller.set_appointed_device(device)

    def toggle_lock(self):
        self.log("toggle lock")
	self.c_instance.toggle_lock()

    def suggest_map_mode(self, cc_no):
	return Live.MidiMap.MapMode.absolute

    def restore_bank(self, bank):
	pass

    def show_message(self, message):
        if hasattr(self.c_instance, 'show_message'):
            self.c_instance.show_message(message)

    def instance_identifier(self):
	return self.c_instance.instance_identifier()

    def connect_script_instances(self, instanciated_scripts):
	pass

    def request_rebuild_midi_map(self):
	self.c_instance.request_rebuild_midi_map()

    def send_midi(self, midi_event_bytes):
	self.c_instance.send_midi(midi_event_bytes)

    def refresh_state(self):
	for c in self.components:
	    c.refresh_state()

    def build_midi_map(self, midi_map_handle):
        self.log("script build midi map")
	script_handle = self.c_instance.handle()
	self.param_map.remove_mappings()
	for c in self.components:
            self.log("build midi map on %s" % c)
	    c.build_midi_map(script_handle, midi_map_handle)

    def update_display(self):
	self.exec_commands()
	for c in self.components:
	    c.update_display()
	    
    def receive_midi(self, midi_bytes):
	channel = (midi_bytes[0] & CHAN_MASK)
	status = (midi_bytes[0] & STATUS_MASK)
	if (status == CC_STATUS):
	    cc_no = midi_bytes[1]
	    cc_value = midi_bytes[2]
	    for c in self.components:
		c.receive_midi_cc(channel, cc_no, cc_value)
	    self.param_map.receive_midi_cc(channel, cc_no, cc_value)
	elif (status == NOTEON_STATUS) or (status == NOTEOFF_STATUS):
	    note_no = midi_bytes[1]
	    note_vel = midi_bytes[2]
	    for c in self.components:
		c.receive_midi_note(channel, status, note_no, note_vel)
	    self.param_map.receive_midi_note(channel, status, note_no, note_vel)
	else:
	    assert False, ('Unknown MIDI message %s' % str(midi_bytes))

