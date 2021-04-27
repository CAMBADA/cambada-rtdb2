#!/usr/bin/python
import argparse
import rtdb2tools


# Main structure of the program
if __name__ == "__main__":

    # Argument parsing.
    descriptionTxt = 'This tool monitors a RtDB item, continuously displaying its value until CTRL-C is pressed.\n'
    exampleTxt = 'Example: rtdb2_mon.py -a 4 ACTION\n'
    parser     = argparse.ArgumentParser(description=descriptionTxt, epilog=exampleTxt,  formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-a', '--agent', help='agent ID to use', type=int, default=rtdb2tools.guessAgentId())
    parser.add_argument('-f', '--frequency', help='refresh frequency in Hz', type=float, default=10)
    parser.add_argument('-t', '--timestamp', help='prepend timestamp', action='store_true')
    #parser.add_argument('-c', '--onchange', help='show items directly when changed, minimizing latency', action='store_true')
    # TODO: zero-latency '--onchange' option requires RTDB wait_for_put, currently not implemented
    parser.add_argument('-s', '--showonce', help='filter duplicates, show stale items once', action='store_true')
    parser.add_argument('-p', '--path', help='database path to use', type=str, default=rtdb2tools.RTDB2_DEFAULT_PATH)
    parser.add_argument('key', help='RtDB key to read')
    args = parser.parse_args()

    # Instantiate the monitor
    r = rtdb2tools.RTDBMonitor(args.agent, args.frequency, args.path)
    r.subscribe(args.key)
    r.prependTimestamp = args.timestamp
    r.showOnce = args.showonce

    # Run
    r.run()

