# Emacs offline documentation(boost, stl, python) with cppman, anything and emacs #
## Intro ##
I have always tried to optimize my coding practices.
And the usual stopper to coding is when you switch your focus.  
And when you are going to look at some api documentation - you usualy switch to a browser.  
So I was wondering can we integrate c++ stl and boost documentation into emacs?

## Solution ##
The perfect one is man pages.  
They are already integrated into emacs as manmode and womanmode.  
They have anything support for candidates choosing.  
For the STL we will be using cppman project.  
For boost we will be using native boost man pages generator.  

## STL ##
As I mentioned we will use cppman for STL library.  
It crawles cplusplus.com/reference and convertes all files into man.  

    cppman -r       # crawl database
    cppman -c       # cache all(download and convert to man)
    cppman -m true  # add cpp man pages to man path

Now you can type M-x man and enter something like std::vector.  
The better way is anything framework.  

    (require 'anything)
    (require 'anything-config)
    (global-set-key (kbd "C-c I")  ;; i -> info
     (lambda () (interactive)
      (anything
       :prompt "Info about: "
       :candidate-number-limit 100
       :sources
          '(anything-c-source-man-pages
            anything-c-source-boost-html))))

Skip now anything-c-source-boost-html. I will introduce it later.  
Now just push C-c I and enter something like "std::vector", or "std vector".  

## Boost ##
I have been trying to compile man pages from boost doc, but encounter some errors and decided to use html instead.  
To view html we have 2 options - use browse-url or w3m-browse-url.  
First one will open new tab in your default browser.  
Second one will create new buffer in emacs and will use w3m browser.  
I have choosed w3m approach.  
I am not very cool with lisp, so code will be a bit ugly.  

    ;; boost documentation
    (require 'w3m)
    
    (defvar boost-documentation-directory
      "/usr/share/doc/libboost1.55-doc/"
      "defines boost directory location")
    
    
    (defun recursive-file-list (dir)
      (let ((files-list '())
            (current-entries (directory-files dir t)))
        (dolist (entry current-entries)
          (cond ((and (file-regular-p entry)
                      (string-match "html?$" entry))
                 (setq files-list
                       (cons entry files-list)))
                ((and (file-directory-p entry)
                      (not (string-equal ".." (substring entry -2)))
                      (not (string-equal "." (substring entry -1))))
                 (setq files-list (append files-list (recursive-file-list entry))))))
          files-list))
    
    
    (defvar anything-c-source-boost-html
      '((name . "boost html documentation")
        (requires-pattern . 3)
        (candidates . (lambda ()
                        (recursive-file-list boost-documentation-directory)))
        (delayed)
        (action . (lambda (entry)
                    (w3m-browse-url entry)))))
    
## Python ##
And a bonus one for python.  

    (defvar python-documentation-directory
      "/usr/share/doc/python2.7/html/"
      "defines python docs directory location")
    
    (defvar anything-c-source-python-html
      '((name . "python html documentation")
        (requires-pattern . 3)
        (candidates . (lambda ()
                        (recursive-file-list python-documentation-directory)))
        (delayed)
        (action . (lambda (entry)
                    (w3m-browse-url entry)))))

## Dependencies ##
I am using Debian and for me next works

    sudo apt-get install w3m w3m-el
    sudo apt-get install libboost1.55-doc python-doc 

Anything I have been installed through el-get.
