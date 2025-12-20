#!/bin/bash

mkdir ~/app_revision
rsync -av --exclude='node_modules' . ~/app_revision
cd ~/app_revision
npm start