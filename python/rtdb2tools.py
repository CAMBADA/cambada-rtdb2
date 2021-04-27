import os
import signal
import traceback
from rtdb2 import RtDB2Store, RTDB2_DEFAULT_PATH, RtDBTime
import datetime
import pause
from collections import defaultdict


def guessAgentId():
    """
    Guess the agent ID.
    If used on a real robot, this will return associated id, so tool users do not always have to specify it explicitly.
    """
    agentId = os.getenv("AGENT")
    if agentId != None:
        return int(agentId)
    return 0


class RTDBMonitor:
    def __init__(self, agent, frequency=30.0, rtdbpath=RTDB2_DEFAULT_PATH):
        """
        Monitor RTDB keys and show their values.
        """
        # store arguments
        self.agent = agent
        self.frequency = frequency
        # other control options, can be overridden after construction
        self.prependTimestamp = False
        self.showOnce = False
        self.afterInit = None # callback
        # the keys to monitor and actions associated to them
        self.keys = set()
        self.handles = {}
        self.lastItemTimestamp = defaultdict(lambda : "")
        # setup RTDB connection
        self.rtdb2Store = RtDB2Store(rtdbpath)
        # setup signal handler for cleanup
        self.done = False
        signal.signal(signal.SIGINT, self.cleanup)

    def cleanup(self, sig, frame):
        self.done = True

    def subscribe(self, key, handle=str):
        """
        Subscribe to a key. The value may be nicely formatted through a custom function.
        """
        self.keys.add(key)
        self.handles[key] = handle

    def output(self, line, key):
        """
        Print the line.
        """
        print(line)

    def handle(self, key, item):
        """
        Handle a RTDB item.
        """
        result = None
        try:
            result = self.handles[key](item.value)
        except:
            print("WARNING: blacklisting key " + key + " after callback failure:")
            traceback.print_exc()
            self.keys.remove(key)
            return
        return result

    def iterate(self):
        """
        Perform an iteration.
        """
        for key in list(self.keys):
            item = self.rtdb2Store.get(self.agent, key)
            if item != None:
                # check for change / freshness
                if not self.showOnce or self.fresh(key, item):
                    r = self.handle(key, item)
                    if r != None:
                        # something was given back, let's display it
                        line = r
                        if self.prependTimestamp:
                            line = "%d.%06d %s" % (item.timestamp[0], item.timestamp[1], line)
                        self.output(line, key)
                    self.lastItemTimestamp[key] = item.timestamp
        # trigger optional action
        if self.afterInit != None:
            self.afterInit()

    def fresh(self, key, item):
        """
        Test if a RTDB item is fresh.
        """
        if item.age() > 0.5:
            return False
        if self.lastItemTimestamp[key] == item.timestamp:
            return False
        return True

    def run(self):
        """
        Continuously iterate
        """
        dt = datetime.timedelta(seconds=(1.0 / self.frequency))
        self.t = datetime.datetime.now()
        while not self.done:
            self.iterate()
            # sleep until
            self.t += dt
            pause.until(self.t)
        # cleanup
        self.rtdb2Store.closeAll()

