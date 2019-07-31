#!/usr/bin/python

import subprocess
import distutils.spawn

# technical
def launchProcess(executable, args, workingDir):
    procObj = subprocess.Popen(executable + " " + args, cwd=workingDir, shell=True);
    retval = procObj.wait();


def execExists(executable):
    val = distutils.spawn.find_executable(executable);
    return (val != None);


### check for necessary dependencies

if not (execExists("gcc") and execExists("g++")):
    print "error: gcc or g++ are not detected in the system";
    print "aborting build";
    exit();


if not (execExists("msbuild") and execExists("mono")):
    print "error: mono and/or msbuild are not detected in the system";
    print "if you're sure mono is installed, restart the terminal window"
    print "aborting build";
    exit();


### build TestingFramework

launchProcess("msbuild", "TestingFramework.sln", "TestingFramework");

### build all algorithms

# TRMF
# not needed (unsupported)

# instead we disable it in the testing framework
filename = "TestingFramework/config.cfg";

instream = open(filename, "r");
lines = instream.readlines();

nrows = len(lines);

for i in xrange(nrows):
    lines[i] = lines[i].replace("#DisableTrmf", "DisableTrmf");


with open(filename, 'w') as f:
    f.writelines(lines);


# All others
launchProcess("make", "mac", "Algorithms/AlgoCollection");
