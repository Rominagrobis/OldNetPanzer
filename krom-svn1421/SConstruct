import os
import glob
import sys
import string
import subprocess

################################################################
# Fix compiling with long lines in windows
################################################################
class ourSpawn:
    def ourspawn(self, sh, escape, cmd, args, env):
        newargs = string.join(args[1:], ' ')
        cmdline = cmd + " " + newargs
        startupinfo = subprocess.STARTUPINFO()
        startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
        proc = subprocess.Popen(cmdline, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
            stderr=subprocess.PIPE, startupinfo=startupinfo, shell = False, env = env)
        data, err = proc.communicate()
        rv = proc.wait()
        if rv:
            print "====="
            print err
            print "====="
        return rv

def SetupSpawn( env ):
    if sys.platform == 'win32':
        buf = ourSpawn()
        buf.ourenv = env
        env['SPAWN'] = buf.ourspawn

################################################################
# Modified copy from BosWars of globSources
################################################################

def globSources(localenv, sourcePrefix, sourceDirs, pattern):
    sources = []
    sourceDirs = Split(sourceDirs)
    for d in sourceDirs:
        sources.append(glob.glob( sourcePrefix + '/' + d + '/' + pattern))
    sources = Flatten(sources)
    targetsources = []
    for s in sources:
        targetsources.append(buildpath + s)

    return targetsources

################################################################
# Make static libraries
################################################################

def MakeStaticLib(localenv, libname, libdirs, pattern):
    sources = globSources(localenv, 'src/Lib', libdirs, pattern)
    localenv.StaticLibrary( libpath + libname, sources)


def PrependPaths( base, str ):
    r = []
    for e in str.split(','):
        r.append(base + e.strip())
    return r
    
################################################################
# Add options
################################################################



vars = Variables()
vars.AddVariables(
    EnumVariable('mode', 'set compile mode', 'release', allowed_values=('debug', 'release')),
    EnumVariable('cross','do a cross compilation','', allowed_values=('','mingw','linux')),
    ('datadir','define the extra directory where the netpanzer will look for data files, usefull for linux distributions, defaults to no extra directory',''),
    ('sdlconfig','sets the sdl-config full path, cross compilation sure needs this', 'sdl-config'),
    ('universal','builds universal app in Max OS X(default false, other value is true)', 'false'),
    ('compilerprefix', 'sets the prefix for the cross linux compiler, example: i686-pc-linux-gnu-', ''),
    ('version', 'sets the version name to build, use "auto" for using the RELEASE_VERSION file or the default svn code', 'auto'),
    ('with_lua', 'use internal lua or link with provided parameter, with_lua=lua5.1 will add -llua5.1 in the link stage, default is internal', 'internal'),
    ('with_physfs', 'use internal physfs or link with provided parameter, with_physfs=physfs will add -lphysfs in the link stage, default is internal', 'internal'),
    EnumVariable('with_libgcc', 'Link libgcc static or dynamic', 'static', allowed_values=('static', 'dynamic')),
    EnumVariable('with_libstdcpp', 'Link libstdc++ static or dynamic', 'static', allowed_values=('static', 'dynamic')),
    EnumVariable('with_stackprotector', 'Enable gcc stack protector (requires glibc 2.7)', 'no', allowed_values=('no', 'yes')),
)

env = Environment(ENV = os.environ, variables = vars)

Help(vars.GenerateHelpText(env))

################################################################
# Set NetPanzer Version
################################################################

NPVERSION = ''

if env['version'] != 'auto':
    NPVERSION = env['version']

if NPVERSION == '':
    try:
        FILE = open('RELEASE_VERSION', 'r')
        NPVERSION = FILE.readline().strip(' \n\r')
        FILE.close()
    except:
        pass

if NPVERSION == '':
    try:
        SVERSION = os.popen('svnversion').read()[:-1]
        NPVERSION = SVERSION.split(':')[-1]
    except:
        pass

if NPVERSION == '':
    NPVERSION = 'testing';

thisplatform = sys.platform;
print 'Building version ' + NPVERSION + ' on ' + thisplatform

#
# 
#

env.Append( CCFLAGS = [ '-DPACKAGE_VERSION=\\"' + NPVERSION + '\\"' ] )

if env['with_stackprotector'] == 'no':
    #env.Append( CCFLAGS = [ '-fno-stack-protector' ] )
    env.Append( CFLAGS = [  '-fno-stack-protector' ] )

if env['datadir'] != '':
    env.Append( CCFLAGS = [ '-DNP_DATADIR=\\"' +  env['datadir'] + '\\"' ])

buildpath = 'build/'
if env['cross'] != '':
    buildpath += env['cross'] + '/'

buildpath += env['mode'] + '/'

libpath = buildpath + 'libs/'

if env['cross'] != '':
    binpath = buildpath
else:
    binpath = './'

exeappend = ''

