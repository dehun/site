import subprocess

if __name__ == '__main__':
    subprocess.check_call('rsync -rva --progress ./static/* ../dehun.bitbucket.org', shell=True)
