# MultithreadedUdpReceiver
The first 2 streams receive messages over the UDP protocol, the 3rd send over TCP

## How to clone and build ?
```bash
    git clone https://github.com/Childcity/MultithreadedUdpReceiver.git &&\
    cd ./MultithreadedUdpReceiver && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make -j 4
```

## How to run ?

#### Step 1
##### Open ONE terminal and run next command (this will start two UdpReceivers and TcpSender):

```bash
    # Imagine, you now in the root of the project and have './build' dir...
    cd build && ./MultithreadedUdpReceiver "::1" 6333 6334 6335
```

#### Step 2
##### Open TWO terminals and run next commands in each terminal (this will start two UdpSenders):

```bash
    # Imagine, you now in the root of the project and have './build' dir...
    cd build/UdpSender && ./UdpSender "::1" 6333
```
```bash
    # Imagine, you now in the root of the project and have './build' dir...
    cd build/UdpSender && ./UdpSender "::1" 6334
```

#### Step 3
##### Open ONE terminal and run next command (this will start TcpReceiver):

```bash
    # Imagine, you now in the root of the project and have './build' dir...
    cd build/TcpReceiver && ./TcpReceiver "::1" 6335
```