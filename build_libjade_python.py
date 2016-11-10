# libjade.py

import os, shutil
import sipconfig

# Build settings
master_input_file = "python/Drawing.sip"
build_file = "jade.sbf"
output_dir = "build"
clean = True

# Qt settings
qt_include_dir = r"C:\Development\Qt5.5.1\5.5\msvc2010\include"
qt_lib_dir = r"C:\Development\Qt5.5.1\5.5\msvc2010\lib"

# SIP settings
config = sipconfig.Configuration()
sip_bin = config.sip_bin
sip_includedir_pyqt = os.path.join(config.sip_bin, r"..\Lib\site-packages\PyQt5\sip\PyQt5")
sip_flags_pyqt = "-t WS_WIN"


# Run SIP
if (not os.path.exists(output_dir)):
	os.mkdir(output_dir)

os.system(" ".join([config.sip_bin, "-c", output_dir, "-b", os.path.join(output_dir, build_file), "-I", "./python", "-I", sip_includedir_pyqt, sip_flags_pyqt, master_input_file]))


# Create the Makefile
current_dir = os.getcwd()
os.chdir(output_dir)

makefile = sipconfig.SIPModuleMakefile(config, build_file)
makefile.extra_include_dirs = [ qt_include_dir, os.path.join(qt_include_dir, "QtCore"), os.path.join(qt_include_dir, "QtGui"), os.path.join(qt_include_dir, "QtWidgets"), "../include" ]
makefile.extra_lib_dirs = [ qt_lib_dir, "../lib" ]
makefile.extra_libs = [ "Qt5Core", "Qt5Gui", "Qt5Widgets", "jade" ]

makefile.generate()


# Build the code
os.system("nmake")

os.chdir(current_dir)


# Copy the python file to the lib directory
shutil.copy(os.path.join(output_dir, "jade.pyd"), "lib")
shutil.copy(os.path.join(output_dir, "jade.pyd"), "test")


# Clean all intermediate files
if (clean):
	shutil.rmtree(output_dir)