#!/usr/bin/python
import os
import sys
import argparse
from rtdb2 import RtDB2Store, RTDB2_DEFAULT_PATH
import rtdb2tools


# Main structure of the program
if __name__ == "__main__":

    # Argument parsing.
    descriptionTxt = 'This tool writes content of database to standard output. It acts similar to rtdb2_top, but only one time, and without the GUI.\n'
    exampleTxt = 'Example: rtdb2_dump.py -k ACTION\n'
    parser     = argparse.ArgumentParser(description=descriptionTxt, epilog=exampleTxt)
    parser.add_argument('-a', '--agent', help='agent IDs to use, default all', type=int, nargs='*', default=[])
    parser.add_argument('-k', '--key', help='keys to display, default all', type=str, nargs='*', default=[])
    parser.add_argument('-p', '--path', help='database path to use', type=str, default=RTDB2_DEFAULT_PATH)
    args = parser.parse_args()

    # Create instance of RtDB2Store and get data
    rtdb2Store = RtDB2Store(args.path)
    items = rtdb2Store.getAllRtDBItems()

    # Sort items
    items.sort(key=lambda a: a.key)
    items.sort(key=lambda a: a.agent)

    # Display table header
    print("{:5s} {:<30s} {:<6s} {:6s} {:<s}".format("agent", "key", "type", "age  ", "value"))

    # Display table
    for item in items:
        show = True
        if len(args.agent) > 0 and item.agent not in args.agent:
            show = False
        if len(args.key) > 0 and item.key not in args.key:
            show = False
        if show:
            print("{:5d} {:<30s} {:<6s} {:6s} {:<s}".format(item.agent, item.key, ["local", "shared"][item.shared], item.agef(), str(item.value)))

    rtdb2Store.closeAll()

