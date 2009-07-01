import sys
from nwntools import nsscompiler
nsscompiler.init(sys.argv[1])
c,err = nsscompiler.compile(open(sys.argv[2]).read(),'test',True)
nsscompiler.free()
