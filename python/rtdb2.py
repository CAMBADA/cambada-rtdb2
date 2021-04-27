import lmdb
import msgpack
import math
import sys
import struct
import datetime, time
import os

RTDB2_DEFAULT_PATH = "/tmp/rtdb2_storage"
ENCODING = None
if sys.version_info[0] > 2:
    ENCODING = 'utf-8'


class RtDBTime():
    # TODO: behave as (or inherit) datetime and add serialize/deserialize?
    def __init__(self, s=0, u=0):
        self.tv_sec = int(s)
        self.tv_usec = int(u)
    def now(self):
        t = time.time()
        self.tv_sec = int(math.floor(t))
        self.tv_usec = int(round(1e6 * (t - self.tv_sec)))
    def __float__(self):
        return self.tv_sec + 1e-6*self.tv_usec
    def __add__(self, other):
        t = datetime.datetime.fromtimestamp(self.tv_sec + 1e-6*self.tv_usec)
        t = t + datetime.timedelta(seconds=float(other))
        return RtDBTime(int(time.mktime(t.timetuple())), int(t.microsecond))
    def __str__(self):
        return "%d.%06d" % (self.tv_sec, self.tv_usec)
    def pretty(self):
        return datetime.datetime.fromtimestamp(float(self)).strftime("%Y-%m-%d,%H:%M:%S.%f")

def now():
    t = RtDBTime()
    t.now()
    return [t.tv_sec, t.tv_usec]


class RtDB2Store():
    def __init__(self, path, readonly=True):
        self.path = path
        self.readonly = readonly
        self.rtdb_instances = {}
        # initialize
        self.refresh_rtdb_instances()

    def closeAll(self):
        for rtdbInstance in self.rtdb_instances.values():
            rtdbInstance.close()

    def refresh_rtdb_instances(self):

        # Check if path exists
        if not os.path.isdir(self.path):
            self.rtdb_instances.clear()
            return

        dbsOnDisk = os.listdir(self.path)

        # agent{,_sync}{0-9}
        for db in dbsOnDisk:

            # Skip if already exists
            if db in self.rtdb_instances.keys():
                continue

            # Open database connection and add to dict
            self.rtdb_instances[db] = RtDB2Instance(self, self.path, db, self.readonly)

        # Remove environments that no longer exist
        for old in self.rtdb_instances.keys():
            if old not in dbsOnDisk:
                self.rtdb_instances.pop(old)

    def get(self, agent, key, timeout=1.0):
        # Dispatch to appropriate database
        db = "agent" + str(agent)
        if db not in self.rtdb_instances.keys():
            return None

        item = self.rtdb_instances[db].get(key)

        if item and timeout and item.age() > timeout:
            return None

        return item


    def get_and_clear(self, agent, key):
        # Dispatch to appropriate database
        db = "agent" + str(agent)
        if db not in self.rtdb_instances.keys():
            return None

        item = self.rtdb_instances[db].get_and_clear(key)

        if item and timeout and item.age() > timeout:
            return None

        return item

    def put(self, agent, key, value):
        # Dispatch to appropriate database
        db = "agent" + str(agent)
        if db in self.rtdb_instances.keys():
            return self.rtdb_instances[db].put(key, value)
        return None

    def getAllRtDBItems(self):
        # Refresh all RtDB instances
        self.refresh_rtdb_instances()

        # Get all data from all RtDB instances in the form of RtDBFrameItems
        list_items = []
        for rtdbInstance in self.rtdb_instances.values():
            list_items.extend( rtdbInstance.getRtDBItems() )

        return list_items

    def wakeWaitingConsumers(self, agent, key):
        # Get sync database
        agent = "agent_sync" + str(agent)
        if agent in self.rtdb_instances.keys():

            syncDB = self.rtdb_instances[agent]

            # Get list of RtDB2SyncPoint
            data = syncDB.get_and_clear(key)

            if data is not None:

                # Free the list of semaphores
                for rtdb2SyncPoint in data:
                    import posix_ipc
                    semName = "/%s" % rtdb2SyncPoint["sem_ID"]
                    sem = posix_ipc.Semaphore(semName)
                    sem.release()

