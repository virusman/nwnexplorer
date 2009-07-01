from distutils.core import setup, Extension
import sys

link_args = []
if sys.platform == 'darwin':
    link_args.append('-Wl,-flat_namespace,-U,_environ')
module1 = Extension('nwntools.nsscompiler',
                    libraries = ['nsc','nwn'],
                    library_dirs = ['../../_NscLib','../../_NwnLib'],
                    extra_compile_args=['-Wno-sign-compare'],
                    sources = ['compiler.cpp'],
                    depends = ['../../_NscLib/libnsc.a','../../_NwnLib/libnwn.a'],
                    extra_link_args=link_args)

setup (name = 'nwntools',
       package_dir = {'nwntools':''},
       packages = ['nwntools'],
       version = '1.1',
       description = 'nss compiler',
       author = 'By Torlack, python interface by sumpfork',
       author_email = 'sumpfork@users.sf.net',
       url = 'http://www.python.org/doc/current/ext/building.html',
       long_description = '''
nss compiler in python
''',
       ext_modules=[module1])
