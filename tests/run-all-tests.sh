#!/bin/bash
set -e

PROJECT_DIR="$( cd "$(  dirname "${BASH_SOURCE[0]}" )" && cd .. && pwd )"
cd "${PROJECT_DIR}"

bash tests/run-packet-tests.sh
bash tests/run-timer-tests.sh
bash tests/run-flash-tests.sh
bash tests/run-davis-tests.sh
