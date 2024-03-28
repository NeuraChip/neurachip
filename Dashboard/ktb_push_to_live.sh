#!/bin/bash

if [ "$EUID" -ne 0 ]
	then echo "Please run as root (with sudo)"
	exit
fi
rm -rf /var/www/html/*
cp -r /home/ktb/git/neurachip_deploy/Dashboard/template/* /var/www/html/
echo "Completed"
