# Installation Process

This project is implemented in Go. It can be developed using any other language such as Python but we opted for Go because it's fast. Also, we use Docker to set up the project. This helps to run the services eaisly in containers. There are 2 services:  

1- push\_mongodb \(database\)

2- push\_service \(application\)

There's also one micro-service that we use to help stream the data to Telegram: Apache Kafka. 

For more infromation on how to install Docker, please refer to the offical guide [Get Docker](https://docs.docker.com/get-docker/).



## YAML file

For reference, below is a snapshot of the steps that we have taken in our YAML file:



![docker-compose.yml](https://user-images.githubusercontent.com/48340795/89509153-8e415780-d794-11ea-996e-0311f6dc32b4.png)

