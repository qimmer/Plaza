import re
import gdb

def lookup_function (val):
    lookup_tag = val.type.tag
    if lookup_tag == None:
        return None

    if lookup_tag is "Entity":
        return HandlePrinter(val)
    return None


class HandlePrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        return str(gdb.parse_and_eval("GetUuid(%s)" % (str(self.val))))


# register the pretty-printer
gdb.pretty_printers.append(lookup_function)
