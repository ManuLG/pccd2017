#!/bin/bash

tmux new-session -s default -n editor -d
tmux split-window -h -t default
tmux send-keys -t default:0.0 C-z "./main 1 2 5 1" Enter
tmux send-keys -t default:0.1 C-z "./main 2 2 3 1"
tmux select-pane -t default:0.1
tmux select-window -t default:0
tmux attach-session -t default
