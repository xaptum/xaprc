#!/bin/bash

set -eo pipefail

check() {
    echo \
"-------------------------------
Checking source code formatting
-------------------------------"
    local rc=0
    local sources=$(find ./src -name \*.cpp -type f -or -name \*.hpp -type f)
    for file in $sources; do
        count=$(clang-format -output-replacements-xml $file | (grep offset || true) | wc -l)
        if [ "$count" -gt "0" ]; then
            echo "✗ $file"
            rc=1
        fi
    done

    exit $rc
}

format() {
    find ./src -name \*.cpp -type f -or -name \*.hpp -type f | xargs clang-format -i
}

main() {
    case "$1" in
        check)
            check
            ;;

        format | *)
            format
            ;;

    esac

}

# Call main with all the arguments
main $@
