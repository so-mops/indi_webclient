# The web based indi client

## Undockerized
It was requested to undockerize the indi_webclient. 

The master branch is the undockerized version for the VATT. The dockerized version still exists in scotts directory. 
| What it be          | Information                                                                          |
|---------------------|--------------------------------------------------------------------------------------|
| Websocket port      | 3001 (or 3000 if code does not specify)                                              |
| Nginx website port  | 4002 (localhost:4002 to access pages)                                                |
| Service file        | nginx-docker.service (enabled)                                                       |
| Nginx docker volume | /home/mtnops/git-clones/indi_webclient/www (maps to /usr/share/nginx/html in docker) |

### NGINX docker
To avoid issues with ports with Apache on VATT Control, NGINX does run in docker and it is a systemd file. 
It is enabled and automatically starts. To restart the systemctl
```bash
sudo systemctl restart nginx-docker.service
```

### Reverting Back To Docker
Scott's directory still contains the dockerized version at VATT. To start that up use the systemd files.

```bash
sudo systemctl stop nginx-docker.service
sudo systemctl start webclient-indi.service
sudo systemctl start webclient-compose.service
```

**NOTE: You might need to remove the drivers startup in indiserver desktop launcher.**

*Ok, now back to the README.*
______

## Description 
This is an INDI client for the web. It has two parts:
1. The [INDI client](https://github.com/srswinde/indi_webclient/blob/master/src/webclient.cpp) written in C++
2. The web gui--[indi.js](https://github.com/srswinde/indi_webclient/blob/master/www/indi/indi.js) and accompanying html/css files. 

 
## The INDI client (webclient.cpp) 
The INDI client is written in c++ using the [indilib client API](http://www.indilib.org/api/classINDI_1_1BaseClient.html).
It acts as a bridge between the javascript/html based webpage and the low level socket based indiserver. It converts 
the indiserver data to JSON strings for the webpage and converts json webpage data back into the INDI format for the
indiserver. All of the websocket stuff is handled by [websocketd](https://github.com/joewalnes/websocketd) so webclient.cpp
simply uses stdin and stdout to communicate via the websocket created by websocketd. 

## The web GUI (indi.js and html) 
The web GUI uses the javascript implementation of the websocket. It is event based, meaning you need a callback function to handle incoming data. This is done in practice by overwriting the INDIws.onmessage member. 

For background information, indilib describes four types of widgets (INDI Vector Properties) Switches, lights, numbers and text. A fifth widget, Binary Large Object, is also described but it is not implemented here. indi.js maps those widgets to a jquery-ui widget. When a new INDI Vector Property is recieved by indi.js, it determines the widget type and populates the jquery widget fields as necessary. It then calls the postProc function to determine where in the web page to append the jquery widget. The postProc functon is defined in the individual webpages made for each GUI. For an example look at the [guidebox engineering GUI](https://github.com/srswinde/indi_webclient/blob/master/www/guidebox_engineering.html). postProc returns a jquery selector so indi.js can find the correct tag in which to append the widget. 


|  INDI  | jquery-ui/html5           |
|:------:|---------------------------|
| Switch | checkboxradio             |
| Light  | checkboxradio (read only) |
| Number | text input                |
| Text   | text input                |



## FLow chart for the webclient 
<pre>
____________                   ________________                      _________
|          | low level socket  |              |  JSON over websocket |       |
|indiserver|<----------------->|webclient.cpp |<-------------------->|indi.js|
|__________|  lilxml (indilib) |______________|  with websocketd     |_______|
</pre>

## Docker
Creating a webserver on each machine that might run the webclient quickly became an unbearable chore. For this reason, docker is used to build the [webserver](https://hub.docker.com/_/nginx) and run the [webclient](https://hub.docker.com/r/srswinde/indi_webclient). The INDIdrivers can also be run in a docker container for increased modularity. 
Docker along with docker-compose gives the added benefit of allowing you to run prebuilt containers without having to recompile the webclient. You should be able to simply use 
```bash
docker-compose -f docker-compose-big61.yml up 
```
to run the 61" TCS gui. 

## Running docker-compose as a systemd service. 
Generally, one instance of the docker-compose bundle will be run at each observatory. The docker-compose.yml can be thought of as a config file. To run the docker-compose file at start up you can install it as a systemd service. There is a [python script](https://github.com/srswinde/indi_webclient/blob/master/systemd/install-service.py) that does this for you. It will install the docker-compose file in /etc/compose and point to the correct directory that contains the html files. 

### Why so complicated?! 
At first glance  (and perhaps a second and third) the INDI web client may seem a bit overly complex. Indeed, it requires a C++ client, a websocket program and a lot of javascript CSS and HTML. This doesn't even mention the INDI driver that has to be built to make the whole thing work! In short, this complexity gives us modularity and a greater separation between the underlying control of the device ( which the indidrver handles) and the look/design of the UI. There are [several INDI clients](https://www.indilib.org/about/clients.html) we could have used and may still. The author of this code found them wanting. None of them gives you the flexibillity to change the placement and look of the indi vector properties. HTML5 and CSS is perfect for this flexibility. 


### Requirements 
JSON (de)serialization is done by nhloman
nhloman::json <https://github.com/nlohmann/json>





And of course the full INDI development API available with apt. 


### build ###
make
