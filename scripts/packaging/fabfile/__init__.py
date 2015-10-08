from __future__ import print_function
import os
from pprint import pprint
from cStringIO import StringIO
from fabric.api import task, run, sudo, runs_once, local
from fabric.api import env, cd
from fabric.contrib import files
from fabtools import require
env.user = 'vagrant'
env.home = '/home/%s' % env.user
env.build = '%s/build' % env.home

UBUNTU_BUILD_HOST = '192.168.31.10'
UBUNTU_TEST_HOST = '192.168.31.11'


def get_fab_dir():
    return os.path.dirname(os.path.realpath(__file__))

def in_fab_dir(file_path):
    return os.path.join(get_fab_dir(), file_path)

def get_build_vm_dir():
    return os.path.join(get_fab_dir(), '../build_vm/')

def in_build_vm_dir(file_path):
    return os.path.join(get_build_vm_dir(), file_path)

def get_project_dir():
    return os.path.realpath(os.path.join(get_fab_dir(), '../../../'))

def in_project_dir(file_path):
    return os.path.join(get_project_dir(), file_path)

@task
def copy_ssh_key():
    ssh_dir = '%s/.ssh' % os.getenv('HOME')
    with open('%s/id_rsa.pub' % ssh_dir) as f:
        key = '\n%s\n' % f.read()
    files.append('%s/.ssh/authorized_keys' % env.home, key)

@task
@runs_once
def build_vm():
    env.host_string = UBUNTU_BUILD_HOST

@task
@runs_once
def test_vm():
    env.host_string = UBUNTU_TEST_HOST

@task
@runs_once
def debug():
    pprint({'fabric_env': env})
    pprint({
        'dirs': {
            'fabric': get_fab_dir(),
            'project': get_project_dir(),
            'build_vm': get_build_vm_dir()
        }
    })
    pprint({
        'hosts': {
            'ubuntu_build': UBUNTU_BUILD_HOST,
            'ubuntu_test': UBUNTU_TEST_HOST
        }
    })

@task
def init():
    require.deb.uptodate_index()

@task
def install():
    require.deb.packages((
        'gcc',
        'build-essential',
        'libboost1.54-all-dev',
        'g++',
        'curl',
        'wget',
        'automake',
        'autoconf',
        'autoconf-archive',
        'libtool',
        'libiberty-dev',
        'libbz2-dev',
        'libgnutls-dev',
        'libevent-dev',
        'libdouble-conversion-dev',
        'libgoogle-glog-dev',
        'libgflags-dev',
        'liblz4-dev',
        'liblzma-dev',
        'libsnappy-dev',
        'make',
        'zlib1g-dev',
        'binutils-dev',
        'libjemalloc-dev',
        'libssl-dev',
        'flex',
        'bison',
        'libkrb5-dev',
        'libsasl2-dev',
        'libnuma-dev',
        'pkg-config',
        'clang-3.6',
        'cmake',
        'git'
    ))

@task
def pull_fb_libs():
    run('mkdir -p %s' % env.build)
    with cd(env.build):
        if not files.exists('folly'):
            run('git clone https://github.com/facebook/folly.git')
        if not files.exists('wangle'):
            run('git clone https://github.com/facebook/wangle.git')
        if not files.exists('fbthrift'):
            run('git clone https://github.com/facebook/fbthrift.git')
        if not files.exists('proxygen'):
            run('git clone https://github.com/facebook/proxygen.git')
        if not files.exists('rocksdb'):
            run('git clone https://github.com/facebook/rocksdb.git')
        if not files.exists('MITIE'):
            run('git clone https://github.com/mit-nlp/MITIE')


def _autotools_build():
        run('autoreconf -ifv')
        run('CXX=clang++-3.6 ./configure')
        run('LD_LIBRARY_PATH=/usr/local/lib CXX=clang++-3.6 make -j4')
        sudo('make install')

def _cmake_build():
    run('mkdir -p build')
    run('cd build && CXX=clang++-3.6 cmake ../')
    run('cd build && CXX=clang++-3.6 make -j4')
    sudo('cd build && make install')

@task
def build_vm_prereqs():
    require.deb.packages([
        'ruby',
        'ruby-dev',
        'libffi-dev'
    ])
    if not run('which fpm', warn_only=True).succeeded:
        sudo('gem install --no-ri --no-rdoc fpm')

@task
def build_deps():
    build_vm_prereqs()
    pull_fb_libs()
    if not files.exists('/usr/local/include/folly'):
        with cd('%s/folly/folly' % env.build):
            run('git checkout v0.57.0')
            _autotools_build()
    if not files.exists('/usr/local/include/wangle'):
        with cd('%s/wangle/wangle' % env.build):
            run('git checkout v0.13.0')
            _cmake_build()
    if not files.exists('/usr/local/include/thrift'):
        with cd('%s/fbthrift/thrift' % env.build):
            run('git checkout v0.31.0')
            _autotools_build()
            sudo('cp lib/cpp2/libsaslstubs.a /usr/local/lib')
    if not files.exists('/usr/local/include/rocksdb'):
        with cd('%s/rocksdb' % env.build):
            run('git checkout v3.13.1')
            run('make librocksdb.so')
            run('make static_lib')
            sudo('cp -r include/rocksdb /usr/local/include')
            sudo('cp librocks* /usr/local/lib')
    if not files.exists('/usr/local/include/mitie'):
        with cd('%s/MITIE' % env.build):
            run('git checkout v0.4')
            run('make mitielib -j4')
            sudo('cp mitielib/libmitie.* /usr/local/lib')
            sudo('cp -r mitielib/include/* /usr/local/include')

@task
@runs_once
def build_relevanced_for_revision(git_tag):
    build_vm()
    with cd(env.build):
        if not files.exists('relevanced'):
            run('git clone https://github.com/scivey/relevanced.git')
    with cd('%s/relevanced' % env.build):
        run('git remote update')
        run('git checkout %s' % git_tag)
        run('git pull')
        run('make thrift')
        run('CXX=clang++-3.6 make build-server-static')
        run('CXX=clang++-3.6 make deb-package-local')
        run('cp build/deb/*.deb /vagrant/')

    dest_dir = in_project_dir('build/deb')
    local('mkdir -p %s' % dest_dir)
    local('rm -f %s/*.deb' % dest_dir)
    local_source = in_build_vm_dir('*.deb')
    local('mv %s %s' % (local_source, dest_dir))

@task
def build_relevanced_for_current_branch():
    branch = local('git rev-parse --abbrev-ref HEAD', capture=True).strip()
    build_relevanced_for_revision(branch)
