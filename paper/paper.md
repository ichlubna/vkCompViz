---
title: 'vkCompViz: Universal C++ Library for GPU-Based Experiments'
tags:
  - GPU
  - Vulkan
  - acceleration
  - graphics
  - GPGPU
authors:
  - name: Tomas Chlubna
    orcid: 0000-0003-3126-0545
    affiliation: 1
affiliations:
 - name: Faculty of Information Technology Brno University of Technology, Czech Republic
   index: 1
date: 7 August 2017
bibliography: paper.bib
---

# Summary

GPUs are gaining popularity due to their massive computational parallelism [@jia2021] and usage in interactive graphics [@nguyen2007] or machine learning applications [@mittal2019].
Prototyping GPU-executed experiments is often time-consuming due to the complexity of GPU-related APIs.
Multiplatform and multi-vendor support is also not guaranteed with all existing APIs.
The vkCompViz C++ library offers a simple way to write a GPGPU [@hu2016] (general-purpose computing on graphics processing units) program.
Only the paths to files with GPU code (shader/kernel), paths to input images, or a buffer of arbitrary input data need to be provided by the host application.
The library is capable of running a sequence of compute shaders, processing the input data, and storing the result or presenting it in a window.
Memory usage and computational time can also be automatically measured.

# Statement of need

Necessary data allocations and transfers, and the execution of shaders, require a special API that communicates with the GPU drivers [@plebanski2025; @henriksen2024].
However, using these APIs presents various problems.
Vendor-specific GPGPU API CUDA by NVIDIA and HIP by AMD quickly provide access to new GPU features but can be used only with the given vendor's GPUs.
OpenCL is a multiplatform and vendor-free API that contains a lot of features, but, in practice, GPU drivers usually do not implement its entire specification.
The APIs referenced earlier are tailored only for GPGPU computations and lack direct access to GPU rendering features. 
In contrast, subsequent APIs are capable of using rendering pipelines and GPGPU functions.
DirectX does not depend on the vendor, but is developed by Microsoft and can be used only on Windows.
Similarly, Metal works only on Apple devices.
OpenGL is multiplatform and vendor-free, but it does not support new features and its development is discontinued.
Vulkan is multi-platform, vendor-free, supports new features quickly, and offers low-level optimization settings.
The main disadvantage of Vulkan is its verbosity, which makes it difficult to use for quick experiments, as a substantial portion of the code is necessary even for basic functionality.
All the APIs also require the host application to allocate data in GPU memory, load and decode resources like images, transfer the data, create a window, etc.

The proposed vkCompViz library offers a high-level API that significantly simplifies GPU programming.
The library only requires the paths to the code files to be executed on GPU, paths to the input images, a buffer with arbitrary input data, and a set of parameters.
The library then allocates the necessary memory, creates the GPU-related objects, transfers the data on the GPU, runs the computation (or rendering) pipeline, and returns or stores the results.
The libary can visualize the results in a window, where the parameters can be interactively adjusted at run-time, or can be run in headless mode on machines with no window systems.
The memory usage and time performance of the data transfer and shader execution reporting mechanism is also implemented.
With this library, researchers do not need to learn about complex GPU-related APIs and can quickly conduct scientific experiments on GPU.
This tackles a frequent issue in the scientific domain, where bringing an experimental concept to life usually requires a considerable amount of time owing to the need to acquire novel technological skills and address numerous small, implementation-related challenges.

Compared to existing frameworks used in science that simplify the work with Vulkan, such as vk-bootstrap [@lopez2025], or Auto-Vk-Toolkit [@unter2023], the proposed library does not require an in-depth work with the GPU-related structures.
The library is specially designed for quick experimental prototyping.
Other frameworks often focus on specific tasks, such as Datoviz [@rossant2021] for scientific data visualization, the framework for remote rendering of large data [@lavrivc2018], or VComputeBench for benchmarking purposes [@mammeri2018].
The proposed library aims to be a general GPGPU scientific framework.
The goal is to reduce the time required for GPU programming, particularly since numerous scientific experiments may not produce significant results, and it is inefficient to spend excessive time learning about existing APIs.

# Architecture

The library uses Vulkan [@bailey2019], which ensures support for modern operating systems and GPUs.
Vulkan is expected to be further supported in the future and also quickly adopts novel GPU features which can be possibly used in the library.
The input shaders are expected to be written in the Slang language, which is a universal modern language designed for GPU shaders.
The input images are loaded with the FFmpeg library, which supports a wide range of multimedia formats.
The window is created by the GLFW multi-platform library.
The library workflow and architecture are described in \autoref{fig:architecture}.

![The figure describes the architecture of vkCompViz library.\label{fig:architecture}](vkCompViz.pdf){width=60%}

# Usage
The project uses CMake, C++26 standard, and C++20 modules.
The usage of the library is demonstrated and documented in three basic example subprojects.
The Simple Blending example demonstrates operations with images in which two images can be blended together with a given factor.
The 3D Viewer also shows how the rendering pipeline can be customized to render an input 3D model file.
The Parallel Reduction example shows how the library can be used to accelerate a summation of a big array of numbers.
This example demonstrates a simple use case of an experimental evaluation of the GPU accelerated algorithm [@jradi2020], compared to its CPU variant.
The experiment was carried out on a machine equipped with a NVIDIA GeForce RTX 3060 Ti and Intel(R) Core(TM) i5-8500 CPU @ 3.00GHz CPU, running Arch Linux.
The output of several runs of this example is shown in \autoref{fig:reduction}.
\autoref{fig:reductionZoom} shows the detailed runs where the GPU started to be faster than CPU.
The results show that computational time is shorter with massively parallel GPU architecture.
However, the data transfer delay increases the total time and shows that using GPU for this use case is viable only when the data is already generated on GPU or later used there or when the data size reaches certain amount.
This fact is in alignment with previously published findings [@dinkelbach2012].

![Comparison of GPU and CPU array summation is shown in the chart.\label{fig:reduction}](reduction.pdf){width=100%}

![The turning point where GPU starts to be faster than CPU for array summation is shown with two neighboring masurements.\label{fig:reductionZoom}](reductionZoom.pdf){width=100%}

# References
