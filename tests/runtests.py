from subprocess import Popen, PIPE

def test1():
    assert Popen("./testpr-ut1", stdout = PIPE, shell = True).wait() == 1

def test2():
    assert Popen("./testpr-ut2", stdout = PIPE, shell = True).wait() == 2

def test3():
    assert Popen("./sotest", stdout = PIPE, shell = True).communicate()[0] == b"3\n"

def test4():
    assert Popen("./pchtest", stdout = PIPE, shell = True).communicate()[0] == b"0\n1\n2\n0\n1\n2\n"