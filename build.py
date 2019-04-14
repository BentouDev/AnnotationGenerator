#!/bin/python3
from cpt.packager import ConanMultiPackager
from conans import tools
import os, sys

DEBUG_MODE = False

def build(channel, commit, password, version):
    branch_pattern = 'release*' # channel is set explicitly!
    username = "bentoudev"

    if password:
        builder = ConanMultiPackager(username=username,
                channel=channel,
                stable_branch_pattern=branch_pattern,
                upload="https://api.bintray.com/conan/bentoudev/yage",
                password=password)
    else:
        builder = ConanMultiPackager(username=username, 
                channel=channel,
                stable_branch_pattern=branch_pattern)

    builder.add(settings={"arch": "x86_64", "build_type": "Release"},
                options={},
                env_vars={'AGNES_COMMIT':commit, 'AGNES_VERSION':version},
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
        stdout = str(raw_out)

        #cmd = run(args, encoding='utf-8', stdout=PIPE)
        #cmd.check_returncode()

        out = stdout.split('\n')
        if DEBUG_MODE:
            print(" [*] stdout: " + stdout)
            for x in range(len(out)):
                print(out[x])

        return out[0]
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

    if 'CI' in os.environ:
        print(' [info] CI Environment detected')

        if 'APPVEYOR' in os.environ:
            print(" [info] Welcome, AppVeyor!")
            if 'APPVEYOR_REPO_TAG_NAME' in os.environ:
                version = os.environ['APPVEYOR_REPO_TAG_NAME']
                channel = 'stable'
            if 'APPVEYOR_REPO_COMMIT' in os.environ:
                commit = os.environ['APPVEYOR_REPO_COMMIT']

        if 'TRAVIS' in os.environ:
            print(" [info] Welcome, Travis!")
            os.environ['CXX'] = 'clang++-5.0'
            os.environ['CC'] = 'clang-5.0'
            if 'TRAVIS_TAG' in os.environ:
                version = os.environ['TRAVIS_TAG']
                channel = 'stable'
            if 'TRAVIS_COMMIT' in os.environ:
                commit = os.environ['TRAVIS_COMMIT']

    if not version or not commit:
        print (' [*] Attempt to get version from git...')
        gitData = getGitVersion()
        if gitData:
            version = gitData['version']
            commit = gitData['commit']

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
