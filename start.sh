#!/bin/bash

gnome-terminal -- ./bin/server 8080
gnome-terminal -- ./bin/server 8081
gnome-terminal -- ./bin/server 8082

gnome-terminal -- ./bin/client 192.168.247.128 8080 "一号客户端来电"
gnome-terminal -- ./bin/client 192.168.247.128 8081 "二号客户端来电"
gnome-terminal -- ./bin/client 192.168.247.128 8081 "三号客户端来电"
gnome-terminal -- ./bin/client 192.168.247.128 8082 "四号客户端来电"