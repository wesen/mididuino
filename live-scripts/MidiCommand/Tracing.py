"""Tracing metaclass.

XXX This is very much a work in progress.

"""

import sys

class TraceMetaClass:
    """Metaclass for tracing.

    Classes defined using this metaclass have an automatic tracing
    feature -- by setting the __trace_output__ instance (or class)
    variable to a file object, trace messages about all calls are
    written to the file.  The trace formatting can be changed by
    defining a suitable __trace_call__ method.

    """

    __inited = 0

    def __init__(self, name = "", bases = [], dict = {}):
        self.__name__ = name
        self.__bases__ = bases
        self.__dict = dict
        # XXX Can't define __dict__, alas
        self.__inited = 1

    def __getattr__(self, name):
        try:
            return self.__dict[name]
        except KeyError:
            for base in self.__bases__:
                try:
                    return base.__getattr__(name)
                except AttributeError:
                    pass
            raise AttributeError, name

    def __setattr__(self, name, value):
        if not self.__inited:
            self.__dict__[name] = value
        else:
            self.__dict[name] = value

    def __call__(self, *args, **kw):
        inst = TracingInstance()
        inst.__meta_init__(self)
        try:
            init = inst.__getattr__('__init__')
        except AttributeError:
            init = lambda: None
        apply(init, args, kw)
        return inst

    __trace_output__ = None

class TracingInstance:
    """Helper class to represent an instance of a tracing class."""

    myhash = {}
    
    def __trace_call__(self, fp, fmt, *args):
	args2 = []
	for i in range(0, len(args)):
	    args2 += [args[i].__str__()]
#	    if (isinstance(args2[i], unicode)):
#		args2[i] = args2[i].encode("latin-1")
#	for i in range(0, len(args)):
#	    args[i] = args[i].__str__()
#	    if (isinstance(args[i], unicode)):
#		args[i] = args[i].encode("latin-1")
	str = (fmt + "\n") % tuple(args2)
	fp.write(str)
	fp.flush()

    def __meta_init__(self, klass):
        self.__class = klass

    def __getattr__(self, name):
        # Invoked for any attr not in the instance's __dict__
        try:
            raw = self.__class.__getattr__(name)
        except AttributeError:
            raise AttributeError, name
#	print "raw %s" % (raw)
	try:
	    raw.__call__
	except:
	    return raw
        # It's a function
        fullname = self.__class.__name__ + "." + name
	if (self.myhash.has_key(fullname)):
	    return self.myhash[fullname]
	else:
	    newWrapper = TracingWrapper(fullname, raw, self)
	    self.myhash[fullname] = newWrapper
	    return newWrapper
#        if not self.__trace_output__ or name == '__trace_call__':
#            return NotTracingWrapper(fullname, raw, self)
#        else:
#	return TracingWrapper(fullname, raw, self)

class NotTracingWrapper:
    def __init__(self, name, func, inst):
        self.__name__ = name
        self.func = func
        self.inst = inst
    def __call__(self, *args, **kw):
        return apply(self.func, (self.inst,) + args, kw)

Traced = None
if (sys.platform == "win32"):
    Traced = TraceMetaClass('Traced', (), {'__trace_output__': \
                                               open("C:/fftrace.log", "a"), \
                                               '__filter_funcs__': []})
else:
    Traced = TraceMetaClass('Traced', (), {'__trace_output__': \
                                               open("/tmp/fftrace.log", "a"), \
                                               '__filter_funcs__': []})
    
class TracingWrapper(NotTracingWrapper):
    def __call__(self, *args, **kw):
	if (self.__name__.split(".")[-1] in self.inst.__filter_funcs__) or \
	    (not self.inst.__trace_output__):
	    rv = apply(self.func, (self.inst,) + args, kw)
	    return rv
	else:
	    self.inst.__trace_call__(self.inst.__trace_output__,
				     "calling %s, args=%s, kw=%s",
				     self.__name__, args, kw)
	    try:
		rv = apply(self.func, (self.inst,) + args, kw)
		self.inst.__trace_call__(self.inst.__trace_output__,
					 "returning from %s with value %s",
					 self.__name__, rv)
		return rv
	    except Exception, inst:
		self.inst.__trace_call__(self.inst.__trace_output__,
					 "Exception in %s: %s", self.__name__, inst)
		raise inst
