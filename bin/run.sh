#!/bin/bash

while getopts h OPT; do
  case $OPT in
  h)
    echo "Usage: ./run.sh [name]"
    echo "name: your name"
    exit 0
    ;;
  \?)
    echo "Usage: ./run.sh [name]"
    echo "name: your name"
    exit 1
    ;;
  esac
done

script_dir=$(dirname "$0")
cd "${script_dir}"/.. || exit

if [ -z "${1}" ]; then
  echo "Please enter your name"
  read -rp "Name: " name
else
  name=${1}
fi

echo "start vim-tetris..."
docker compose up -d
docker compose exec tetris /bin/bash -c "tvim ${name}"
