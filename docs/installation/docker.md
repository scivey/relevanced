## Docker

The latest version of **relevanced** is available as a Docker image, ready to deploy to any host running Docker or any higher-level container management system (CoreOS, Kubernetes, Compose, ECS).

With [docker](https://www.docker.com/) installed, run:
```bash
sudo docker pull relevanced/relevanced
sudo rocker run --rm -t -i relevanced/relevanced-server:latest
```
