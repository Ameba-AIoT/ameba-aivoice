#!/bin/bash

:<<!
description:
Insert all aivoice example settings (dependencies, linked resources, etc) to a project_dsp

1. run this script with the project path as argument, e.g.
    ./add_all_setings_to_project.sh dsp/source/project/project_dsp

2. cd dsp/source/project/auto_build/ 

3. run auto_build.sh to build the project_dsp

Note: This script must be executed as a sub-module within the ameba_dsp directory structure
    and will not function properly when run from a standalone directory.
!

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)

if [ -z "$1" ] || [ -z "$2" ]; then
    echo "Error: project_dir and aivoice_example_name should be specified."
    echo "Usage: $0 <project_dir> <aivoice_example_name>"
    exit 1
fi

PROJECT_DIR=$1
EXAMPLE_NAME=$2
LIB_RESOURCES_FLAG=$3

EXAMPLE_DIR=${SCRIPT_DIR}/../../${EXAMPLE_NAME}

python ${SCRIPT_DIR}/add_deps.py --project_dir ${PROJECT_DIR}/ --deps_xml ${EXAMPLE_DIR}/platform/ameba_dsp/${EXAMPLE_NAME}_dependencies.xml ${LIB_RESOURCES_FLAG}
python ${SCRIPT_DIR}/add_link.py --project_dir ${PROJECT_DIR}/ --lr_xml ${EXAMPLE_DIR}/platform/ameba_dsp/${EXAMPLE_NAME}_linked_resources.xml