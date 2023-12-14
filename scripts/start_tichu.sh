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
sleep 1 


# Start four instances of Tichu-client
./Tichu-client --size 1200 800 --name Alice --auto 1 &
./Tichu-client --size 1200 800 --name Bob --auto 1 &
./Tichu-client --size 1200 800 --name Carl --auto 1 &
./Tichu-client --size 1200 800 --name Dan --auto 1 &

ps

# Wait for all background processes to finish
wait

