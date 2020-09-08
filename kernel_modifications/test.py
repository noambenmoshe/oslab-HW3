#!/usr/bin/python

import os
import time
import pyPolicy


def test_sleep():
    """Verify sleep."""
    pid = os.getpid()
    print('Parent PID: %d' % os.getpid())

    cpid = os.fork()
    if (cpid == 0):
        print('Child PID: %d' % os.getpid())
        #
        # In child, spin for 2-3 seconds before exiting
        #
        for i in range(3):
            for k in xrange(12000000):
                pass

        print('before exit')
        os._exit(0)
        print('after exit')

    print('before set_policy')
    pyPolicy.set_policy(cpid, 1, 10)
    print('after set_policy')

    print('start_time')
    start_time = int(time.time())
    print('start_time: %d' % (start_time))

    # Wait for child to exit
    print('before wait')
    os.wait()
    print('after wait')

    end_time = int(time.time())
    print('end_time-start_time: %d' % (end_time - start_time))
    assert (end_time - start_time >= 10)
    assert (end_time - start_time < 20)


def test_terminate():
    """Verify terminate."""
    pid = os.getpid()
    print('Parent PID: %d' % os.getpid())

    cpid = os.fork()
    if (cpid == 0):
        print('Child PID: %d' % os.getpid())
        #
        # In child, spin for 2-3 seconds before exiting
        #
        while True:
            pass

        print('before exit')
        os._exit(0)

    print('before set_policy')
    pyPolicy.set_policy(cpid, 2, 5)
    print('after set_policy')

    print('start_time')
    start_time = int(time.time())
    print('start_time: %d' % (start_time))

    # Wait for child to exit
    print('before wait')
    os.wait()
    print('after wait')

    end_time = int(time.time())
    print('end_time-start_time: %d' % (end_time - start_time))
    assert (end_time - start_time > 5)
    assert (end_time - start_time < 10)


def test_rescue():
    """Verify rescue from terminate."""
    pid = os.getpid()
    print('Parent PID: %d' % os.getpid())

    cpid = os.fork()
    if (cpid == 0):
        print('Child PID: %d' % os.getpid())
        #
        # In child, spin for 2-3 seconds before exiting
        #
        while True:
            pass

        print('before exit')
        os._exit(0)

    print('before set_policy')
    pyPolicy.set_policy(cpid, 2, 3)
    print('after set_policy')

    #
    # In Parent spin for 2-3 seconds before rescueing
    #
    for i in range(1):
        for k in xrange(12000000):
            pass

    print('before set_policy - rescue')
    pyPolicy.set_policy(cpid, 0, 3)
    print('after set_policy - rescue')

    #
    # In Parent spin for 2-3 seconds before rescueing
    #
    for i in range(5):
        for k in xrange(12000000):
            pass

    print('before set_policy - terminate')
    pyPolicy.set_policy(cpid, 2, 3)
    print('after set_policy - terminate')

    print('start_time')
    start_time = int(time.time())
    print('start_time: %d' % (start_time))

    # Wait for child to exit
    print('before wait')
    os.wait()
    print('after wait')

    end_time = int(time.time())
    print('end_time-start_time: %d' % (end_time - start_time))
    assert (end_time - start_time >= 3)
    assert (end_time - start_time < 6)


def test_sleep_after_sleep():
    """Verify sleep."""
    pid = os.getpid()
    print('Parent PID: %d' % os.getpid())

    cpid = os.fork()
    if (cpid == 0):
        print('Child PID: %d' % os.getpid())
        #
        # In child, spin for 2-3 seconds before exiting
        #
        for i in range(15):
            for k in xrange(12000000):
                pass

        print('before exit')
        os._exit(0)
        print('after exit')

    print('before set_policy - sleep')
    pyPolicy.set_policy(cpid, 1, 5)
    print('after set_policy - sleep')

    print('start_time')
    start_time = int(time.time())
    print('start_time: %d' % (start_time))
    #
    # In parent, spin for 2-3 seconds before exiting
    #
    for i in range(3):
        for k in xrange(12000000):
            pass

    print('before set_policy - sleep')
    pyPolicy.set_policy(cpid, 1, 5)
    print('after set_policy - sleep')

    # Wait for child to exit
    print('before wait')
    os.wait()
    print('after wait')

    end_time = int(time.time())
    print('end_time-start_time: %d' % (end_time - start_time))
    assert (end_time - start_time >= 10)
    assert (end_time - start_time < 20)


def test_fork_child_start_sleep():
    """Verify sleep."""
    pid = os.getpid()
    print('Parent PID: %d' % os.getpid())

    print('before set_policy')
    pyPolicy.set_policy(pid, 1, 10)
    print('after set_policy')

    cpid = os.fork()
    if (cpid == 0):
        #child needs to go to sleep when initilized
        print('Child PID: %d' % os.getpid())

        print('before exit')
        os._exit(0)
        print('after exit')


    print('start_time')
    start_time = int(time.time())
    print('start_time: %d' % (start_time))

    # Wait for child to exit
    print('before wait')
    os.wait()
    print('after wait')

    end_time = int(time.time())
    print('end_time-start_time: %d' % (end_time - start_time))
    assert (end_time - start_time >= 10)
    assert (end_time - start_time < 20)

def test_two_wait():
    """Verify two waits in a row."""



if __name__ == "__main__":
    #test_sleep()  #original+-
    test_sleep_after_sleep() #not working
    #test_fork_child_start_sleep()
    #test_terminate()
    #test_rescue()