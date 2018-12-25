python regen.py && docker build -t site . && docker run --rm --name site -p8080:80 site
