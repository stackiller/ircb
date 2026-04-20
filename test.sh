#!/bin/bash
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./ircb irc.libera.chat 6697 test_bug_bot secret '#testchannel' &
pid=$!
sleep 5
kill -TERM $pid
wait $pid
