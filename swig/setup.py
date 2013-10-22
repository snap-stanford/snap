#!/usr/bin/env python

"""
setup.py file for SNAP (Stanford Network Analysis Platform) Python
    Linux version, generated on CentOS, tested on Ubuntu as well
"""

import inspect
import os
import platform
import sys

from distutils.core import setup, Extension

#
#   determine package parameters:
#       snap-py version, python version, os version, architecture
#
snappy_version = "0.8.2r"

# snap-py version
snap_version = "dev"
try:
    f = open("Version","r")
    content = f.read()
    f.close()
    w = content.split("-")
    snap_version = w[1].strip()
except:
    pass

# python version
python_version = "py" + str(sys.version_info[0]) + "." + str(sys.version_info[1])

# os version
uname = platform.uname()
os_version = "unknown-x.x"

swubuntu = False
if uname[0] == "Linux":
    try:
        f = open("/etc/centos-release","r")
        versionpos = 2
    except:
        try:
            f = open("/etc/redhat-release","r")
            versionpos = 2
        except:
            try:
                f = open("/etc/issue","r")
                versionpos = 1
            except:
                pass

    try:
        content = f.read()
        f.close()
        w = content.split(" ")
        os_version = (w[0] + w[versionpos]).lower()
        if w[0] == "Ubuntu":
	    swubuntu = True
    except:
        pass

    obj_name = "_snap.so"

elif uname[0] == "Darwin":
    os.system("sw_vers -productVersion > OSX-Release")
    try:
        f = open("OSX-Release","r")
        content = f.read()
        f.close()
        os_version = "macosx" + content.strip()
    except:
        pass

    obj_name = "_snap.so"

elif uname[0].find("CYGWIN") == 0:
    w = uname[0].rsplit("-",1)
    os_version = w[0].lower()
    obj_name = "_snap.so"

elif uname[0].find("Windows") == 0:
    os_version = "Win"
    obj_name = "_snap.pyd"

# architecture
arch = "i386"
# x86_64 on Linux, Mac OS X, i686 on Cygwin
if uname[4] == "x86_64"  or  uname[4] == "i686"  or  uname[4] == "AMD64":
    arch = "x64"

pkg_version = "-".join([snappy_version, snap_version,
                        os_version, arch, python_version])


#
#   get the installation directory
#

# get the system Python directory
sys_install = os.path.join(
            os.path.dirname(inspect.getfile(inspect)),
            "site-packages")

instdir = "site-packages"
if swubuntu:
    instdir = "dist-packages"

# check for an alternative Python user directory
user_install = sys_install
for p in sys.path:
    n = p.find(instdir)
    if n > 0:
        user_install = os.path.join(p[:n],instdir)
        break

#print "swubuntu", swubuntu
#print "pkg_version", pkg_version
#print "obj_name", obj_name
#print "user_install", user_install
#sys.exit(0)

#
#   setup configuration
#

setup (name = 'snap',
    py_modules  = ["snap"],
    #ext_modules = [snap_module],
    data_files  = [(user_install, [obj_name])],
    version     = pkg_version,
    author      = "snap.stanford.edu",
    description = """SNAP (Stanford Network Analysis Platform) Python""",
    )

