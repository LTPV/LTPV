#! /usr/bin/env python
# encoding: utf-8

from waflib import Logs
from waflib import Context
import os
import sys
import subprocess
import locale


VERSION='0.0.1'
APPNAME='xvin'

top = '.'
out = 'build'

######################### WAF FIX
def bld_command(*k):
	fun = k[0]
	name = fun.__name__
	from waflib.Build import BuildContext
	class tmp(BuildContext):
		cmd = name
		fun = name
	return fun


cflags = ['-std=c99']
cxxflags = ['-std=c++11']
warningflags = ['-W', '-Wall', '-Wextra','-Wno-write-strings', '-Wno-mismatched-tags', '-Wunused']

modeflags = {
    'release': ['-O2'],
    'debug-opti': ['-O2', '-g', '-ggdb'],
    'debug': ['-O0',  '-g', '-ggdb'],
    }

from waflib import Build    
class doc(Build.BuildContext):
    fun = 'build'
    cmd = 'doc' 

def options(opt):
    opt.load('compiler_c compiler_cxx doxygen boost')
    opt.add_option(
            '-m', '--mode',
            dest                    = 'mode',
            default                 = 'debug',
            action                  = 'store',
            help                    = 'build mode: *debug*, debug-opti, release'
        )

    opt.add_option(
            '--no-warning',
            dest                    = 'nowarning',
            default                 = False,
            action                  = 'store_true',
            help                    = 'disable Warning'
        )

def configure(conf):
    # auto choose gcc not working
    #from waflib.Tools.compiler_c import c_compiler
    #c_compiler['win32'] = ['gcc']
    #from waflib.Tools.compiler_cxx import cxx_compiler
    #cxx_compiler['win32'] = ['g++']
 
    conf.load('compiler_c compiler_cxx boost')
	# LOADING OPTIONS
    conf.env.MODE = conf.options.mode

    # ADDING CONSTANTS

    if "release" in conf.env.MODE:
        conf.define('NDEBUG', 1)


    conf.write_config_header('config.h')
    
    
    # ADDING FLAGS
    conf.env.append_unique('CFLAGS', modeflags[conf.env.MODE])
    conf.env.append_unique('CXXFLAGS', modeflags[conf.env.MODE])
    if conf.env.WARNING:
        conf.env.append_unique('CFLAGS', warningflags)
        conf.env.append_unique('CXXFLAGS', warningflags)
    else:
        conf.env.append_unique('CFLAGS', '-w')
        conf.env.append_unique('CXXFLAGS', '-w')


    conf.env.append_unique('CFLAGS', cflags)
    conf.env.append_unique('CXXFLAGS', cxxflags)


    conf.check(
            uselib_store='dl',
            lib='dl',
            header_name='dlfcn.h')

    print ("Mode: ", conf.env.MODE)
    print ("Flags: ", conf.env.CFLAGS)

def build(bld):
    print ("Mode: ", bld.env.MODE)
    print ("Flags: ", bld.env.CFLAGS)

    #bld.read_shlib('alleg-debug')
    # Building alfont

    #Adding correct sources
    src = bld.path.ant_glob(
            ['src/*.c', 'src/**/*.c','src/*.cc', 'src/**/*.cc'],excl=[])

    # Xvin dependency
    bld.shlib(target='ltpv' ,source=src)
    os.umask(0o022)
    bld.install_files('${PREFIX}/bin/', 'bin/ltpv', chmod=0o755)
    bld.install_files('${PREFIX}/include/', 'src/ltpv.h', chmod=0o744)
    start_dir = bld.path.find_dir('share/')
    bld.install_files('${PREFIX}/share/', start_dir.ant_glob('**/*'), cwd=start_dir,
            relative_trick=True, chmod=0o644)

@bld_command
def doc(bld):
    bld(
        features='doxygen',
        doxyfile='doxygen.conf',
        doxy_tar='docs.tar.bz2')

@bld_command
def status(conf):
    print ("Mode: ", conf.env.MODE)
    print ("Warnings :", conf.env.WARNING)

    print ("C Flags: ", conf.env.CFLAGS)
    print ("C++ Flags: ", conf.env.CXXFLAGS)
