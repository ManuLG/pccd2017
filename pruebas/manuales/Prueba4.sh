#!/bin/bash

tmux new-session -s default -n editor -d
tmux split-window -v -t default
tmux split-window -h -t default
tmux send-keys -t default:0.0 C-z "./main 1 3 2 1" Enter
tmux send-keys -t default:0.1 C-z "./main 2 3 3 1"
tmux send-keys -t default:0.2 C-z "./main 3 3 1 1"
tmux select-pane -t default:0.1
tmux select-window -t default:0
tmux attach-session -t default
