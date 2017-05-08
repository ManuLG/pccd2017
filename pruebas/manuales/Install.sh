#!/bin/sh

# Este script permite instalar la herramienta tmux, la cual permite tener varios paneles en una misma terminal.
# Este script detecta el sistema operativo. Sólo funciona si tienes uno de los siguientes gestores de paquetes: port (macOS), apt-get (Debian), yum (OpenSUSE/Fedora) o pacman (ArchLinux).

if [[ $(uname -s) == 'Darwin' ]]; then
        sudo port install tmux
elif [[ $(uname -s) == 'Linux' ]]; then
        YUM_CMD=$(which yum)
        APT_GET_CMD=$(which apt-get 2>&1 /dev/null)
        PACMAN_CMD=$(which pacman 2>&1 /dev/null)
        if [[ ! -z $YUM_CMD ]]; then
                 sudo yum install tmux
        elif [[ ! -z $APT_GET_CMD ]]; then
                sudo apt-get install tmux
        elif [[ ! -z $PACMAN_CMD ]]; then
                sudo pacman -S tmux
        else
                echo "No soy capaz de detectar tu gestor de paquetes, por favor, instala manualmente tmux."
        fi
else
        echo "No soy capaz de detectar tu sistema operativo, por favor, instala manualmente tmux."
fi
