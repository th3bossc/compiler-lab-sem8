#! /bin/bash

input_path=""
output_path=""
evaluate=false
keep_temp=false

usage() {
    echo "Usage: $0 --input <input_path> --output-path <output_path> [--evaluate] [--keep-temp]"
    exit 1
}

while [[ $# -gt 0 ]]; do
    case $1 in
        --input)
            input_path="$2"
            shift 2
            ;;
        --output)
            output_path="$2"
            shift 2
            ;;
        --evaluate)
            evaluate=true
            shift
            ;;
        --keep-temp)
            keep_temp=true
            shift
            ;;
        *)
            echo "Unknown option: $1"
            usage
            ;;
    esac
done


if [ "$evaluate" = true ]; then
    ./output --input $input_path --evaluate
else 
    ./output --input $input_path --output temp.xsm
fi 

if [ $? -eq 0 ]; then 
    ./label_translator --input temp.xsm --output $output_path
    echo "file saved to $output_path"
else 
    echo "compilation failed"
fi

if [ "$keep_temp" = false ]; then
    rm temp.xsm
fi