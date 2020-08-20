#!/bin/bash
#==============================================================================
#        FILE: start-secondary-guider.sh
#
#       USAGE: ./start-secondary-guider.sh or from the desktop launcher
#
# DESCRIPTION: Runs the websocketd program on port 3001 with the webclient
#              build. 
#==============================================================================
#------------------------------------------------------------------------------
# Constants
#------------------------------------------------------------------------------
PORT=3001
printf "\n##### Starting VATT Secondary and Guider GUIs\n\n"
printf "##### Tips\n"
printf "##### > Open browser and go to localhost:4002 for GUIs\n"
printf "##### > Keep this terminal running\n"
printf "##### > Requires indiserver to be running\n"
printf "\n##### You may minimize this window\n\n"

printf "\n##### Output from websocketd\n"
printf "##############################################\n\n"

/snap/bin/midori --app=http://localhost:4002 &

/usr/bin/websocketd --devconsole -port $PORT /home/mtnops/git-clones/indi_webclient/webclient

