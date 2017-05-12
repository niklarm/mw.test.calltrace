import argparse
import os
import sys

parser = argparse.ArgumentParser(description='Test script')
parser.add_argument('--root', metavar='R', type=str, # nargs='+',
                    help='an integer for the accumulator')
parser.add_argument('--hrf_compare', metavar='C',  type=str,
                    help='outdate to compare to')


parser.add_argument('bin', nargs='*', help='binaries!')


args = parser.parse_args()

plugin_test    = None
plugin_test_ts = None
runner         = None
calltrace      = None

hrf_cmp_file = None
hrf_cmp_ts_file = None

for arg in args.bin:
    base = os.path.basename(arg)
    stem = os.path.splitext(base)[0]
    if stem == "plugin-test":
        plugin_test = os.path.abspath(arg)
    elif stem == "plugin-test-ts":
        plugin_test_ts = os.path.abspath(arg)
    elif stem == "mw-dbg-runner":
        runner = os.path.abspath(arg)
    elif stem == "libmw-calltrace":
        calltrace = os.path.abspath(arg)
    elif stem == "mw-calltrace":
        calltrace = os.path.abspath(arg)
    elif stem == "hrf-cmp":
        hrf_cmp_file = os.path.abspath(arg)
    elif stem == "hrf-cmp-ts":
        hrf_cmp_ts_file = os.path.abspath(arg)

import subprocess

cwd = os.getcwd();

root = os.path.normpath(str(args.root))

print ("FILE      : " + os.path.realpath(__file__))
print ("PWD       : " + os.getcwd())
print ("CT        : " + calltrace)
print ("RN        : " + runner)
print ("Plugin-Ts : " + plugin_test_ts)
print ("Plugin    : " + plugin_test)
print ("HRF-CMP   : " + hrf_cmp_file)
print ("HRF-CMP-ts: " + hrf_cmp_ts_file)


plugin_test_ts_out = subprocess.check_output([runner, "--exe", plugin_test_ts, "--lib", calltrace, "--mw-calltrace-timestamp"]).decode()
plugin_test_out    = subprocess.check_output([runner, "--exe", plugin_test,    "--lib", calltrace, "--mw-calltrace-timestamp"]).decode()

import re

hex_regex = re.compile(r"@0x[0-9A-Za-z]+")
ts_regex = re.compile(r"with timestamp \d+")


plugin_test_out    = ts_regex.sub("with timestamp --timestamps--", hex_regex.sub("--hex--", plugin_test_out)).splitlines()
plugin_test_ts_out = ts_regex.sub("with timestamp --timestamps--", hex_regex.sub("--hex--", plugin_test_ts_out)).splitlines()

errored = False

with open(hrf_cmp_file) as f:
    hrf_cmp = f.read().splitlines()
    i = 1
    for out, cmp in zip(plugin_test_out, hrf_cmp):
        if (out != cmp):
            print(hrf_cmp_file + '(' + str(i) + ') :Mismatch in comparison : "' + out + '" != "' + cmp + '"')
            i+=1
            errored = True;

with open(hrf_cmp_ts_file) as f:
    hrf_cmp_ts = f.read().splitlines()
    i = 0
    for out, cmp in zip(plugin_test_ts_out, hrf_cmp_ts):
        if (out != cmp):
            print(hrf_cmp_ts_file + '(' + str(i) + ')Mismatch in comparison : "' + out + '" != "' + cmp + '"')
            i+=1
            errored = True;

plugin_test_out    = subprocess.check_output([runner, "--exe", plugin_test,    "--lib", calltrace, "--mw-calltrace-timestamp"]).decode()


min = subprocess.check_output([runner, "--exe", plugin_test, "--lib", calltrace, 
                               "--mw-calltrace-timestamp", "--mw-calltrace-minimal", "--mw-calltrace-format=json"])

import json

min_json = json.loads(min.decode())

def compare(Lhs, Rhs):
    if Lhs != Rhs:
        errored = True;
        print ('Error: "' + str(Lhs) + '" != "' + str(Rhs) + '"');

compare(len(min_json["calls"]), 0)

err = min_json["errors"];

compare(err[0]["mode"], "error")
compare(err[0]["type"], "missing-timestamp");

compare(err[1]["mode"], "set")
compare(err[2]["mode"], "set")
compare(err[3]["mode"], "set")
compare(err[4]["mode"], "set")
compare(err[5]["mode"], "set")
compare(err[6]["mode"], "set")

def get_ptr(Value):
    return Value["calltrace"]["location"]

addr = [get_ptr(err[1]), get_ptr(err[2]), get_ptr(err[3]), get_ptr(err[4]), get_ptr(err[5]), get_ptr(err[6])]



compare(err[7] ["mode"], "error")
compare(err[8] ["mode"], "error")
compare(err[9] ["mode"], "error")
compare(err[10]["mode"], "error")

compare(err[7] ["type"], "mismatch")
compare(err[8] ["type"], "mismatch")
compare(err[9] ["type"], "overflow")
compare(err[10]["type"], "incomplete")

compare(get_ptr(err[11]), addr[0])
compare(get_ptr(err[12]), addr[1])
compare(get_ptr(err[13]), addr[2])
compare(get_ptr(err[14]), addr[3])
compare(get_ptr(err[15]), addr[4])
compare(get_ptr(err[16]), addr[5])


out = subprocess.check_output([runner, "--exe", plugin_test, "--lib", calltrace, 
                               "--mw-calltrace-timestamp", "--mw-calltrace-format=json", "--mw-calltrace-depth=3"])

jsn = json.loads(out.decode())
compare(len(jsn["calls"]), 10)

out = subprocess.check_output([runner, "--exe", plugin_test, "--lib", calltrace, 
                               "--mw-calltrace-timestamp", "--mw-calltrace-format=json", "--mw-calltrace-depth=4"])

jsn = json.loads(out.decode())
compare(len(jsn["calls"]), 14)

out = subprocess.check_output([runner, "--exe", plugin_test, "--lib", calltrace, 
                               "--mw-calltrace-timestamp", "--mw-calltrace-format=json", "--mw-calltrace-all"])

jsn = json.loads(out.decode())
if len(jsn["calls"]) <= 14:
    print ("Too few calls in complete log")
    errored = True

if errored == True:
    print ("Errored")
    sys.exit(1)
else:
    sys.exit(0)
