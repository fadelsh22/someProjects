# Software Dependencies

We need to build the binaries for our project and install the dependencies. We use Docker here for the golang image. To install the dependencies, simply run: 

```text
go mod download
```

Below is a snapshot in how our Dockerfile looks like: 



![Dockerfile](https://user-images.githubusercontent.com/48340795/89511603-c4340b00-d797-11ea-91ff-5b1799b614c2.png)

