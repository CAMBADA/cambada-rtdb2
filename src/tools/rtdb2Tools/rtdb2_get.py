#!/usr/bin/python
import os
import sys

# Main structure of the program
if __name__ == "__main__":
    from rtdb2_curses import RtDBCurses
    from rtdb2 import RtDB2

    if len(sys.argv) < 3:
        print "Expected the storage path."
        print "Usage: " + sys.argv[0] + " <agent_id> <key>\nOptional:\n"+\
        "\t--path=storage_path\n\t--watch\n\t--raw_value"
        sys.exit(0)

    agent_id = sys.argv[1]
    key = sys.argv[2]

    found_path = False
    watch = False
    raw_value = False
    try:
        for i in range(3, len(sys.argv)):
            if "--path" in sys.argv[i]:
                storage_path = sys.argv[i][7:]
                found_path = True
            elif "--watch" in sys.argv[i]:
                watch = True
            elif "--raw_value" in sys.argv[i]:
                raw_value = True
    except:
        print "Expected the storage path."
        print "Usage: " + sys.argv[0] + " <agent_id> <key>\nOptional:\n"+\
        "\t--path=storage_path\n\t--watch\n\t--raw_value"
        sys.exit(0)

    if not found_path:
        DEFAULT_PATH = "/tmp/rtdb2_storage"
        agents = os.listdir(DEFAULT_PATH)
        if len(agents) <= 0:
            print "No agents where found in %s" % (DEFAULT_PATH, )
            sys.exit(0)

        if len(agents) == 1:
            storage_path = os.path.join(DEFAULT_PATH, agents[0])
        else:
            while True:
                print "Found agents: %s" % (agents, )
                sys.stdout.write('Write agent number: ')

                agent = raw_input()
                agent = "agent" + agent
                if agent in agents:
                    storage_path = os.path.join(DEFAULT_PATH, agent)
                    break
    
    if raw_value:
        data = RtDB2(storage_path)
        (value, life) = data.get(agent_id, key)
        if value == None:
            print "Failed to find key " + key + " for agent " + agent_id
            sys.exit(1)
        else:
            print value
            sys.exit(0)
    elif watch:
        lastValue = None
        data = RtDB2(storage_path)
        (lastValue, life) = data.get(agent_id, key)
        while True:
            (value, life) = data.get(agent_id, key)
            if lastValue != value:
                print "Value changed!\n --- OLD VALUE ---"
                print lastValue
                print "--- NEW VALUE ---"
                print value

                lastValue = value
