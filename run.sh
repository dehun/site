python regen.py && docker build -t site . && docker run --rm --name site -p9090:80 site
