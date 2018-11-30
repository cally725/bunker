# bunker

To start automatically 

https://www.raspberrypi.org/forums/viewtopic.php?p=485866

Re: To Auto- launch a terminal and excute a command on start
Quote
Sun Jan 12, 2014 8:43 pm

lxterminal is not a good choice because it is riddled with bugs, one of which is that if given a sequence of commands to execute it will only run the first one. I.e. it can't run several commands piped to one another. You'll need to install another terminal program:
Code: Select all

sudo apt-get ibstall xfonts-base

sudo apt-get install xterm
Create the directory ~/.config/autostart if it doesn't already exist:
Code: Select all

mkdir -p ~/.config/autostart
With your favourite text editor, create the file ~/.config/autostart/lxterm-autostart.desktop and put this in it:
Code: Select all

[Desktop Entry]
Encoding=UTF-8
Name=Terminal autostart
Comment=Start a terminal and list directory
Exec=/usr/bin/lxterm -e 'ls -l | less'


Enable sound

in /etc/rc.local
sudo amixer -c 0 cset numid=3 1


