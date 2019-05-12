import subprocess

if __name__ == '__main__':
    subprocess.check_call('rsync -rva --exclude ".git" --progress ./static/* ../dehun.gitlab.io', shell=True)
