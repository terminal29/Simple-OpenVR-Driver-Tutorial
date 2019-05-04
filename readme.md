# OpenVR Driver Examples

This repo contains a set of OpenVR driver examples I have written for you to copy, use, and learn from using c++17. I have tried my best to add useful comments to make it easier to understand what everything means, but if you have any other questions, or want to contribute, please raise an issue.

I originally planned some more examples for this but my priorities have moved elsewhere, so I'm posting this update as-is.

If you are looking for the original tutorial, it is still available on the wiki, and the files are available on [this commit](https://github.com/terminal29/Simple-OpenVR-Driver-Tutorial/tree/df1a41a02edca88dba7426c9e5b6d5354f91d299).

### Projects

1. Basic Setup (Tracked HMD & Controllers)
2. Trackers
3. Compositor (Custom Compositor Window) (Incomplete)

### Building

Clone this repository and the submodule(s) using

```bat
git clone --recursive https://github.com/terminal29/Simple-OpenVR-Driver-Tutorial.git
```

And build with CMake

```bat
cd <insert name here>
mkdir build
cd build
cmake ..
```

(or just use CMake GUI)
