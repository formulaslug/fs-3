#!/bin/bash

# Get list of ttyACM devices
devices=(/dev/ttyACM*)

# If no devices found, abort
if [[ ! -e "${devices[0]}" ]]; then
    echo "No /dev/ttyACM* devices found."
    exit 1
fi

# If exactly one device, use it
if [[ ${#devices[@]} -eq 1 ]]; then
    selected="${devices[0]}"
else
    echo "Multiple /dev/ttyACM* devices found:"
    for i in "${!devices[@]}"; do
        echo "[$i] ${devices[$i]}"
    done

    read -p "Select device number: " choice

    # Validate input
    if ! [[ "$choice" =~ ^[0-9]+$ ]] || (( choice < 0 || choice >= ${#devices[@]} )); then
        echo "Invalid selection."
        exit 1
    fi

    selected="${devices[$choice]}"
fi

baud=115200

echo "Connecting to $selected at $baud baud..."
exec screen "$selected" "$baud"
