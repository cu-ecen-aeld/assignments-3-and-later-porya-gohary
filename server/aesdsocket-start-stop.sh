#!/bin/sh

case "$1" in
  start)
    echo "Starting aesdsocket"
    start-stop-daemon --start --oknodo --exec /usr/bin/aesdsocket -- -d
    ;;
  stop)
    echo "Stopping aesdsocket"
    start-stop-daemon --stop --oknodo --signal TERM --exec /usr/bin/aesdsocket
    ;;
  *)
    echo "Usage: $0 {start|stop}"
    exit 1
    ;;
esac