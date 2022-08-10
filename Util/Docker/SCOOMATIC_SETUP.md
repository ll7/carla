
# Dockerized Scoomatic Setup

## About
This tutorial shows how to build the scoomatic vehicle as Docker container.

## Prerequisites

### Machine Setup and Requirements

| Requirement | Description |
| ----------- | ----------- |
| Free Disk Space: | ~ 200-300 GB |
| Available RAM:   |    16 GB |
| CPU:             | as many cores as possible, build is really resource-intensive |
| Suggested OS:    | Ubuntu 18.04 |

### Unreal Engine GitHub Access
1) Register as Unreal developer to access the Unreal Engine GitHub repositories (required for CARLA source build) [UE source code](https://www.unrealengine.com/en-US/ue-on-github)
2) Create a GitHub personal access token for login (pull the Unreal Engine repo)
see GitHub docs https://docs.github.com/en/authentication/keeping-your-account-and-data-secure/creating-a-personal-access-token

### OS Setup
Install the latest updates.

```sh
sudo apt-get update && sudo apt-get upgrade -y
reboot
```

### Python Version (3.6 or higher)
Check for the Python version related to the 'python3' command:

```sh
python3 --version
```

This should return 3.6 or higher.

### Install Docker

```sh
sudo apt-get update && sudo apt-get install -y docker.io
sudo usermod -aG docker $USER && reboot
```

Set up NVIDIA Docker tools as well,
see https://github.com/ll7/paf21-1/wiki/Development-Machine-Setup-(NVIDIA-Docker)

## Setup Steps
This tutorial is following the steps from
https://github.com/carla-simulator/carla/blob/master/Util/Docker/README.md,
but there are some twitsts to fix the inconsistencies.

### Clone the CARLA Source Code

```sh
CARLA_REPO=https://github.com/ll7/carla
CARLA_BRANCH=ll7/scoomatic/0.9.13/simple-reimp

git clone $CARLA_REPO --depth=1 -b $CARLA_BRANCH
cd carla
```

### Docker Image Build

```sh
GITHUB_USERNAME=....
GITHUB_ACCESS_TOKEN=....

docker build -t carla-prerequisites \
    --build-arg EPIC_USER=$GITHUB_USERNAME \
    --build-arg EPIC_PASS=$GITHUB_ACCESS_TOKEN \
    -f Util/Docker/Prerequisites.Dockerfile .

docker build -t carla:latest -f Util/Docker/Carla.Dockerfile-mod .
```

## Simulating The Vehicle

### Launching The Simulator

```sh
docker run --privileged --gpus all \
    --net=host -e 2000-2002 \
    -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
    --name carla_sim carla:latest

# stop simulator with 'docker stop carla_sim'
```

### Spawn + Remote-Control Scoomatic Via PythonAPI

Setup Python on the host system to test against the simulator instance:

```sh
sudo apt-get update && sudo apt-get install -y python3-pip
python3 -m pip install pip --upgrade
python3 -m pip install numpy pygame
```

Copy the PythonAPI from the simulator instance onto the host machine:

```sh
mkdir temp
docker cp carla_sim:/home/carla/PythonAPI ./temp/
cd temp
```

Check if the carla package can be imported:

```sh
export PYTHONPATH=$PWD/PythonAPI/carla:$PWD/PythonAPI/carla/dist/carla-0.9.13-py3.6-linux-x86_64.egg

python3 -c 'import carla'
if [ $? -eq 0 ]; then echo 'carla import successful!'; fi
```

Check if the Scoomatic vehicle is registered:

```py
import carla

client = carla.Client('localhost', 2000)
world = client.get_world()

vehicle_blueprints = world.get_blueprint_library().filter('*vehicle*')
print('all vehicles:', vehicle_blueprints)
print('==========================================')
print('scoomatic vehicles:', vehicle_blueprints.filter('*scoomatic*'))
```

Spawn a new vehicle and control it via the keyboard:

```sh
python3 PythonAPI/examples/manual_control.py --filter *scoomatic*
```

## Author
Marco Tröster, 2022-08
