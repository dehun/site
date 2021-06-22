#!/usr/bin/python

import re
import os
import shutil
import glob
import logging
import datetime
import subprocess
from shutil import copyfile

import jinja2

import sys
# reload(sys)
# sys.setdefaultencoding('utf-8')

logging.basicConfig(level=logging.INFO)

TEMPLATES_DIR = "./templates/"

templates = jinja2.Environment(
    loader=jinja2.FileSystemLoader(TEMPLATES_DIR))


def parse_article_filename(article_file):
    (name, day, month, year, title) = \
        re.search(".+/((\d\d)_([a-z]{3})_(\d\d\d\d)-(.+)).md",
                  article_file).groups()
    title = title.replace('_', ' ')

    date = datetime.datetime.strptime("%s%s%s" % (year, month, day), "%Y%b%d")

    return {'name': name,
            'day': day,
            'month': month,
            'year': year,
            'title': title,
            'date': date}


def html_from_markdown_file(markdown_file):
    article_html = subprocess.check_output("markdown \"%s\"" % (markdown_file),
                                           shell=True).decode('utf-8')
    return article_html


def generate_page_for_article(article_file):
    logging.info("generating page for article file: %s", article_file)
    article_html = html_from_markdown_file(article_file)

    rendered_article = templates.get_template('article.html').render(
        title=parse_article_filename(article_file)['title'],
        article_body=article_html)

    article_out_filename = './static/articles/' + \
                           parse_article_filename(article_file)['name'] + \
                           '.html'
    with open(article_out_filename, 'w') as article_out:
        article_out.write(rendered_article)


def generate_index_page_for_articles(articles):
    # sort articles by date
    articles.sort(key=lambda a: parse_article_filename(a)["date"],
                  reverse=True)
    # render them to template
    logging.info("generating index file")

    for a in articles:
        print(parse_article_filename(a)["date"])
        print(a)

    rendered_index = templates.get_template('articles_index.html').render(
        articles=[parse_article_filename(a) for a in articles])

    with open('./static/articles.html', 'w') as index_out:
        index_out.write(rendered_index)


def parse_presentation_filename(filename):
    return os.path.splitext(os.path.basename(filename))[0]


def generate_index_page_for_presentations(presentations):
    rendered_index = templates.get_template('presentations_index.html').render(
        presentations=[parse_presentation_filename(p)
                      for p in presentations])
    with open('./static/presentations.html', 'w') as index_out:
        index_out.write(rendered_index)


def regen_pages_from_presentations():
    # generating index
    presentations = glob.glob("./res/presentations/*.html")
    generate_index_page_for_presentations(presentations=presentations)


def regen_pages_from_articles():
    # prepare dirs
    os.mkdir("./static/articles/")
    # generating articles
    articles = glob.glob("./pages/articles/*.md")
    for article_file in articles:
        generate_page_for_article(article_file)

    # generate index
    generate_index_page_for_articles(articles)


def generate_info_pages():
    # generate about page
    about_text = html_from_markdown_file('./pages/about.md')
    rendered_about = templates.get_template('page.html').render(
        title='About',
        text=about_text)
    with open('./static/about.html', 'w') as about_out:
        about_out.write(rendered_about)

    # generate projects page
    about_text = html_from_markdown_file('./pages/projects.md')
    rendered_about = templates.get_template('page.html').render(
        title='Projects',
        text=about_text)
    with open('./static/projects.html', 'w') as projects_out:
        projects_out.write(rendered_about)

    # generate hire page
    about_text = html_from_markdown_file('./pages/hire.md')
    rendered_about = templates.get_template('page.html').render(
        title='Hire',
        text=about_text)
    with open('./static/hire.html', 'w') as hire_out:
        hire_out.write(rendered_about)


def prepare_resources():
    logging.info("preparing resources")
    subprocess.check_call("rsync -vra ./res/* ./static", shell=True)

def prepare_cv():
    logging.info("preparing cv")
    copyfile("../cv/cv.html", "./static/cv.html")
    copyfile("../cv/cv.css", "./static/cv.css")
    copyfile("../cv/cvprint.css", "./static/cvprint.css")    


def main():
    # prepare static folder
    try:
        shutil.rmtree("./static/")
    except:
        pass
    os.mkdir("./static/")

    # main page
    generate_info_pages()
    # articles
    regen_pages_from_articles()
    # presentations
    regen_pages_from_presentations()
    # put the resources into static
    prepare_resources()
    # make a link from about to index
    os.link("static/articles.html", "static/index.html")
    # update cv
    prepare_cv()


if __name__ == '__main__':
    main()
