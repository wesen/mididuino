import Live
from MidiCommandComponent import MidiCommandComponent
from consts import *
import sys
from Devices import *
from DevicesXY import *
from ParamMap import ParamMap

class MidiCommandDeviceController(MidiCommandComponent):
    __module__ = __name__
    __doc__ = 'Class representing the device controller section of MidiCommand controllers'
    __filter_funcs__ = ["update_display", "log"]

    def __init__(self, parent):
	MidiCommandDeviceController.realinit(self, parent)

    def realinit(self, parent):
	MidiCommandComponent.realinit(self, parent)
	self.log("device controller %s" % parent)
        self.device = None
        self.olddevice = None
        if (hasattr(self.parent.song(), "appointed_device")):
            self.device = self.parent.song().appointed_device
        self.log("device %s" % self.device)
	self.device_locked = False
	self.bank = 0
	self.show_bank = False
	self.selected_clip = None
	self.on_track_selected_callback = lambda : self.on_track_selected()
	self.parent.song().view.add_selected_track_listener(self.on_track_selected_callback)
        self.selected_track = None
	
    def build_midi_map(self, script_handle, midi_map_handle):
	def forward_note(chan, note):
	    Live.MidiMap.forward_midi_note(script_handle, midi_map_handle, chan, note)

	def forward_cc(chan, cc):
	    Live.MidiMap.forward_midi_cc(script_handle, midi_map_handle, chan, cc)

        self.log("midi rebuild map")
	self.map_device_params(script_handle, midi_map_handle)

    def map_device_params(self, script_handle, midi_map_handle):
	def map_params_by_number(device):
	    ccs = PAGE3_CCS + PAGE4_CCS
	    channel = CHANNEL_MIDI_COMMAND
	    mode = Live.MidiMap.MapMode.absolute
	    for encoder in range(8):
		# +1 to skip "Device on"
		if (len(device.parameters) >= encoder + 1):
		    ParamMap.map_with_feedback(midi_map_handle, channel, \
					       ccs[encoder], \
					       device.parameters[encoder + 1], \
					       mode)

	def map_params_by_name(device, param_bank):
	    ccs = PAGE3_CCS + PAGE4_CCS
	    channel = CHANNEL_MIDI_COMMAND
	    mode = Live.MidiMap.MapMode.absolute
		
#	    self.log("ccs: %s" % ccs)
	    for encoder in range(8):
		if (len(params) >= encoder):
		    if (param_bank[encoder] == ''):
			continue
                    param_name = param_bank[encoder]
                    parameter = None
                    parameter = self.helper.get_parameter_by_name(self.device, param_bank[encoder])
		    if parameter:
			mode2 = mode

                        fullname = self.helper.device_name(device) + "." + parameter.name
			ParamMap.map_with_feedback(midi_map_handle, channel, \
						   ccs[encoder], parameter, \
						   mode2)
		    else:
			self.log("Could not find parameter %s" % param_bank[encoder])
	
        self.log("map device params %s" % self.device)
	if self.device:
	    params = self.device.parameters
	    device_bank = 0
	    param_bank = 0
            names = ['FX1', 'FX2', 'FX3', 'FX4',
                     'FX5', 'FX6', 'FX7', 'FX8']

            device_name = self.helper.device_name(self.device)
            self.log("device name '%s'" % device_name)
	    if (device_name in DEVICE_BOB_DICT.keys()):
		param_bank = DEVICE_BOB_DICT[device_name]
                # live 7 compressor 2 workaround
                if (device_name in DEVICE_BOB_NAMES_DICT.keys()):
                    names = DEVICE_BOB_NAMES_DICT[device_name]
                if (device_name == "Compressor2" and \
                    self.helper.get_parameter_by_name(self.device, 'Ext. In Gain')):
                    param_bank = CP2_BANK1_LIVE7
                    names = CP2_BOB_NAMES_LIVE7
		self.log("class %s bank: %s" % (device_name, param_bank))
		self.show_bank_select("Best of parameters")
		map_params_by_name(self.device, param_bank)
	    elif (self.helper.device_is_plugin(self.device)):
		self.show_bank_select("First eight parameters")
		map_params_by_number(self.device)
	    else:
                self.log("Could not find %s in %s" % (device_name, DEVICE_BOB_DICT.keys()))
		return
            
            self.helper.send_page({'patch' : 1,
                                   'page' : 2,
                                   'type': 0,
                                   'ccs' : PAGE3_CCS,
                                   'channel' : [ CHANNEL_MIDI_COMMAND for i in range(0, 4)],
                                   'macros' : [ [128, SCENE_SCROLL_CC],
                                                [0,   TRACK_SCROLL_CC],
                                                [0, 0],
                                                [0, 0]],
                                   'names' : names[0:4]})
                                   
            self.helper.send_page({'patch' : 1,
                                   'page' : 3,
                                   'type': 0,
                                   'ccs' : PAGE4_CCS,
                                   'channel' : [ CHANNEL_MIDI_COMMAND for i in range(0, 4)],
                                   'macros' : [ [128, SCENE_SCROLL_CC],
                                                [0,   TRACK_SCROLL_CC],
                                                [0, 0],
                                                [0, 0]],
                                   'names' : names[4:8]})
            self.log("olddevice %s device: %s" % (self.olddevice, self.device))
            if (self.olddevice != self.device):
                self.olddevice = self.device
                self.helper.send_flash("DEVICE", device_name[0:16].upper())
		
    def show_bank_select(self, bank_name):
	if self.show_bank:
	    self.show_bank = False
	    if self.device:
		self.parent.show_message(str(self.device.name + " Bank: " + bank_name))
	
    def receive_midi_note(self, channel, status, note_no, note_vel):
        pass
	    
    def receive_midi_cc(self, chan, cc_no, cc_value):
	pass

    def lock_to_device(self, device):
        self.log("lock to device")
	if device:
	    self.device_locked = True
	    if (not (device == self.device)):
		self.bank = 0
		self.show_bank = True
            self.device = device
	    self.parent.request_rebuild_midi_map()

    def unlock_from_device(self, device):
	if (device and (device == self.device)):
	    self.device_locked = False
        
        if (hasattr(self.parent.song(), "appointed_device")):
            if (not (self.parent.song().appointed_device == self_device)):
                self.parent.request_rebuild_midi_map()

    def set_appointed_device(self, device):
        self.log("set appointed device %s to %s" % (self.device, device))
	if self.device_locked:
	    self.device_locked = False
	if not (device == self.device):
	    self.bank = 0
	    self.show_bank = True
	    self.log("new appointed device")
            self.device = device
	self.parent.request_rebuild_midi_map()

    def refresh_state(self):
	pass

    def update_display(self):
	pass

    def disconnect(self):
	self.parent.song().view.remove_selected_track_listener(self.on_track_selected_callback)

    def on_device_selected(self):
        self.log("selected device %s" % self.selected_track.view.selected_device)
        self.set_appointed_device(self.selected_track.view.selected_device)
        
    def on_track_selected(self):
        if (self.parent.is_live_5()):
            self.log("add a listener to selected device on track %s" % self.parent.song().view.selected_track)
            self.selected_track = self.parent.song().view.selected_track

        self.parent.request_rebuild_midi_map()
