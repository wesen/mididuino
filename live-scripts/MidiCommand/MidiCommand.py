import Live
from MidiCommandScript import MidiCommandScript
from MidiCommandMixerController import MidiCommandMixerController
from MidiCommandDeviceController import MidiCommandDeviceController
from consts import *

class MidiCommand(MidiCommandScript):
    __module__ = __name__
    __doc__ = 'Automap script for MidiCommand controllers'
    __name__ = "MidiCommand Remote Script"
    
    def __init__(self, c_instance):
        self.suffix = ""
	MidiCommand.realinit(self, c_instance)

    def realinit(self, c_instance):
	MidiCommandScript.realinit(self, c_instance)
	self.mixer_controller = MidiCommandMixerController(self)
	self.device_controller = MidiCommandDeviceController(self)
        self.components = [ self.mixer_controller, self.device_controller ]

    def suggest_map_mode(self, cc_no):
        return Live.MidiMap.MapMode.absolute
