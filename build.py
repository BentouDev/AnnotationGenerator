#!/bin/python3
from cpt.packager import ConanMultiPackager
from conans import tools
import os, sys

DEBUG_MODE = False

def createBuilder(channel, commit, password, version):
    branch_pattern = 'release*' # channel is set explicitly!
    username = "bentoudev"

    if password:
        return ConanMultiPackager(username=username,
                channel=channel,
                stable_branch_pattern=branch_pattern,
                upload="https://api.bintray.com/conan/bentoudev/yage",
                password=password)
    else:
        return ConanMultiPackager(username=username,
                channel=channel,
                stable_branch_pattern=branch_pattern)

def build(channel, commit, password, version):

    os.environ['AGNES_COMMIT'] = commit
    os.environ['AGNES_VERSION'] = version

    builder = createBuilder(channel, commit, password, version)

    # settings = 
    # if 'TRAVIS' in os.environ:
    #     settings['compiler'] = 'clang'

    builder.add(settings={"arch": "x86_64", "build_type": "Release"},
                options={},
                env_vars={},
                build_requires={})

    builder.run()

def runCommand(args):
    import subprocess
    if len(args) == 0:
        return None
    try:
        if DEBUG_MODE:
            print(' [*] Running cmd... ' + args[0])
        raw_out = subprocess.check_output(args, shell=False)

        stdout = raw_out.decode()

        #cmd = run(args, encoding='utf-8', stdout=PIPE)
        #cmd.check_returncode()

        out = stdout.split('\n')
        if DEBUG_MODE:
            print(" [*] stdout: " + out[0])
            for x in range(len(out)):
                print(str(x) + ': ' + out[x])

        return str(out[0])
    except Exception as error:
        print (' [*] Caught error: ' + str(error))
        return None

def getGitVersion():
    try:
        data = {}
        data['version'] = runCommand(['git', 'describe'])
        data['commit'] = runCommand(['git', 'rev-parse', 'HEAD'])
        return data
    except Exception as error:
        print (' [*] Caught error: ' + str(error))
        return None

def execute(password):
    channel = 'dev'
    version = None
    commit = None
    build_number = None

    if 'CI' in os.environ:
        print(' [info] CI Environment detected')

        if 'APPVEYOR' in os.environ:
            print(" [info] Welcome, AppVeyor!")
            if 'APPVEYOR_REPO_TAG_NAME' in os.environ:
                version = os.environ['APPVEYOR_REPO_TAG_NAME']
            if 'APPVEYOR_REPO_COMMIT' in os.environ:
                commit = os.environ['APPVEYOR_REPO_COMMIT']

        if 'TRAVIS' in os.environ:
            print(" [info] Welcome, Travis!")
            os.environ['CXX'] = 'clang++-5.0'
            os.environ['CC'] = 'clang-5.0'
            if 'TRAVIS_TAG' in os.environ:
                version = os.environ['TRAVIS_TAG']
            if 'TRAVIS_COMMIT' in os.environ:
                commit = os.environ['TRAVIS_COMMIT']

    if not version or not commit:
        print (' [*] Attempt to get version from git...')
        gitData = getGitVersion()
        if gitData:
            version = gitData['version']
            commit = gitData['commit']
        ver_parts = version.split('-')
        if len(ver_parts) > 1:
            if build_number:
                # Fix semvar
                version = ver_parts[0] + '+' + build_number
            else:
                version = ver_parts[0] + '-dev'
    else:
        channel = 'stable'

    if version and commit:
        print (' [info] Channel: ' + channel)
        print (' [info] Version: ' + version)
        print (' [info] Commit: ' + commit)
        print ('')
        print (' [*] Executing conan build...')

        build(channel, commit, password, version)
    else:
        msg = "Unable to determine version"
        sys.stderr.write(str.format(' [error] {}!', msg))
        raise Exception(msg)

if __name__ == '__main__':
    print ('')
    if len(sys.argv) >= 2:
        execute(sys.argv[1])
    else:
        print(" [warn] Missing repository key argument! Package won't be uploaded")
        execute(None)
