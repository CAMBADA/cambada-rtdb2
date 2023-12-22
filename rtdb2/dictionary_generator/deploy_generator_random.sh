#!/bin/bash
pyinstaller -n rtdb2_randomize_db --onefile generator_random.py --distpath ../../../../bin/rtdb2_tools
rm -rf build
rm -f rtdb2_randomize_db.spec
