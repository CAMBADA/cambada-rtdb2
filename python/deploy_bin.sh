pyinstaller --onefile rtdb2_top.py --distpath ../../../bin/rtdb2_tools
rm -rf build
rm -f rtdb2_top.spec
pyinstaller --onefile rtdb2_put.py --distpath ../../../bin/rtdb2_tools
rm -rf build
rm -f rtdb2_put.spec
pyinstaller --onefile rtdb2_get.py --distpath ../../../bin/rtdb2_tools
rm -rf build
rm -f rtdb2_get.spec