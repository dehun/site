import subprocess

if __name__ == '__main__':
    subprocess.check_call('rsync -rva --no-links --exclude ".git" --progress ./static/* ../dehun.gitlab.io/public', shell=True)
