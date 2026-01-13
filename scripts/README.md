## buildOnUbuntu-24_04.sh
This script is used in GitHub Workflow to make sure the project can be compiled. 

### Usage in Docker
To run the script in docker, the recommended approach is as follows.
Make sure the Docker daemon is running:

    sudo systemctl start docker

The following commands will download the script, run it in a new Ubuntu Docker machine and stay in the machine so the results can be checked. Note that the script uses a parameter `docker` to indicate it is not run in the GitHub Workflow.

    wget https://github.com/ichlubna/vkCompViz/blob/main/scripts/buildOnUbuntu-24_04.sh
    docker run --privileged -it --name vkCompViz -v "$(pwd)/buildOnUbuntu-24_04.sh:/buildOnUbuntu-24_04.sh:ro" ubuntu:24.04 bash -c "bash /buildOnUbuntu-24_04.sh docker; exec bash"

The Docker machine can be later removed using:

    docker stop vkCompViz; docker rm vkCompViz



