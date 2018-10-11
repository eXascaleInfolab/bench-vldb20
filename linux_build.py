#!/usr/bin/python

import subprocess
import distutils.spawn

# technical
def launchProcess(executable, args, workingDir):
    procObj = subprocess.Popen(executable + " " + args, cwd=workingDir, shell=True);
    retval = procObj.wait();


def execExists(executable):
    val = distutils.spawn.find_executable(executable);
    return (val != None or val != '');


### check for necessary dependencies

if not (execExists("gcc") and execExists("g++")):
    print "error: gcc or g++ are not detected in the system";
    print "aborting build";


if not (execExists("msbuild") and execExists("mono")):
    print "error: mono and/or msbuild are not detected in the system";
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

# CD
launchProcess("make", "all", "Algorithms/CD");

# ST-MVL
launchProcess("msbuild", "STMVL.sln /p:Configuration=Release", "Algorithms/ST-MVL");

# TKCM
# not needed (built from TestingFramework)

# NNMF
# not needed (python only)

# SPIRIT
# not needed (matlab/octave only)

