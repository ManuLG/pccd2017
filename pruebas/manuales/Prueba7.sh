#!/bin/sh

tmux new-session -s default -n editor -d
tmux split-window -h -t default
tmux split-window -v -t default
tmux split-window -v -t default
tmux select-pane -t default:0.0
tmux split-window -v -t default
tmux split-window -v -t default
tmux send-keys -t default:0.0 C-z "./main 1 2 4 2" Enter
tmux send-keys -t default:0.3 C-z "./main 2 2 5 3" Enter
tmux send-keys -t default:0.1 C-z "./main 3 3 3 1"
tmux send-keys -t default:0.2 C-z "./main 1 3 4 2"
tmux send-keys -t default:0.4 C-z "./main 2 3 4 3"
tmux send-keys -t default:0.5 C-z "./main 1 3 5 2"
tmux select-pane -t default:0.0
tmux select-window -t default:0
tmux attach-session -t default
