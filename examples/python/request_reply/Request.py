#!/usr/bin/python
import time
import PyDragonfly
from PyDragonfly import copy_from_msg, copy_to_msg, MT_EXIT, MT_KILL
import message_defs as mdefs
import sys

MID_REQUEST = 11

# Note: Request must be started second

if __name__ == "__main__":
    mod = PyDragonfly.Dragonfly_Module(MID_REQUEST, 0)
    mod.ConnectToMMM()
    mod.Subscribe(mdefs.MT_TEST_DATA)
    mod.Subscribe(MT_EXIT)
    
    print "Request running...\n"
    
    while ( 1):
        mod.SendSignal(mdefs.MT_REQUEST_TEST_DATA)
        print("Sent request for data")
        msg = PyDragonfly.CMessage()
        mod.ReadMessage(msg)
        print "Received message", msg.GetHeader().msg_type
        if msg.GetHeader().msg_type == mdefs.MT_TEST_DATA:
            msg_data = mdefs.MDF_TEST_DATA()
            copy_from_msg(msg_data, msg)
            print "Data = [a: %d, b: %d, x: %f]" % (msg_data.a, msg_data.b, msg_data.x)
        time.sleep(1)
        
    mod.DisconnectFromMMM()
        
