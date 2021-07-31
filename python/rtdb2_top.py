#!/usr/bin/python
import os
import sys
import argparse

from rtdb2_curses import RtDBCurses
from rtdb2 import RtDB2MultiStore, RTDB2_DEFAULT_PATH


def main():

    # Argument parsing.
    descriptionTxt = 'This tool continuously monitors a set of databases. It is the default diagnostics tool to be run on robot or during simulation. It presents a table format with live update and some interaction modes.\n'
    exampleTxt = ''
    parser     = argparse.ArgumentParser(description=descriptionTxt, epilog=exampleTxt)
    parser.add_argument('-p', '--path', help='database path to use', type=str, default=RTDB2_DEFAULT_PATH)
    parser.add_argument('-d', '--database', help='database name to use', type=str, default="default")
    args = parser.parse_args()

    # setup
    rtdb2Store = RtDB2MultiStore(args.path, args.database)
    window = RtDBCurses()

    # run
    try:
        while True:
            info = rtdb2Store.getAllRtDBItems()
            window.display_info(info)
    except KeyboardInterrupt:
        pass
    finally:
        window.exit_screen()
        rtdb2Store.closeAll()


if __name__ == "__main__":
    main()


