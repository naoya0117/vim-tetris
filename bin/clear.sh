#!/bin/bash

script_dir=$(cd $(dirname $0); pwd)

docker_dir=$script_dir/..

echo "1. delete playing data"
echo "2. remove container"
echo "3. remove image"

read -p "please input your choice: " choice

cd $docker_dir
if [ $choice -eq 1 ]; then
    docker compose exec tetris bash -c "rm -f ./ranking.txt"
elif [ $choice -eq 2 ]; then
    docker compose down
elif [ $choice -eq 3 ]; then
    docker-compose down --rmi all --volumes --remove-orphans
else
    echo "invalid choice"
fi
