"""
dumpXML


nar@liveapi.org - 5-28-07

	This script was originally found on ActiveState in the 
	scripts directory. It was originally by Philip Kromer
	http://aspn.activestate.com/ASPN/Cookbook/Python/Recipe/137951

	I modified it to dump XML of the objects and parse some of the 
	contents out, especially in the case of stripping out <>'s from
	object references when dumping attributes where they ended up
	being tuples/lists of objects.

"""

def dumpXML(obj, Filename=None):
    import sys
    # There seem to be a couple of other types; gather templates of them
    MethodWrapperType = type(object().__hash__)

    # Gather all the attributes of the object
    if Filename:
        output_file = open(Filename, 'w')
        stdoutold = sys.stdout
        sys.stdout = output_file 
        
    objclass  = None
    objdoc    = None
    objmodule = 'undefined'
    
    methods   = []
    builtins  = []
    classes   = []
    attrs     = []
    listeners = []
    
    for slot in dir(obj):
        try:
            attr = getattr(obj, slot)
        except:
            attr = 'no_attr'
            
        if   slot == '__class__':
            try:
                objclass = attr.__name__
            except:
                objclass = 'no_name'
        elif slot == '__doc__':
            objdoc = attr
        elif slot == '__module__':
            try:
                objmodule = attr
            except: 
                objmodule = 'no_module'
                
        elif ((type(attr).__name__ == "builtin_function_or_method") or
              (type(attr).__name__ == "method-wrapper")):
            if (slot.find("listener") != -1):
                listeners.append( slot )
            else:
                builtins.append( slot )
        elif ((type(attr).__name__ == "instancemethod") or
              (type(attr).__name__ == "function")):
            if (slot.find("listener") != -1):
                listeners.append( slot )
            else:
                methods.append( (slot, attr) )
        elif (type(attr).__name__ == "type"):
            classes.append( (slot, attr) )
        else:
            attrs.append( (slot, attr) )

    # Organize them
    methods.sort()
    builtins.sort()
    classes.sort()
    attrs.sort()

    # Print a readable summary of those attributes

    # Summary of introspection attributes
    if objclass == '':
        objclass = type(obj).__name__
    
    print '<dumpObj>' 
    print "<title class='%s' module='%s'>Instance of class '%s' as defined in module '%s'</title>" % \
           (objclass, objmodule, objclass, objmodule)
    
    # Object's Docstring
    if objdoc is None:
        objdoc = str(objdoc)
    else:
        objdoc = objdoc.strip()

    print '<documentation>', objdoc, '</documentation>'
    
    # Built-in methods
    if builtins:
        
        bi_str   = delchars(str(builtins), "[']") or str(None)
        print '<builtin>'
        for bi in bi_str.split(','):
            print '<name>', bi, '</name>'
           
        print '</builtin>'
        
    # Classes
    if classes:
 
        print '<classes>'

        for (classname, classtype) in classes:
            print '<class>'
            classdoc = getattr(classtype, '__doc__', None) or 'no documentation'
            classdoc = classdoc.replace("<", "&lt;")
            classdoc = classdoc.replace(">", "&gt;")
            print '<name>', classname, '</name>'
            print '<doc>', classdoc, '</doc>'
            print '</class>'

        print '</classes>'
        
    # User methods
    if methods:

        print '<methods>'

        for (methodname, method) in methods:
            print '<method>'
            methoddoc = getattr(method, '__doc__', None) or 'no documentation'
            methoddoc = methoddoc.replace("<", "&lt;")
            methoddoc = methoddoc.replace(">", "&gt;")
            print '<name>', methodname, '</name>'
            print '<doc>', methoddoc, '</doc>'
            print '</method>'
        print '</methods>'
    
    # Attributes
    
    if attrs:
        
        print '<attributes>'
        
        for (attr, val) in attrs:
            if attr[0:2] == "__" and attr[-2:] == "__":
                continue
            try:
                print "<attribute>"
                print '<name>', str(attr), '</name>'
                print '<type>', delchars(str(type(val)), "<'>"), '</type>'
                print '<value>', delchars(str(val), "<'>"), '</value>'
                print "</attribute>"  
            except:
		pass
                
        print '</attributes>'

    # Listener
    
    if listeners:
        
        print '<listeners>'
        
        for (attr) in listeners:
            if attr[0:2] == "__" and attr[-2:] == "__":
                continue
            if attr.find("remove_") == 0:
                continue
            if attr.find("add_") == 0:
                continue
            start =  attr.find("_has_listener")
            if start != -1:
                name = attr.replace("_has_listener", "")
                try:
                    print "<listener>"
                    print '<name>', name, '</name>'
                    print "</listener>"  
                except:
                    pass
                
        print '</listeners>'
        
        
    print "</dumpObj>"
    output_file.close()
    stdout = stdoutold
        
def delchars(str, chars):
    """Returns a string for which all occurrences of characters in
    chars have been removed."""

    # Translate demands a mapping string of 256 characters;
    # whip up a string that will leave all characters unmolested.
    identity = ''.join([chr(x) for x in range(256)])

    return str.translate(identity, chars)

def printDict(di, format="%-25s %s"):
    for (key, val) in di.items():
        print format % (str(key)+':', val)