if env['cross'] == 'mingw':
    print 'configuring for mingw cross compilation'
    env.Tool('crossmingw', toolpath = ['.'])
    env.Append( CCFLAGS = [ '-D_WIN32_WINNT=0x0501' ] )
    env.Append( LDFLAGS = [ '-mwindows' ] )
    env.Append( LIBS = [ 'ws2_32', 'mingw32' ] )
    env['WINICON'] = env.RES( 'support/icon/npicon.rc' )

if env['with_libstdcpp'] == 'static':
    env.Append( LINKFLAGS = [ '-static-libstdc++' ] )
    
if env['with_libgcc'] == 'static':
    env.Append( LINKFLAGS = [ '-static-libgcc' ] )

if env['mode'] == 'debug':
    env.Append(CCFLAGS = ['-g', '-O0'])
    exeappend = '-debug'
else:
    if env['cross'] == 'mingw':
        # it seems -O2 makes the mingw port to crash, use -O1
        env.Append(CCFLAGS = '-O1')
    else:
        env.Append(CCFLAGS = '-O2')
    env.Append(CCFLAGS = '-s')

env.Append(CCFLAGS = ['-Wall'])

# keep for reference for when redoing the linux cross compilation
#    crosslinuxenv= Environment(ENV = {'PATH' : os.environ['PATH']},tools = ['gcc','g++','ar','gnulink'], options=opts )
#    crosslinuxenv.Replace( CXX = env['crosslinuxcompilerprefix'] + env['CXX'] )
#    crosslinuxenv.Replace( CC = env['crosslinuxcompilerprefix'] + env['CC'] )
#    crosslinuxenv.Replace( AR = env['crosslinuxcompilerprefix'] + env['AR'] )
#    crosslinuxenv.Replace( RANLIB = env['crosslinuxcompilerprefix'] + env['RANLIB'] )
#    crosslinuxenv.Append( LINKFLAGS = [ '-static-libgcc' ] )
#    #crosslinuxenv.Prepend( _LIBFLAGS = [ '-static-libgcc' ] )
#    #crosslinuxenv.Prepend( _LIBFLAGS = [ '-lstdc++' ] )
#    #crosslinuxenv.Append( _LIBFLAGS = [ '`' + crosslinuxenv['CXX'] + ' -print-file-name=libstdc++.a`' ] )
#    #crosslinuxenv.Prepend( _LIBFLAGS = [ '/usr/local/gcc/i686-linux/lib/gcc/i686-linux/4.2.4/../../../../i686-linux/lib/libstdc++.a' ] )

# note, fix for gcc 4.7 if compiler is older and doesn't want to compile, comment these two
env.Append(  CFLAGS = '-mno-ms-bitfields' )
env.Append(  CXXFLAGS = '-mno-ms-bitfields' )

env.VariantDir(buildpath,'.',duplicate=0)

if env['with_lua'] == 'internal':
    luaenv = env.Clone()

if env['with_physfs'] == 'internal':
    physfsenv = env.Clone()
    
networkenv = env.Clone()    

################################################################
# Configure Environments
################################################################

env.Append( CPPPATH = [ 'src/Lib', 'src/NetPanzer' ] )

if env['with_physfs'] == 'internal':
    env.Append( CPPPATH = [ 'src/Lib/physfs' ] )

if env['with_lua'] == 'internal':
    env.Append( CPPPATH = [ 'src/Lib/lua/etc', 'src/Lib/lua/src' ] )

# for this platform
if thisplatform == 'darwin':
    env.Append( CPPPATH = ['/Library/Frameworks/SDL.framework/Headers',
                           '/Library/Frameworks/SDL_mixer.framework/Headers' ] )
    networkenv.Append( CPPPATH = ['/Library/Frameworks/SDL.framework/Headers'] )
    if env['universal'] != 'false':
		env.Append( CCFLAGS = [ '-arch', 'ppc', '-arch', 'i386' ] )
		luaenv.Append( CCFLAGS = [ '-arch', 'ppc', '-arch', 'i386' ] )
		physfsenv.Append( CCFLAGS = [ '-arch', 'ppc', '-arch', 'i386' ] )
		networkenv.Append( CCFLAGS = [ '-arch', 'ppc', '-arch', 'i386' ] )
		env.Append( LINKFLAGS = [ '-mmacosx-version-min=10.4', '-arch', 'ppc', '-arch', 'i386' ] )
    else:
        env.Append( CCFLAGS = [ '-arch', 'i386' ] )
        luaenv.Append( CCFLAGS = [ '-arch', 'i386' ] )
        physfsenv.Append( CCFLAGS = [ '-arch', 'i386' ] )
        networkenv.Append( CCFLAGS = [ '-arch', 'i386' ] )
        env.Append( LINKFLAGS = [ '-arch', 'i386' ] )
    env.AppendUnique(FRAMEWORKS=Split('SDL SDL_mixer Cocoa IOKit'))
    env.Append( NPSOURCES =  ['support/macosx/SDLMain.m'] )
