#!/usr/bin/python
import os
import sys

# Main structure of the program
if __name__ == "__main__":
    from rtdb2_curses import RtDBCurses
    from rtdb2 import RtDB2Store

    if len(sys.argv) != 2 and len(sys.argv) != 1:
        print("Expected the storage path.")
        print("Usage: %s storage_path" % sys.argv[0])
        sys.exit(0)

    if len(sys.argv) != 2:
        DEFAULT_PATH = "/tmp/rtdb2_storage"
        agents = os.listdir(DEFAULT_PATH)
        if len(agents) <= 0:
            print("No agents where found in %s" % (DEFAULT_PATH, ))
            sys.exit(0)

        if len(agents) == 1:
            storage_path = os.path.join(DEFAULT_PATH, agents[0])
        else:
            while True:
                print("Found agents: %s" % (agents, ))
                sys.stdout.write('Write agent number: ')

                agent = input()
                agent = "agent" + agent
                if agent in agents:
                    storage_path = os.path.join(DEFAULT_PATH, agent)
                    break
    else:
        storage_path = sys.argv[1]

    rtdb2Store = RtDB2Store(storage_path)
    window = RtDBCurses()

    try:
        while True:
            info = rtdb2Store.getAllRtDBItems()
            window.display_info(info)
    except KeyboardInterrupt:
        pass
    finally:
        window.exit_screen()
        rtdb2Store.closeAll()
