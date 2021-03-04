import lmdb
import msgpack
import sys
import struct
import time
import os

class RtDB2():
    def __init__(self, path, readonly=True):
        self.path = path
        self.list_env = {}

        self.update_environments_list(readonly)

    def close(self):
        for (agent_id, type_) in self.list_env:
            env = self.list_env[(agent_id, type_)][0]
            env.close()

    def update_environments_list(self, readonly=True):
        # Check for possible new environments
        if not os.path.isdir(self.path):
            self.list_env.clear()
            return
        agents = os.listdir(self.path)
        for agent in agents:
            agent_id = agent[-1]
            type = 'local' if 'local' in agent else 'shared'
            if (agent_id, type) in self.list_env:
                self.list_env[(agent_id, type)][1] = True
                continue
            try:
                env = lmdb.open(os.path.join(self.path, agent), readonly=readonly)
            except:
                print "RtDB Opening Error: %s does not seem a valid storage or missing permissions!" % (self.path)
                sys.exit(0)
            self.list_env[(agent_id, type)] = [env, True]
        # Remove old environments
        for key in self.list_env.keys():
            if self.list_env[key][1] == False:
                self.list_env.pop(key)
            else:
                self.list_env[key][1] = False

    def put(self, agent, key, value, shared=True):
        for (agent_id, type_) in self.list_env:
            agent_num = agent_id[-1]
            if agent_num != agent:
                continue
            type_requested = 'shared' if shared else 'local'
            if type_ != type_requested:
                continue
            env = self.list_env[(agent_id, type_)][0]
            env.reader_check()
            txn = env.begin(write=True)
            cursor = txn.cursor()
            cursor.put(key, RtDBItem.create_item(value, 0))
            cursor.close()
            txn.commit()

    def get(self, agent, key):
        value_found = False
        for (agent_id, type_) in self.list_env:
            agent_num = agent_id[-1]
            if agent_num != agent:
                continue
            env = self.list_env[(agent_id, type_)][0]
            env.reader_check()
            txn = env.begin()
            value = txn.get(key)
            txn.abort()

            if value != None:
                value_found = True
                break
        if not value_found:
            return (None, None)
        return RtDBItem.get_data(value)

    def update_data(self):
        self.update_environments_list()

        list_items = []
        for (agent_id, type) in self.list_env:
            env = self.list_env[(agent_id, type)][0]
            env.reader_check()
            txn = env.begin()
            cursor = txn.cursor()

            for key, value in cursor:
                list_items.append(RtDBItem(agent_id, key, value, type))

            cursor.close()
            txn.abort()
        return list_items

class RtDBItem():
    def __init__(self, agent_id, key, raw_data, data_type):
        self.agent = agent_id
        self.key = key
        self.raw_data = raw_data

        life_s = raw_data[-16:-8]
        life_us = raw_data[-8:]
        self.data = msgpack.unpackb(raw_data[:-16])

        self.type = data_type
        self.size = len(self.raw_data)
        self.keys = self.data.keys() if type(self.data) == dict else self.data
        self.nokeys = len(self.data) if type(self.data) == dict or type(self.data) == list else 1

        machine_millis = int(round(time.time() * 1000))
        self.life = machine_millis - (struct.unpack("<q", life_s)[0] * 1e3 + struct.unpack("<q", life_us)[0] / 1e3)

    @staticmethod
    def create_item(json_data, life_ms):
        time_val = time.time()
        time_s = round(time_val)
        time_us = round(((time_val * 1000.0) % 1000.0) * 1000.0)

        data = msgpack.packb(json_data)
        life_s = time_s - life_ms * 1000.0
        life_us = time_us - (life_ms % 1000.0) * 1000.0

        buf = bytearray(16)
        buf[:8] = struct.pack('<q', life_s)
        buf[8:] = struct.pack('<q', life_us)
        data += buf
        return data

    @staticmethod
    def get_data(raw_data):
        data = msgpack.unpackb(raw_data[:-16])

        life_s = raw_data[-16:-8]
        life_us = raw_data[-8:]

        machine_millis = int(round(time.time() * 1000))
        life = machine_millis - (struct.unpack("<q", life_s)[0] * 1e3 + struct.unpack("<q", life_us)[0] / 1e3)
        

        return (data, life)



