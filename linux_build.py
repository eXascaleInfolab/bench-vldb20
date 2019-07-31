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


matlabExec = "octave";

if (execExists("octave")):
    matlabExec = "octave";
elif (execExists("matlab")):
    matlabExec = "matlab";
else:
    print "error: octave or matlab are not detected in the system";
    print "aborting build";
    exit();


### build TestingFramework

launchProcess("msbuild", "TestingFramework.sln", "TestingFramework");

### build all algorithms

# TRMF
launchProcess(matlabExec, "--eval \"install\"", "Algorithms/trmf");

# All others
launchProcess("make", "all", "Algorithms/AlgoCollection");
