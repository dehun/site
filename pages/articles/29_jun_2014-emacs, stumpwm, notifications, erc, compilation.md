# Notifications from emacs(erc, compilation) in stumpwm #
## Intro ##
I have using stumpwm for 2 years now and I like it a lot.  
This is a great tiled WM which is ideal for emacs users due to the same keybindgins idealogy.  
One day I decided to use emacs irc client and to solve context switch issues by notifications.  

## Stumpwm and emacs integration ##
We can send commands to stumpwm via swank + slime.  
Swank is like rpc server for lisp. And slime is client for swank for emacs.  
In your .stumpwmrc you have to start the swank.

    (load "/usr/share/common-lisp/source/slime/swank-loader.lisp")
    (swank-loader:init)
    (swank:create-server :port 4005
                         :style :spawn
                         :dont-close t)

Now you can command your emacs to connect to it through slime

    M-x slime-connect

All lisp code you will enter in appeared REPL will be executed inside of stumpwm process.

## Notifications ##
Stumpwm have a message command to display a message in a corner of your screen.  
So all we need is to send the code like

    (stumpwm:message "something happened")

into the stumpwm process.
I have implemented a small module for mine config.  
Here it is

    (require 'slime)
    
    (defvar de-notifications-enabled 1 "notifications switcher")
    
    (defun de-notify-init ()
      (slime-connect "127.0.0.1" 4005))
    
    (defun de-enable-notifications ()
      (setq de-notifications-enabled 1))
    
    (defun de-disable-notifications ()
      (setq de-notifications-enabled 0))
    
    (defun de-notify (origin message)
      (when (eq de-notifications-enabled 1)
        (slime-eval `(stumpwm:message 
                      (common-lisp:concatenate 'common-lisp:string "[" ,origin "] :: " ,message)))))
    
    (de-notify-init)
    (de-enable-notifications)
    (de-notify "emacs" "notifications system started up")
    
    (provide 'de-notifications)


## ERC ##
For irc I have use following piece of code

    (require 'de-notifications)
    (de-notify-init)
    
    (defun bytes-to-string (bytes)
      (apply 'concat (mapcar `byte-to-string bytes)))
    
    (defun erc-global-notify (match-type nick message)
      (de-notify "erc" (concat "[" nick "]" ": " (bytes-to-string message))))
    
    (add-hook 'erc-text-matched-hook 'erc-global-notify)
    
    (de-notify "erc" "initialized erc notifications system")
    
    (provide 'de-chat)

## Compilation finished ##

    (require 'de-notifications)
    (de-notify-init)
    (defun compilation-finished-hook (buf status)
      (de-notify "emacs" (concat "compilation finished with status: " status)))
    (add-hook 'compilation-finish-functions #'compilation-finished-hook)
