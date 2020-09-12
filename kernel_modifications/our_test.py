#!/usr/bin/python

import os
import time
import pyPolicy


def test_sleep():
    """Verify sleep."""
    print('Verify sleep:')
    pid = os.getpid()
    print('\tParent PID: %d' % os.getpid())

    cpid = os.fork()
    if cpid == 0:
        print('\tChild PID: %d' % os.getpid())
        #
        # In child, spin for 2-3 seconds before exiting
        #
        for i in range(3):
            for k in xrange(12000000):
                pass

        print('\tbefore exit')
        os._exit(0)
        print('\tafter exit')

    print('\tbefore set_policy')
    pyPolicy.set_policy(cpid, 1, 10)
    print('\tafter set_policy')

    start_time = int(time.time())
    print('\tstart_time: %d' % (start_time))

    # Wait for child to exit
    print('\tbefore wait')
    os.wait()
    print('\tafter wait')

    end_time = int(time.time())
    print('\tend_time-start_time: %d' % (end_time - start_time))
    assert (end_time - start_time >= 10)
    assert (end_time - start_time < 20)
    print('passed test_sleep!')


def test_terminate():
    """Verify terminate."""
    print('Verify terminate:')
    pid = os.getpid()
    print('\tParent PID: %d' % os.getpid())

    cpid = os.fork()
    if cpid == 0:
        print('\tChild PID: %d' % os.getpid())
        #
        # In child, infinite loop
        #
        while True:
            pass

        print('\tbefore exit')
        os._exit(0)

    print('\tbefore set_policy')
    pyPolicy.set_policy(cpid, 2, 5)
    print('\tafter set_policy')

    start_time = int(time.time())
    print('\tstart_time: %d' % (start_time))

    # Wait for child to exit
    print('\tbefore wait')
    os.wait()
    print('\tafter wait')

    end_time = int(time.time())
    print('\tend_time-start_time: %d' % (end_time - start_time))
    assert (end_time - start_time >= 5)
    assert (end_time - start_time < 10)
    print('passed test_terminate!')


def test_rescue():
    """Verify rescue from terminate."""
    print('Verify rescue from terminate:')
    pid = os.getpid()
    print('\tParent PID: %d' % os.getpid())

    cpid = os.fork()
    if (cpid == 0):
        print('\tChild PID: %d' % os.getpid())
        #
        # In child, infinite loop
        #
        while True:
            pass

        print('\tbefore exit')
        os._exit(0)

    print('\tbefore set_policy')
    pyPolicy.set_policy(cpid, 2, 3)
    print('\tafter set_policy')

    #
    # In Parent spin for 2-3 seconds before rescueing
    #
    for i in range(1):
        for k in xrange(12000000):
            pass

    print('\tbefore set_policy - rescue')
    pyPolicy.set_policy(cpid, 0, 3)
    print('\tafter set_policy - rescue')

    #
    # In Parent spin for 2-3 seconds before rescueing
    #
    for i in range(5):
        for k in xrange(12000000):
            pass

    print('\tbefore set_policy - terminate')
    pyPolicy.set_policy(cpid, 2, 3)
    print('\tafter set_policy - terminate')

    start_time = int(time.time())
    print('\tstart_time: %d' % (start_time))

    # Wait for child to exit
    print('\tbefore wait')
    os.wait()
    print('\tafter wait')

    end_time = int(time.time())
    print('\tend_time-start_time: %d' % (end_time - start_time))
    assert (end_time - start_time >= 3)
    assert (end_time - start_time < 6)
    print('passed test_rescue!')


