#!/usr/bin/python
import time
import PyDragonfly
from PyDragonfly import copy_to_msg
import message_defs as md
import sys

MID_PRODUCER = 10

# Note: Producer must be started first

if __name__ == "__main__":
    mod = PyDragonfly.Dragonfly_Module(MID_PRODUCER, 0)
    mod.ConnectToMMM()
    mod.SendModuleReady()
    
    print "Producer running...\n"

    a = 0
    b = 0
    x = 0.0
    run = True
    while run:
        out_msg = PyDragonfly.CMessage(md.MT_TEST_DATA)
        data = md.MDF_TEST_DATA()
        data.a = a
        data.b = b
        data.x = x
        copy_to_msg(data, out_msg)
        mod.SendMessage(out_msg)
        print "Sent message", out_msg.GetHeader().msg_type
        
        a += 1
        b -= 3
        x += 1.234
        
        time.sleep(1)