class RtDB2Instance():
    def __init__(self, store, path, name, readonly):

        self.store = store
        self.path = path
        self.readonly = readonly

        # in rtdb v2 we would have: agent_{local,shared,sync}{0-9}
        # now: local/shared has become attribute of items in agentX
        # sync database remains separated though
        self.name = name
        self.agent = int(name[-1])

        # Open database connection
        try:
            self.conn = lmdb.open(os.path.join(path, name), lock=True, readonly=readonly, writemap=True, metasync=False, sync=False)
        except:
            print("RtDB Opening Error: {} does not seem a valid storage or missing permissions! (name={})".format(self.path, name))
            sys.exit(0)

    def close(self):
        self.conn.close()

    def put(self, key, value):
        # A bit of extra effort: get current item, try to preserve its attributes
        item = self.get(key)
        if item == None:
            item = RtDBItem()
            # TODO determine shared attribute based on configuration
            if key == "JOYSTICK_CONTROL_1":
                item.shared = True
            elif key == "JOYSTICK_CONTROL_2":
                item.shared = True
            elif key == "JOYSTICK_CONTROL_3":
                item.shared = True
            elif key == "JOYSTICK_CONTROL_4":
                item.shared = True
            elif key == "JOYSTICK_CONTROL_5":
                item.shared = True
            elif key == "JOYSTICK_CONTROL_6":
                item.shared = True
            elif key == "JOYSTICK_CONTROL_7":
                item.shared = True
            elif key == "JOYSTICK_CONTROL_8":
                item.shared = True
            elif key == "JOYSTICK_CONTROL_9":
                item.shared = True
        # Overwrite value and update timestamp
        item.value = msgpack.packb(value)
        item.timestamp = now()
        # Store
        self.conn.reader_check()
        txn = self.conn.begin(write=True)
        cursor = txn.cursor()
        cursor.put(key.encode(), item.serialize() )
        cursor.close()
        txn.commit()

        # Wake up all ITEM "consumers"
        self.store.wakeWaitingConsumers(self.agent, key)

    def get(self, key, consume=False):
        self.conn.reader_check()
        txn = self.conn.begin(write=consume)
        value = txn.get( key.encode() )

        if consume:
            txn.delete( key.encode() )
            txn.commit()
        else:
            txn.abort()

        if value != None:
            if self.isSync():
                return RtDBSyncPoint.get_data(value)
            else:
                r = RtDBItem()
                r.deserialize(value) # with RtDB3 some properties (local/shared) are encoded in value
                return r
        return None

    def get_and_clear(self, key):
        return self.get(key, consume=True)

    def isSync(self):
        if 'sync' in self.name:
            return True
        else:
            return False

    def getType(self):
        if self.isSync():
            return 'sync'
        return 'normal'

    def getRtDBItems(self):

        # Skip RtDBInstances of type 'sync'
        if self.isSync():
            return []

        list_items = []

        self.conn.reader_check()
        txn = self.conn.begin()
        cursor = txn.cursor()
        for key, value in cursor:
            if ENCODING != None:
                key = key.decode(ENCODING) # decode from bytestring
            r = None
            rf = None
            try:
                r = RtDBItem()
                r.deserialize(value) # with RtDB3 all properties (agent, local/shared) are encoded in value
                rf = RtDBFrameItem(self.agent, key)
                rf.value = r.value
                rf.timestamp = r.timestamp # TODO: find better 'pythonic' way to assign subclass?
                rf.shared = r.shared
                rf.list = r.list
                rf.size = r.size
                list_items.append(rf)
            except:
                print(key)
                print(r)
                print(rf)
                raise Exception("failed to create RtDBFrameItem for dbname {}, key '{}', #value={:d}\n".format(self.name, key, len(value)))

        cursor.close()
        txn.abort()

        return list_items

    def getSyncItems(self):
        # Only allow RtDBInstances of type 'sync'
        if not self.isSync():
            return []

        list_items = []

        self.conn.reader_check()
        txn = self.conn.begin()
        cursor = txn.cursor()
        for key, value in cursor:
            try:
                r = RtDBSyncPoint(self.agent, key, value, self.getType())
                list_items.append(r)
            except:
                raise Exception("failed to create RtDBSyncPoint for agent {}, key {}, #value={:d}\n".format(self.agent, key, len(value)))

        cursor.close()
        txn.abort()

        return list_items


