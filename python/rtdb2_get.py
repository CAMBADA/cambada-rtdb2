#!/usr/bin/python
import os
import sys
import argparse
from rtdb2 import RtDB2Store, RTDB2_DEFAULT_PATH
import rtdb2tools
from hexdump import hexdump


# Main structure of the program
if __name__ == "__main__":

    # Argument parsing.
    descriptionTxt = 'This tool reads a value from the database given an RtDB key.\n'
    exampleTxt = """Example: rtdb2_get.py -a 6 ROBOT_STATE
   age: 2h
shared: True
  list: False
 value: [2, [1581172987, 618438], [0.05368572473526001, -0.2938263416290283, 5.330356597900391], [0.1385340541601181, -0.8020891547203064, 0.7817431688308716], False, [0.0, 0.0], 6, 'A']

Example: rtdb2_get.py -a 2 DIAG_WORLDMODEL_LOCAL -x "['balls'][0]['result']"
[[5.3209381103515625, 0.5837346315383911, 0.15281200408935547], [-0.0029433025047183037, 0.01433953270316124, 1.2758345292240847e-05], 1.0, [22033, 1889585904]]
"""
    parser     = argparse.ArgumentParser(description=descriptionTxt, epilog=exampleTxt,  formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('-a', '--agent', help='agent ID to use', type=int, default=rtdb2tools.guessAgentId())
    parser.add_argument('-s', '--serialized', help='also show serialized string (as hexdump)', action='store_true')
    parser.add_argument('-p', '--path', help='database path to use', type=str, default=RTDB2_DEFAULT_PATH)
    parser.add_argument('-x', '--expression', help='evaluate expression, useful to fetch a specific element', type=str)
    parser.add_argument('key', help='RtDB key to read')
    args = parser.parse_args()

    # Create instance of RtDB2Store and read databases from disk
    rtdb2Store = RtDB2Store(args.path)

    item = rtdb2Store.get(args.agent, args.key, timeout=None)
    if args.expression:
        print(eval("item.value" + args.expression))
    else:
        print(str(item))
    if args.serialized:
        hexdump(item.value_serialized)

    rtdb2Store.closeAll()

