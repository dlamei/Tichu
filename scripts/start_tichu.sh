#!/bin/bash

# Function to kill all Tichu processes
cleanup() {
    echo "Killing all Tichu processes..."
    pkill -f Tichu
}

# Trap SIGINT and SIGTERM signals to run the cleanup function
trap cleanup SIGINT SIGTERM

# Change directory to the location of the executables
cd ../build

# Start one instance of Tichu-server
./Tichu-server &

# Start four instances of Tichu-client
for i in {1..4}
do
   ./Tichu-client &
done

ps

# Wait for all background processes to finish
wait

