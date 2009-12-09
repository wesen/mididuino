UI7_TYPE    = 1
UI14_TYPE   = 2
UI28_TYPE   = 3
STRING_TYPE = 4
FLOAT_TYPE  = 5
OBJECT_TYPE = 6

PRINT_ATTRIBUTE_FUNC_ID = 1

class MidiConversion:
    ObjectsById = {}
    IdByObjects = {}
    ObjectIdCounter = 0

    def getObjectById(self, id):
        if id in self.ObjectsById:
            return self.ObjectsById[id]
        else:
            return None

    def getObjectId(self, object):
        if object in self.IdByObjects:
            return self.IdByObjects[object]
        else:
            while self.ObjectIdCounter in self.ObjectsById:
                self.ObjectIdCounter = (self.ObjectIdCounter + 1) & 0x3FFF
            id = self.ObjectIdCounter
            self.IdByObjects[object] = id
            self.ObjectsById[id] = object
            self.ObjectIdCounter = (self.ObjectIdCounter + 1) & 0x3FFF
            return id

    def toMidi(self, value, type):
        if type == "ui7":
            return (UI7_TYPE, (value & 0x7F))
        elif type == "ui14":
            return (UI14_TYPE, (value >> 7) & 0x7F, (value & 0x7F))
        elif type == "ui28":
            return (UI28_TYPE, (value >> 21) & 0x7F, (value >> 14) & 0x7F, (value >> 7) & 0x7F, value & 0x7F)
        elif type == "string":
            return (STRING_TYPE, len(value)) + tuple(map(lambda x: ord(x) & 0x7F, tuple(value)))
        elif type == "float":
            whole = int(value)
            comma = int((value - int(value)) * 10000)
            return (FLOAT_TYPE, (whole >> 7) & 0x7F, whole & 0x7F, (comma >> 7) & 0x7F, (comma & 0x7F))
        elif type == "object":
            id = self.getObjectId(value)
            return (OBJECT_TYPE, (id >> 7) & 0x7F, (id & 0x7F))

    def fromMidi(self, tuple):
        type = tuple[0]
        if type == UI7_TYPE:
            return tuple[1]
        elif type == UI14_TYPE:
            return (tuple[1] << 7) | (tuple[2])
        elif type == UI28_TYPE:
            return (tuple[3] << 21) | (tuple[2] << 14) | (tuple[3] << 7) | (tuple[4])
        elif type == STRING_TYPE:
            len = tuple[1]
            string = tuple[2:(2 + len)]
            string = map(chr, string)
            return "".join(string)
        elif type == FLOAT_TYPE:
            whole = (tuple[1] << 7) | tuple[2]
            comma = (tuple[3] << 7) | tuple[4]
            return whole + (comma / 10000.0)
        elif type == OBJECT_TYPE:
            id = (tuple[1] << 7) | (tuple[2])
            return self.getObjectById(id)

MidiConverter = MidiConversion()


