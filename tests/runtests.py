from subprocess import Popen, PIPE

def test0():
    # main app returns 0, while each ut returns its ut-index
    assert Popen("./testpr", stdout = PIPE, shell = True).wait() == 0

def test1():
    # main app returns 0, while each ut returns its ut-index
    assert Popen("./testpr-ut1", stdout = PIPE, shell = True).wait() == 1

def test2():
    # main app returns 0, while each ut returns its ut-index
    assert Popen("./testpr-ut2", stdout = PIPE, shell = True).wait() == 2

def test3():
    # solib provides a function that returns 3 and can only be accessed 
    # if the linking works
    assert Popen("./sotest", stdout = PIPE, shell = True).communicate()[0] == b"3\n"

def test4():
    # uses pre compiled headers. different templates are used from the pch
    assert Popen("./pchtest", stdout = PIPE, shell = True).communicate()[0] == b"0\n1\n2\n0\n1\n2\n"

def test5():
    # makes no use of uts, should still work
    assert Popen("./no-uts", stdout = PIPE, shell = True).communicate()[0] == b"main app\n"
