from subprocess import Popen, PIPE

def test_ut1():
    assert Popen("./testpr-ut1", cwd="tests", stdout = PIPE, shell = True).wait() == 1

def test_ut2():
    assert Popen("./testpr-ut2", cwd="tests", stdout = PIPE, shell = True).wait() == 2