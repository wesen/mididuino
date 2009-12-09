import Live
from Tracing import Traced
from MidiConversion import *
from LiveClasses import *

#class MidiCommandListener(Traced):
class MidiCommandListener:
    __module__ = __name__
    __doc__ = 'MidiCommand Listener'
    listener_list = {}
    
    def __init__(self, parent, obj):
        self.parent = parent
        self.obj = obj
        MidiCommandListener.realinit(self)

    def add_listener(self, listener, cb = None):
        if listener in self.listener_list:
            self.remove_listener(listener, cb)
        if cb == None:
            cb_function = (lambda (name): lambda : self.print_attr(name))(listener)
        else:
            cb_function = cb
        add_function = getattr(self.obj, "add_" + listener + "_listener")
        if add_function:
            add_function(cb_function)
            if listener in self.listener_list:
                self.listener_list[listener] += [cb_function]
            else:
                self.listener_list[listener] = [cb_function]
            self.parent.log("added listener " + listener)

    def remove_listener(self, listener, cb = None):
        remove_function = getattr(self.obj, "remove_" + listener + "_listener")
        if remove_function:
            if cb != None:
                remove_function(cb)
                self.listener_list[listener] = self.listener_list[listener].remove(cb)
                if self.listener_list[listener] == []:
                    del self.listener_list[listener]
            else:
                for cb in self.listener_list[listener]:
                    remove_function(cb)
                del self.listener_list[listener]
            self.parent.log("removed listener " + listener)
            
    def realinit(self):
        self.class = getObjectClass(self.obj)
        self.listener_list = self.class.listeners
        self.parent.log("listener list: %s" % (self.listener_list))

    def print_attr(self, name):
        self.parent.log(name + (" callback: %s" % (getattr(self.obj, name))))

    def remove_all(self):
        for listener in self.listener_list:
            self.remove_listener(listener)

    def disconnect(self):
        self.remove_all()
