from SCons.Script import *
from os import access, environ, X_OK
import platform

#CacheDir('.scons-cache')
Decider('MD5-timestamp')
SetOption('implicit_cache', True)
SourceCode('.', None)



########################################################################
#
#  various command-line options
#

def pathIsExecutable(key, val, env):
    found = env.WhereIs(val)
    if found: val = found
    PathVariable.PathIsFile(key, val, env)
    if not access(val, X_OK):
        raise SCons.Errors.UserError('Path for option %s is not executable: %s' % (key, val))

opts = Variables(['.scons-options'], ARGUMENTS)
opts.Add(BoolVariable('DEBUG', 'Compile with extra information for debugging', False))
opts.Add(BoolVariable('OPTIMIZE', 'Compile with optimization', False))
opts.Add(BoolVariable('NATIVECAML', 'Use the native-code OCaml compiler', True))
opts.Add(BoolVariable('PROFILE', 'Turn on performance profiling', False))
opts.Add(BoolVariable('VALGRIND', "Run tests under Valgrinds's memory checker", False))
opts.Add(PathVariable('LLVM_CONFIG', 'Path to llvm-config executable', WhereIs('llvm-config'), pathIsExecutable))


if platform.system() == 'Darwin':
    Is64 = False
else:
    Is64 = platform.architecture()[0] == '64bit'


env = Environment(
    ENV = {'PATH' : os.environ['PATH']},
    CCFLAGS = ['-Isrc/lib', '-I'+os.environ['UNWIND_INCLUDE_PATH'], '-I'+os.environ['GLOG_INCLUDE_PATH']],
    options=opts,
    Is64=Is64,
    )
env["CC"] = os.getenv("CC") or env["CC"]
env["CXX"] = os.getenv("CXX") or env["CXX"]

def builder_unit_test(target, source, env):
    app = str(source[0].abspath)
    if os.spawnl(os.P_WAIT, app, app)==0:
        open(str(target[0]),'w').write("PASSED\n")
    else:
        return 1
# Create a builder for tests
bld = Builder(action = builder_unit_test)
env.Append(BUILDERS = {'Test' :  bld})

########################################################################
#
#  basic LaTeX document rendering
#

env.AppendUnique(
        COMMONLATEXFLAGS=['-file-line-error', '-interaction=batchmode'],
        LATEXFLAGS='$COMMONLATEXFLAGS',
        PDFLATEXFLAGS='$COMMONLATEXFLAGS',
        BIBTEXFLAGS='-terse',
        )


########################################################################
#
#  shared compiliation flags
#

flags = [
    '-Wall',
	 '-Wextra',
	 '-Wno-conversion',
	 '-Wno-sign-conversion',
	 '-Wno-sign-compare',
	 '-Wundef',
	 '-pedantic',
    '-Wformat=2',
    '-std=c++11',
    '-Werror',
    '${("", "-g")[DEBUG]}',
    '${("", "-O")[OPTIMIZE]}',
    ]

if env['CXX'] == 'clang++':
	flags.extend([
	#'-Weverything', #TODO: add back in
	'-Wno-c++98-compat',
	'-Wno-c++98-compat-pedantic',
	'-Wno-weak-vtables',
	'-Wno-global-constructors',
	'-Wno-exit-time-destructors',
	'-Wno-float-equal', # necessary for sync
	'-Wno-padded', #may turn off once in a while
	])

env.AppendUnique(
    CCFLAGS=flags,
    LINKFLAGS=flags,
    )


########################################################################
#
#  subsidiary scons scripts
#

SConscript(
    dirs=[
        # our stuff
	'MonitorPass',
	'FPPass',
	'src',
  'BlameAnalysis/backward',
  'BlameAnalysis/forward',
  'tests-blameanalysis',
  'tests',
        ],
    exports='env',
    )
