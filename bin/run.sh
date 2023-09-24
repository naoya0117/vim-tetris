#!/bin/bash

while getopts h OPT
do
    case $OPT in
        h) echo "Usage: ./run.sh [name]"
           echo "name: your name"
           exit 0
           ;;
        \?) echo "Usage: ./run.sh [name]"
            echo "name: your name"
            exit 1
            ;;
    esac
done



script_dir=$(dirname $0)
cd ${script_dir}/..

if [ -z ${1} ] ; then
    echo "Please enter your name"
    read -p "Name: " name
else
    name=${1}
fi

[ -z $(ls | grep Dockerfile) ] && echo "No dockerfile found" && exit 1
if [ -z $(docker image ls | grep -e vim-tetris) ] ; then
    echo "vim-tetris image is not found"
    echo "try to build vim-tetris image..."
    docker-compose build
fi

echo "start vim-tetris..."
docker-compose up -d
docker-compose exec tetris bash -c "./main ${name}"
