from time import sleep
from threading import Thread
from PyDragonfly import Dragonfly_Module, CMessage
import Dragonfly_config as rc

class Subscription(object):
    def __init__(self, name, num=None, has_data=True):
        self.name = name
        if num == None:
            self.num = eval('rc.MT_%s' % name)
        else:
            if not type(num) == int:
                raise ValueError("Subscription number must be of type `int`")
            self.num = num
        if has_data:
            mdf_name = 'MDF_%s' % (name)
            self.mdf = eval('rc.' + mdf_name)
        else:
            self.mdf = None

class DragonflyThread(Thread):
    '''
    Run Dragonfly message collection in a separate thread. Especially useful
    for co-existing with GUI threads.

    Notes
    -----
    In container object, create an instance of DragonflyThread, declare
    'server', 'subs', 'recv_msg', 'status', and then call start()
    '''
    # override these attributes
    mid = 0 # default to dynamic allocation
    subs = []
    server = ''
    recv_msg = lambda msg: None
    status = lambda: True

    def run(self):
        mod = Dragonfly_Module(self.mid, 0)
        mod.ConnectToMMM(self.server)
        for sub in self.subs:
            print "subscribing to %s" % (sub)
            mod.Subscribe(sub)
        mod.SendModuleReady()
        while (self.status()):
            msg = CMessage()
            rcv = mod.ReadMessage(msg, 0)
            if rcv == 1:
                if msg.GetHeader().msg_type in self.subs:
                    self.recv_msg(msg)
            sleep(.001)
