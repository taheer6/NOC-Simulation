# SystemC Network-on-Chip Router Simulation

## Overview
This was the Final Project for COE 838 (System On Chip Design). This project models and simulates a packet-based Network-on-Chip architecture using SystemC. The design began from a simple 1x2 mesh example and was extended into a functional 4x4 mesh containing 16 interconnected nodes, each with its own source, sink, and router. The goal of the project was to explore how data moves through a scalable on-chip interconnect, how routers coordinate packet forwarding, and how different traffic patterns affect communication across the network. 

<img width="304" height="129" alt="image" src="https://github.com/user-attachments/assets/05fb49d7-cafa-46d0-9662-173b8814e2cb" />

## Scaling the Network
The original 1x2 topology provided a small working reference for understanding packet generation, routing, acknowledgements, and buffering between modules. Once the baseline behaviour was verified, the design was expanded into a 4x4 mesh where routers were connected through north, south, east, and west links, allowing packets to travel hop-by-hop until reaching their intended destination. Moving from the smaller structure to the larger mesh made the project much more representative of how Network-on-Chip systems scale in real System-on-Chip designs.

<img width="384" height="347" alt="image" src="https://github.com/user-attachments/assets/8d27509b-2016-4814-b560-9b9780a34cda" />

## How Packets Move
Communication in the network is handled through source, router, and sink modules working concurrently, which gives the simulation behaviour closer to real hardware. The source module builds packets using fields such as source ID, destination ID, and data, then transmits them in the form of flits made up of header and payload information. From there, routers forward those flits toward the correct node while acknowledgement signals regulate flow control and ensure data only moves when the next stage is ready to receive it.

<img width="511" height="134" alt="image" src="https://github.com/user-attachments/assets/0d9352b7-0769-4fc8-b59d-88b3deb57811" />

## Router Operation
The router is the core of the entire design since it is responsible for buffering incoming data, deciding which packet gets access to an output path, and then forwarding it in the correct direction. In this project, the simplified router structure focuses on the main data path through FIFO buffers, an arbiter, and a crossbar switch. This keeps the design easier to understand while still demonstrating the essential behaviour needed for packet routing and wormhole-style communication inside the mesh.

<img width="315" height="239" alt="image" src="https://github.com/user-attachments/assets/3b73933d-aecc-4535-82d3-85185e12b1dc" />

## Traffic Verification
To test the completed 4x4 mesh, two communication patterns were implemented: uniform traffic and neighbouring traffic. In the uniform pattern, each source sends to a unique destination elsewhere in the mesh, which forces packets to travel across multiple routers and verifies correct multi-hop routing. In the neighbouring pattern, each source communicates only with an adjacent node, which checks that short-distance routing and local communication also behave correctly. Together, these two cases provide a solid validation of both the routing logic and the inter-router connections across the network.

<img width="622" height="475" alt="image" src="https://github.com/user-attachments/assets/c3c6c2d2-d40d-484e-84ca-c50fb6d0c9b3" />

The final simulation results showed that packets were transmitted and received correctly under both traffic conditions, with all flits reaching their intended destinations and no packet loss occurring. Even though packet delay was not explicitly measured in the final implementation, the successful end-to-end delivery confirmed that the mesh, buffering logic, arbiter behaviour, and clock configuration were all working as expected. The project ultimately demonstrates a working and scalable NoC model in SystemC rather than just a small isolated example.

<img width="622" height="474" alt="image" src="https://github.com/user-attachments/assets/23abcc64-ba3b-4537-a1c7-fc6b0cf722db" />

## Final Takeaways
This project was a solid step into understanding how scalable on-chip communication systems are modeled beyond simple bus-based designs. Extending the original 1x2 example into a full 4x4 mesh made it much clearer how packet routing, arbitration, buffering, and acknowledgement-based flow control work together in a larger network. While there is still room for future improvements such as detailed packet delay analysis, the final design successfully validates the behaviour of a multi-node mesh NoC and shows how SystemC can be used to study more realistic digital communication architectures.
