
# Checkpoint Report: Supporting Multi-User Concurrent Model Update in Voxel Hashing 

(21 Nov 2016)

## Summary

We have:

1. Compiled and run the starter code from [1] on our own computers. We have successfully run the code with video and Kinect input.

2. Studied and understood the code structure

3. Annotated potential code segments we will work on.

4. Profiled the time breakdown of two major components: pose estimation (ICP) and model integration (voxel hashing).

5. Written the test harness to display performance result.


## Objective

Our objective is to enhance the system with capability to handle multiple input streams concunrrently. That is, we imagine a couple of users scanning the environment with multiple devices. 


## Details

### Running and Modifying the starter code

After setting up the project. We made three captures with Kinect and modified the sensor interface so we can directly load the Kinect's binary dump to run our project even without a Kinect at hand.

Below are some screenshots:

![nssh_1](screenshot0.jpg)

![nssh_2](screenshot1.jpg)


### Code Structure

The Voxel Hashing system uses the DXUT application framework. Functions are registered to callbacks to handle input frames, perform 3D reconstruction and render the model on screen.

Each frame undergoes two major steps: (1) Pose estimation via the iterative cloest point (ICP) algorithm and (2) world model update via Voxel Hashing.

### Preliminary Profiles

+ simple.sensor

```
=================Time Stats=================
id      bucket name     total time      average time
[1]     ICP Tracking    2199.00ms       52.3571ms
[2]     Integration     270.00ms        6.2791ms
[3]     Streaming       21.00ms 0.4884ms
===================END======================
```

+ nsh4224_dynamic.sensor

```
id      bucket name     total time      average time
[1]     ICP Tracking    21171.00ms      54.4242ms
[2]     Integration     2554.00ms       6.5487ms
[3]     Streaming       849.00ms        2.1769ms
```

+ nsh4224_static.sensor

```
=================Time Stats=================
id      bucket name     total time      average time
[1]     ICP Tracking    14796.00ms      49.6510ms
[2]     Integration     1967.00ms       6.5786ms
[3]     Streaming       109.00ms        0.3645ms
===================END======================
```

The profiling result shows that ICP tracking is our bottleneck in the single user case.

### Supporting Multiple Input Streams

With multiple input streams, the system will become highly stressed in both compute and memory. For example, when there are *N* users, each user needs to run her own ICP for pose estimation, and the scene model may need to be updated at *N*x frame rate. We anticipate the GPU will become fully loaded as both ICP and Voxel Hashing are run on it.

We propose to tackle this problem by smartly **scheduling** the operations of each input streams. It includes:

1. Re-ordering the ICP and Voxel Hashing steps among all inputs. Specifically, rearranging the order in which the inputs update the scene model may have significantly impact on memory reuse on GPU, because the voxel hash table is too large to reside entirely in GPU memory so there is swapping between CPU and GPU as in [1].

2. Potentially skipping some input frames. When the system becomes highly loaded, we may want to skipping model update with some frames with minimal quality loss. One intuition is to skip a frame if the scene in its frustum is alreadly well reconstructed (as measured by a *confidence* score).

Currently we are considering the following metrics that can be used to schedule the inputs:

+ The estimated camera pose

+ The velocity of the camera

+ The confidence of the frame's frustum



## Challenges

+ Modifying the DXUT-based system to support multiple input streams.

+ Accelerate the ICP algorithm as it is shown to be a bottleneck.

+ Design a mechanism to allow for concurrent update to the voxel hash table (i.e., scene representation) with minimal synchronization overhead.


## Schedule

+ Week ending Dec 16: Tuning and improving performance based on GPU.
+ Week ending Dec 9: Implement concurrent update mechanism to the scene representation.
+ Week ending Dec 2: Optimize ICP; Design and implement better concurrent update mechanism to the scene representation.
+ Week ending Nov 25: Set up the framework to receive multiple input streams and allow them to update the scene representation (with a global lock as start)


References:

[[1] Nießner, M., Zollhöfer, M., Izadi, S., & Stamminger, M. (2013). Real-time 3D reconstruction at scale using voxel hashing. ACM Transactions on Graphics (TOG), 32(6), 169.](http://www.graphics.stanford.edu/~niessner/niessner2013hashing.html)