elif thisplatform == 'win32':
    env.Append( CPPPATH = [ 'C:/mingw/include/SDL' ] )
    env.Append( LIBS = [ 'ws2_32', 'mingw32', 'SDLMain', 'SDL' ] )
    env.Append( CCFLAGS = [ '-D_WIN32_WINNT=0x0501' ] )
    networkenv.Append( CCFLAGS = [ '-D_WIN32_WINNT=0x0501' ] )
    networkenv.Append( CPPPATH = [ 'C:/mingw/include/SDL' ] )
    env.Append( _LIBFLAGS = [ '-mwindows' ] )
    env.Prepend( _LIBFLAGS = [ 'c:/mingw/lib/SDL_mixer.lib' ] )
    env['WINICON'] = env.RES( 'support/icon/npicon.rc' )
    SetupSpawn(env)
else:
    networkenv.ParseConfig(env['sdlconfig'] + ' --cflags --libs')
    env.ParseConfig(env['sdlconfig'] + ' --cflags --libs')
    # note: for some magic reason, now doesn't need to put the full path for
    # SDL_mixer when using mingw crosscompilation
    env.Append( LIBS = ['SDL_mixer' ] )

################################################################
# Makes libs
################################################################

# BUILDS NETWORK
networkenv.Append(           CPPPATH = [ 'src/Lib' ] )
MakeStaticLib(          networkenv, 'npnetwork', 'Network', '*.cpp')

# BUILDS LUA if requested
if env['with_lua'] == 'internal':
    luaenv.Append(           CPPPATH = [ 'src/Lib/lua/src'] )
    # _GNU_SOURCE to avoid requiring glibc 2.7 (lua uses fscanf)
    luaenv.Append(           CFLAGS = [ '-D_GNU_SOURCE=1', '-DLUA_ANSI'] )

    luasources = 'lapi.c,lcode.c,ldebug.c,ldo.c,ldump.c,lfunc.c,lgc.c,llex.c,lmem.c,\
                  lobject.c,lopcodes.c,lparser.c,lstate.c,lstring.c,ltable.c,ltm.c,\
                  lundump.c,lvm.c,lzio.c,\
                  lauxlib.c,lbaselib.c,ldblib.c,liolib.c,lmathlib.c,loslib.c,ltablib.c,\
                  lstrlib.c,loadlib.c,linit.c'
    luaenv.StaticLibrary( libpath + 'nplua', PrependPaths('src/Lib/lua/src/',luasources) )

# BUILDS PHYSFS
if env['with_physfs'] == 'internal':
    physfsenv.Append( CFLAGS = [ '-DPHYSFS_SUPPORTS_ZIP=1', '-DZ_PREFIX=1', '-DPHYSFS_NO_CDROM_SUPPORT=1' ] )
    physfsenv.Append( CPPPATH = [ 'src/Lib/physfs', 'src/Lib/physfs/zlib123' ] )
    MakeStaticLib(physfsenv, 'npphysfs', 'physfs physfs/platform physfs/archivers physfs/zlib123', '*.c')

# BUILDS 2D
env.Append( CFLAGS = [ '-DZ_PREFIX=1' ] )
MakeStaticLib(env, 'np2d', '2D', '*.c*')

# BUILDS REST OF LIBRARIES
MakeStaticLib(env, 'nplibs', 'ArrayUtil INIParser Types Util optionmm','*.cpp')

################################################################
# NetPanzer source dirs
################################################################

npdirs = """
    Actions Bot Classes Classes/AI Classes/Network Core Interfaces Network
    Objectives Particles PowerUps Resources Scripts System Units Weapons
    Views Views/Components Views/Game Views/MainMenu Views/MainMenu/Multi
    Views/MainMenu/Multi/MasterServer Views/MainMenu/Options
"""

env.Append( NPSOURCES = globSources(env, 'src/NetPanzer', npdirs, "*.cpp") )
if env.has_key('WINICON'):
    env.Append( NPSOURCES = env['WINICON'] )

wanted_libs = ['np2d']

if env['with_lua'] == 'internal':
    wanted_libs.append('nplua')
else:
    wanted_libs.append(env['with_lua'])
    
wanted_libs.append('npnetwork');
wanted_libs.append('nplibs');

if env['with_physfs'] == 'internal':
    wanted_libs.append('npphysfs');
else:
    wanted_libs.append(env['with_physfs'])

env.Prepend( LIBS = wanted_libs )
env.Prepend( LIBPATH = libpath )

netpanzer = env.Program( binpath+'netpanzer'+exeappend, env['NPSOURCES'])

pak2bmp = env.Program( binpath+'pak2bmp'+exeappend, 'support/tools/pak2bmp.cpp')
Alias('pak2bmp',pak2bmp)

bmp2pak = env.Program( binpath+'bmp2pak'+exeappend, 'support/tools/bmp2pak.cpp')
Alias('bmp2pak',bmp2pak)

Default(netpanzer)
