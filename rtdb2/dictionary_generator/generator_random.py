#!/usr/bin/python
import sys
import struct
import time
import os
import os.path
import random

# The following lines force the environment to be activated
#dir_path = os.path.dirname(os.path.realpath(__file__))
#activate_this = os.path.join(dir_path, 'env/bin/activate_this.py')
#if not os.path.isfile(activate_this):
#    print "Virtual environment was not found in " + activate_this + " as expected."
#    sys.exit(1)
#execfile(activate_this, dict(__file__ = activate_this))

import lmdb
import msgpack

class GeneratorRandom():
    NUM_LIMIT = 2**32
    def __init__(self, path):
        self.path = path
        self.list_env = {}

        agents = os.listdir(self.path)
        for agent in agents:
            agent_id = agent[-1]
            type = 'local' if 'local' in agent else 'shared'
            try:
                env = lmdb.open(os.path.join(self.path, agent), writemap=True, metasync=False, sync=False)
            except:
                print "RtDB Opening Error: %s does not seem a valid storage " \
                      "or there is no permissions to open!" % self.path
                sys.exit(0)
            self.list_env[(agent_id, type)] = env

    def randomize_data(self):
        list_items = []
        for (agent_id, type) in self.list_env:
            env = self.list_env[(agent_id, type)]
            env.reader_check()
            txn = env.begin(write=False)
            cursor = txn.cursor()

            items_to_reinsert = []
            for key, value in cursor:
                list_items.append((key, value))
                randomized_value = GeneratorRandom.randomize_value(value)
                items_to_reinsert.append((key, randomized_value))
            cursor.close()

            with env.begin(write=True) as txn:
                for key, value in items_to_reinsert:
                    txn.put(key, value)

    @classmethod
    def randomize_value(cls, raw_data):
        # Unpack the data except the lifetime
        unpacked = msgpack.unpackb(raw_data[:-16], raw=False)
        # Randomize it
        randomized = GeneratorRandom.randomize_value_raw(unpacked)
        # Repack again and re-concatenate the lifetime
        data_repacked = msgpack.packb(randomized)
        data_repacked = data_repacked + raw_data[-16:]
        return data_repacked

    @classmethod
    def randomize_value_raw(cls, data):
        new_data = None
        if type(data) == dict:
            new_data = {}
            for (key, value) in data.iteritems():
                new_data[key] = GeneratorRandom.randomize_value_raw(value)
        elif type(data) == list or type(data) == bytearray or \
                type(data) == tuple or type(data) == set or \
                type(data) == frozenset:
            cast_type = type(data)
            data_uncasted = []
            for item in data:
                data_uncasted.append(GeneratorRandom.randomize_value_raw(item))
            new_data = cast_type(data_uncasted)
        elif type(data) == tuple:
            tuple_list = []
            for item in data:
                tuple_list.append(GeneratorRandom.randomize_value_raw(item))
            new_data = tuple(tuple_list)
        elif type(data) == int or type(data) == long:
            new_data = random.randint(-cls.NUM_LIMIT, cls.NUM_LIMIT)
        elif type(data) == float:
            new_data = random.uniform(-cls.NUM_LIMIT, cls.NUM_LIMIT) 
        elif type(data) == bool:
            new_data = bool(random.getrandbits(1))
        elif type(data) == complex:
            new_data = random.uniform(-cls.NUM_LIMIT, cls.NUM_LIMIT) * 1j + \
                        random.uniform(-cls.NUM_LIMIT, cls.NUM_LIMIT)
        # else clause includes
        # type(data) == str or type(data) == unicode
        else:
            new_data = data
        return new_data


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print "Expected the path for the RtDB storage."
        print "Usage: %s path" % (sys.argv[0], )
        sys.exit(1)
    #print "Reading RtDB from %s" % (sys.argv[1])
    gen = GeneratorRandom(sys.argv[1])
    gen.randomize_data()
