#!/bin/bash

source ~/.bash_profile

if [[ -z $REQUIRED_TF_VERSION ]]; then
    REQUIRED_TF_VERSION="2.12.*"
fi
echo "Using tensorflow version: $REQUIRED_TF_VERSION"

if [[ -z $REQUIRED_CUDA_VERSION ]]; then
    REQUIRED_CUDA_VERSION="11.8.0"
fi
echo "Using cuda version: $REQUIRED_CUDA_VERSION"

if [[ -z $REQUIRED_CUDNN_VERSION ]]; then
    REQUIRED_CUDNN_VERSION="8.6.0.163"
fi
echo "Using cudnn version: $REQUIRED_CUDNN_VERSION"

NAME=$1

if [[ -z $PROFILES_PATH ]]; then
    if [[ -z $2 ]]; then
        set -e
        echo "ERROR: PROFILES_PATH not found. No second argument to use either. exiting."
        exit 1
    fi
    PROFILES_PATH=$2
    echo "PROFILES_PATH not found. Using second argument for PROFILES_PATH"
fi
echo "Using PROFILES_PATH: $PROFILES_PATH"

THIS_PROFILES_PATH="$PROFILES_PATH/$NAME"
mkdir -p $THIS_PROFILES_PATH

if [[ -z $APP_PATH ]]; then
    APP_PATH="$PROFILES_PATH/apps"
fi

mkdir -p $APP_PATH

API_PATH="$THIS_PROFILES_PATH/api"
RESEARCH_PATH="$API_PATH/models/research"
OBJECT_DETECTION_PATH="$RESEARCH_PATH/object_detection"

if ! command -v curl >/dev/null 2>&1; then
    sudo apt install curl
fi

if ! command -v conda >/dev/null 2>&1; then
    echo "Anaconda not installed. Installing..."

    NEWEST="Anaconda3-$(curl -s https://repo.anaconda.com/archive/ | grep -Eo "Anaconda3-[0-9]+\.[0-9]+(-[0-9]+)?-Linux-x86_64\.sh" | grep -Eo "[0-9]+\.[0-9]+(-[0-9]+)?" | sort -rn | head -n 1)-Linux-x86_64.sh"

    CONDA_INSTALL_DIR="$APP_PATH/conda"

    curl -s -O "https://repo.anaconda.com/archive/$NEWEST"
    bash $NEWEST -b -p $CONDA_INSTALL_DIR

    bash /etc/sclai/scripts/set_debian_env.sh PATH "$CONDA_INSTALL_DIR/bin:\$PATH"

    source ~/.bash_profile

    rm $NEWEST

    if ! command -v conda; then
        echo "Something went wrong when installing conda. Could not find conda. exiting."
        rm -r $CONDA_INSTALL_DIR
        exit 1
    fi

    conda --version
    conda init
fi

echo "Initializing..."
eval "$(conda shell.bash hook)"

ENV_PATH="$API_PATH/sa_$NAME"

conda create -p $ENV_PATH pip python=3.9 -y

echo "Activating environment"
conda activate $ENV_PATH

python --version

if [[ ! $(command -v nvcc >/dev/null 2>&1) || "$(nvcc --version | grep "release" | awk '{print $6}' | cut -d ',' -f1)" == "$REQUIRED_CUDA_VERSION" ]]; then
    echo "cuda $REQUIRED_CUDA_VERSION not installed. installing..."
    conda install -c conda-forge cudatoolkit=$REQUIRED_CUDA_VERSION -y
    python -m pip install nvidia-cudnn-cu11==$REQUIRED_CUDNN_VERSION
fi

mkdir -p $ENV_PATH/etc/conda/activate.d
echo 'CUDNN_PATH=$(dirname $(python -c "import nvidia.cudnn;print(nvidia.cudnn.__file__)"))' >> $ENV_PATH/etc/conda/activate.d/env_vars.sh
echo 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CONDA_PREFIX/lib/:$CUDNN_PATH/lib' >> $ENV_PATH/etc/conda/activate.d/env_vars.sh
echo 'NUM_GPUS=$(nvidia-smi --query-gpu=count --format=csv,noheader)' >> $ENV_PATH/etc/conda/activate.d/env_vars.sh
echo 'export CUDA_VISIBLE_DEVICES=$(echo $(seq 0 $((NUM_GPUS-1))) | sed "s/ /,/g")' >> $ENV_PATH/etc/conda/activate.d/env_vars.sh
echo 'echo "CUDA_VISIBLE_DEVICES set to: $CUDA_VISIBLE_DEVICES"' >> $ENV_PATH/etc/conda/activate.d/env_vars.sh
source $ENV_PATH/etc/conda/activate.d/env_vars.sh

if ! command -v git >/dev/null 2>&1; then
    sudo apt install git -y
fi

if ! command -v protoc --version >/dev/null 2>&1; then
    python -m pip install protobuf
fi

git clone https://github.com/tensorflow/models.git "$API_PATH/models"

cd $RESEARCH_PATH
protoc object_detection/protos/*.proto --python_out=.
cp "$OBJECT_DETECTION_PATH/packages/tf2/setup.py" .
python -m pip install .
python -m pip install tensorflow-text==$REQUIRED_TF_VERSION tf-models-official==$REQUIRED_TF_VERSION tensorflow==$REQUIRED_TF_VERSION
