#! /usr/bin/python
import subprocess
import time

# technical
def my_range(start, end, step):
    while start <= end:
        yield start;
        start += step;


MAKEexec = 'make bball'
CDexec = './bball'
HEADERfile = 'testscenario.h'

# --------------------- begin devil code ------------------------
#                             ,-.
#        ___,---.__          /'|`\          __,---,___
#     ,-'    \`    `-.____,-'  |  `-.____,-'    //    `-.
#  ,'        |           ~'\     /`~           |        `.
# /      ___//              `. ,'          ,  , \___      \
# |    ,-'   `-.__   _         |        ,    __,-'   `-.    |
# |   /          /\_  `   .    |    ,      _/\          \   |
# \  |           \ \`-.___ \   |   / ___,-'/ /           |  /
#  \  \           | `._   `\\  |  //'   _,' |           /  /
#   `-.\         /'  _ `---'' , . ``---' _  `\         /,-'
#      ``       /     \    ,='/ \`=.    /     \       ''
#              |__   /|\_,--.,-.--,--._/|\   __|
#              /  `./  \\`\ |  |  | /,//' \,'  \
#             /   /     ||--+--|--+-/-|     \   \
#            |   |     /'\_\_\ | /_/_/`\     |   |
#             \   \__, \_     `~'     _/ .__/   /
#              `-._,-'   `-._______,-'   `-._,-'
# ---------------------------------------------------------------


def generateHeader(runtime, n, m, missing, k, l, d, inputFile, outputFile):
    header = open(HEADERfile, "w");
    
    header.write("\n#define STARTAT " + str(n-missing));
    header.write("\n#define MISSING " + str(missing));
    header.write("\n#define ROWS " + str(n));
    header.write("\n#define COLS " + str(m));
    
    header.write("\n#define FILENAME " + '"' + inputFile + '.txt"');
    header.write("\n#define FILENAME_OUT " + '"' + outputFile + '.txt"');
    
    if (runtime > 0):
        header.write("\n#define RUNTIME_TEST");

    if (runtime > 1):
        header.write("\n#define RUNTIME_STREAMING_TEST");

    
    header.write("\n#define PARAM_K " + str(k));
    header.write("\n#define PARAM_L " + str(l));
    header.write("\n#define PARAM_D " + str(d));
    
    header.write("\n");
    
    header.flush();
    header.close();


# ------------- end devil code --------------
# .                                       ,
# )).               -===-               ,((
# ))).                                 ,(((
# ))))).            .:::.           ,((((((
# ))))))))).        :. .:        ,(((((((('
# `))))))))))).     : - :    ,((((((((((((
#  ))))))))))))))))_:' ':_((((((((((((((('
#  `)))))))))))).-' \___/ '-._(((((((((((
#   `))))_._.-' __)(     )(_  '-._._(((('
#    `))'---)___)))'\_ _/'((((__(---'(('
#      `))))))))))))|' '|(((((((((((('
#        `)))))))))/'   '\((((((((('
#          `)))))))|     |((((((('
#           `))))))|     |(((((('
#                 /'     '\
#                /'       '\
#               /'         '\
#              /'           '\
#              '---..___..---'
# -------------------------------------------

def launchCD():
    s = subprocess.check_output(["make", "bball"]);
    print(s);
    time.sleep(0.1);
    s = subprocess.check_output([CDexec]);


def experimentRun(n, m, code, missing, missing_start, tcase):
    ### tkcm params
    k = 3; l = 20; d = 3;
    if (missing_start > 51):
        l = 30;
    
    inputFile = 'in/' + code + '_m' + str(tcase);
    outputFile = 'out/tkcm' + str(tcase);
    generateHeader(0, missing_start + missing, m, missing, k, l, d, inputFile, outputFile);
    launchCD();


def runtimeExperimentRun(n, m, code, missing, missing_start, tcase):
    ### tkcm params
    k = 3; l = 20; d = 3;
    if (missing_start > 51):
        l = 30;
    
    inputFile = 'in/' + code + '_m' + str(tcase);
    outputFile = 'out/tkcm' + str(tcase);
    generateHeader(1, missing_start + missing, m, missing, k, l, d, inputFile, outputFile);
    launchCD();


def streamingExperimentRun(n, m, code, missing, missing_start, tcase):
    ### tkcm params
    k = 3; l = 20; d = 3;
    if (missing_start > 51):
        l = 30;
    
    inputFile = 'in/' + code + '_m' + str(tcase);
    outputFile = 'out/tkcm' + str(tcase);
    generateHeader(2, missing_start + missing, m, missing, k, l, d, inputFile, outputFile);
    launchCD();