def test_sleep_after_sleep():
    """Verify sleep after sleep."""
    print('Verify sleep after sleep')
    pid = os.getpid()
    print('\tParent PID: %d' % os.getpid())

    cpid = os.fork()
    if cpid == 0:
        print('\tChild PID: %d' % os.getpid())
        #
        # In child, spin for 2-3 seconds before exiting
        #
        for i in range(2):
            for k in xrange(12000000):
                pass

        print('\tbefore exit')
        os._exit(0)
        print('\tafter exit')

    print('\tbefore set_policy - sleep')
    pyPolicy.set_policy(cpid, 1, 5)
    print('\tafter set_policy - sleep')

    start_time = int(time.time())
    print('\tstart_time: %d' % (start_time))
    # #
    # # In parent, spin for 2-3 seconds
    # #
    # for i in range(3):
    #     for k in xrange(12000000):
    #         pass

    print('\tbefore set_policy - sleep')
    pyPolicy.set_policy(cpid, 1, 5)
    print('\tafter set_policy - sleep')

    # Wait for child to exit
    print('\tbefore wait')
    os.wait()
    print('\tafter wait')

    end_time = int(time.time())
    print('\tend_time-start_time: %d' % (end_time - start_time))
    assert (end_time - start_time >= 10)
    assert (end_time - start_time < 20)
    print('passed sleep_after_sleep!')


def test_fork_child_start_sleep():
    """Verify child starts by sleeping."""
    print('Verify child starts by sleeping')
    pid = os.getpid()
    print('\tParent PID: %d' % os.getpid())

    start_time_parent = int(time.time())
    print('\tstart_time parent: %d' % start_time_parent)

    pyPolicy.set_policy(pid, 2, 5)

    end_time_parent = int(time.time())
    print('\tend_time-start_time: %d' % (end_time_parent - start_time_parent))

    start_time = int(time.time())
    print('\tstart_time: %d' % (start_time))

    cpid = os.fork()
    if (cpid == 0):
        # child needs to go to sleep for 5 seconds when initilized
        print('\tChild PID: %d' % os.getpid())

        print('\tbefore exit')
        os._exit(0)
        print('\tafter exit')

    # Wait for child to exit
    print('\tbefore wait')
    os.wait()
    print('\tafter wait')

    end_time = int(time.time())
    print('\tend_time-start_time: %d' % (end_time - start_time))
    assert (end_time - start_time >= 5)
    assert (end_time - start_time < 8)
    print('passed fork_child_start_sleep!')


def test_fork_child_start_terminate():
    """Verify child starts then terminates."""
    print('Verify child starts then terminates')
    pid = os.getpid()
    print('\tParent PID: %d' % os.getpid())

    pyPolicy.set_policy(pid, 2, 5)

    start_time = int(time.time())
    print('\tstart_time: %d' % (start_time))

    cpid = os.fork()
    if cpid == 0:
        print('\tChild PID: %d' % os.getpid())

        #
        # In child, infinite loop
        #
        while True:
            pass

    pyPolicy.set_policy(pid, 0, 5)

    # Wait for child to exit
    print('\tbefore wait')
    os.wait()
    end_time = int(time.time())
    print('\tend_time-start_time: %d' % (end_time - start_time))
    assert (end_time - start_time >= 5)
    # if we got here the child exited because of the policy after 5 seconds
    print('passed fork_child_start_terminate!')


def test_get():
    """Verify sleep."""
    pid = os.getpid()
    print('\tParent PID: %d' % os.getpid())

    cpid = os.fork()
    if cpid == 0:
        print('\tChild PID: %d' % os.getpid())
        #
        # In child, spin for 2-3 seconds before exiting
        #
        for i in range(3):
            for k in xrange(12000000):
                pass

        print('\tbefore exit')
        os._exit(0)
        print('\tafter exit')

    print('\tbefore set_policy')
    pyPolicy.set_policy(cpid, 1, 5)
    print('\tafter set_policy')

    print('\tbefore get_policy')
    policy_id, policy_value = pyPolicy.get_policy(cpid)
    print('\tafter get_policy')

    # Wait for child to exit
    print('\tbefore wait')
    os.wait()
    print('\tafter wait')

    print('\tpolicy_id=%d\t policy_value=%d' % (policy_id, policy_value))
    assert (policy_id == 1)
    assert (policy_value == 5)
    print('passed test_get!')



if __name__ == "__main__":
    # test_sleep()  #original+-
    test_sleep_after_sleep() #not working
    # test_fork_child_start_sleep()
    # test_fork_child_start_terminate()
    # test_terminate()
    # test_rescue()
    # test_get()
