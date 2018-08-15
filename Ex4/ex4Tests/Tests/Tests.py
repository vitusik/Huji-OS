import subprocess as sp
import os
import filecmp


class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


def fail(test_number):
    print(bcolors.FAIL + "Test", test_number, "failed :(" + bcolors.ENDC)


def success(test_number):
    print(bcolors.OKGREEN + "Test", test_number, "passed!" + bcolors.ENDC)


def start_test(test_number):
    print(bcolors.WARNING + "Starting Test " + str(test_number) + "... " + bcolors.ENDC, end="")


def run_test(test_number, diff=True):
    start_test(test_number)
    try:
        sp.check_call("./Test" + str(test_number), cwd=os.getcwd(), stdout=sp.DEVNULL)

        if diff:
            if not filecmp.cmp("BufferOutput" + str(test_number),
                               "ResultFiles/BufferOutputResult" + str(test_number)):
                fail(test_number)
                return
            if not filecmp.cmp("CacheLog" + str(test_number),
                               "ResultFiles/CacheLogResult" + str(test_number)):
                fail(test_number)
                return
            if not filecmp.cmp("StatLog" + str(test_number),
                               "ResultFiles/StatLogResult" + str(test_number)):
                fail(test_number)
                return

        success(test_number)
        return
    except sp.CalledProcessError as e:
        fail(test_number)
        return


if __name__ == '__main__':
    run_test(1)
    run_test(2, False)
    run_test(3)
    run_test(4)
    run_test(5)
