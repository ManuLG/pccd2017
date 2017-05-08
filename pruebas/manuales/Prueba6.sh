#!/bin/bash

tmux new-session -s default -n editor -d
tmux split-window -h -t default
tmux split-window -v -t default
tmux select-pane -t default:0.0
tmux split-window -v -t default
tmux send-keys -t default:0.0 C-z "./main 1 4 1 1" Enter
tmux send-keys -t default:0.1 C-z "./main 2 4 4 5" Enter
tmux send-keys -t default:0.2 C-z "./main 3 4 5 5" Enter
tmux send-keys -t default:0.3 C-z "./main 4 4 2 1" Enter
tmux select-pane -t default:0.3
tmux select-window -t default:0
tmux attach-session -t default
