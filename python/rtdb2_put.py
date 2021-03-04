#!/usr/bin/python
import os
import sys
import argparse
from rtdb2 import RtDB2Store, RTDB2_DEFAULT_PATH
import rtdb2tools


# Main structure of the program
if __name__ == "__main__":

    # Argument parsing.
    descriptionTxt = 'This tool can write a value in the database given an RtDB key.\n'
    exampleTxt = 'Example: rtdb2_put.py -a 0 REFBOX_CONFIG "[0,3]"\n'
    parser     = argparse.ArgumentParser(description=descriptionTxt, epilog=exampleTxt,  formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-a', '--agent', help='agent ID to use', type=int, default=rtdb2tools.guessAgentId())
    parser.add_argument('-p', '--path', help='database path to use', type=str, default=RTDB2_DEFAULT_PATH)
    parser.add_argument('key', help='RtDB key to write to')
    parser.add_argument('value', help='the value to put as string, should be mappable to target struct')
    args = parser.parse_args()

    # Create instance of RtDB2Store and read databases from disk
    rtdb2Store = RtDB2Store(args.path, False) # don't start in read-only

    # This put operation should try to preserve attributes, like shared
    value = eval(args.value) # yikes! but, this prevents argument like '[1, 4]' being written into database as string, whereas we need an array of size 2
    item = rtdb2Store.put(args.agent, args.key, value)

    rtdb2Store.closeAll()

