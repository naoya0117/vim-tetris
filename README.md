# vim-tetris
## Description
授業で作った

vimのキー操作で動くテトリスです
- vimのノーマルモードと挿入モードを実装しています。
- ゲーム中:hでヘルプを起動できます。
- dockerで動くように改良しました

<img src="https://github.com/naoya0117/vim-tetris/blob/image/tetris.png?raw=true" width="50%">

## Getting started

```
git clone https://github.com/naoya0117/vim-tetris.git
cd vim-tetris
```
```
bin/run.sh yourname
```
or 
```
bin/run.sh
yourname
```

If you have a direct execution environment, you can start it with ```cd src && make . /main yourname``` instead of using docker.


## Uninstall
```
$ cd vim-tetris-plus
$ bin/clear.sh
3
$ cd ..
rm -rf ./vim-tetris-plus
```



## Requirement
- docker
- docker-compose
- (or docker-desktop)

if you don't want to use docker, following environment is required.
- gcc
- make
- pthread(c-library)
- ncurses(c-library)
- ja_JP.UTF-8 support