class RtDBItem():
    def __init__(self):
        self.value = None
        self.value_serialized = None
        self.timestamp = now()
        self.shared = False
        self.list = False
        self.size = None

    def deserialize(self, serialized):
        self.size = len(serialized)
        self.fromArray(msgpack.unpackb(serialized))

    def fromArray(self, data):
        # data is a deserialized RtDB2Item:
        #    SERIALIZE_DATA_FIXED(data, timestamp, shared, list);
        self.value_serialized = data[0]
        self.value = msgpack.unpackb(data[0], raw=False, encoding=ENCODING)
        self.timestamp = data[1] # TODO: maybe we should use datetime in this python layer?
        self.shared = data[2]
        self.list = data[3]

    def serialize(self):
        return msgpack.packb([self.value, self.timestamp, self.shared, self.list])

    def age(self):
        """
        Return age in seconds, including fractional part.
        """
        tv_sec = self.timestamp[0]
        tv_usec = self.timestamp[1]
        t = datetime.datetime.fromtimestamp(tv_sec + 1e-6*tv_usec)
        dt = datetime.datetime.now() - t
        return dt.total_seconds()

    def agef(self):
        """
        Return age as a pretty-formatted string, rounded in things like minutes/hours/etc if applicable.
        """
        # calculate traditional 'life': amount of milliseconds
        life = 1e3 * self.age()
        if life < 1000:
            # If less than a second, show as milli seconds (no need to inspect microseconds).
            s = "%3.0fms" % (life)
        elif life < 60 * 1000:
            # If less than a minute, show as seconds.
            s = "%4.0fs " % (life / 1000)
        elif life < 3600 * 1000:
            # If less than an hour, show as minutes.
            s = "%4.0fm " % (life / (1000 * 60))
        elif life < 86400 * 1000:
            # If less than a day, show as hours.
            s = "%4.0fh " % (life / (1000 * 3600))
        elif life < 365.25 * 86400 * 1000:
            # If less than a year, show as days.
            s = "%4.0fd " % (life / (1000 * 86400))
        else:
            # Show as years.
            s = "%4.0fy " % (life / (1000 * 86400 * 365.25))
        return s

    def __str__(self):
        s = ''
        s += '   age: {}\n'.format(self.agef().strip())
        s += 'shared: {}\n'.format(self.shared)
        s += '  list: {}\n'.format(self.list)
        s += ' value: {}\n'.format(self.value)
        return s


class RtDBFrameItem(RtDBItem):
    def __init__(self, agent = None, key = None):
        RtDBItem.__init__(self)
        self.key = str(key)
        self.agent = agent

    def fromArray(self, data):
        # data is a deserialized RtDB2FrameItem:
        #    SERIALIZE_DATA_FIXED(key, agent, data, timestamp, shared, list);
        self.key = data[0].decode("utf-8")
        self.agent = data[1]
        self.value = msgpack.unpackb(data[2], raw=False)
        self.timestamp = data[3]
        self.shared = data[4]
        self.list = data[5]

    def serialize(self):
        return msgpack.packb([self.key, self.agent, self.value, self.timestamp, self.shared, self.list])

    def __str__(self):
        s = ''
        s += '   key: {}\n'.format(self.key)
        s += ' agent: {}\n'.format(self.agent)
        s += '   age: {}\n'.format(self.age())
        s += 'shared: {}\n'.format(self.shared)
        s += ' value: {}\n'.format(self.value)
        s += '  list: {}\n'.format(self.list)
        return s


class RtDBSyncPoint():
    def __init__(self, agent, key, raw_data, data_type):
        self.agent = agent
        self.key = key
        self.raw_data = raw_data
        self.data = msgpack.unpackb(raw_data, raw=False)
        self.type = data_type
        self.size = len(self.raw_data)
        self.keys = self.data.keys() if type(self.data) == dict else self.data
        self.nokeys = len(self.data) if type(self.data) == dict or type(self.data) == list else 1

    # TODO create_item -- needed to implement wait_for_put in Python

    @staticmethod
    def get_data(raw_data):
        data = msgpack.unpackb(raw_data, raw=False)
        return data
