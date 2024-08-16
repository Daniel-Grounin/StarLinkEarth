# StarLinkEarth

## Overview

**StarLinkEarth** is an interactive real-time visualization project that displays the movement of Starlink satellites around the Earth. This project combines the power of OpenFrameworks for rendering 3D graphics in C++ with Python for calculating satellite positions based on real-time data.

## Features

- **Real-time Visualization**: Displays live positions of Starlink satellites.
- **3D Earth Rendering**: A dynamic, interactive 3D model of Earth with satellites orbiting in real-time.
- **GUI Toggle**: Easily toggle satellite names on and off.
- **Satellite Count**: Real-time display of the number of satellites visible on screen.

## Installation

1. **Clone the repository:**
   ```bash
   git clone https://github.com/yourusername/StarLinkEarth.git

2. **Install dependencies:**

- **OpenFrameworks**: Ensure you have OpenFrameworks installed.
- **Python**: Make sure Python is installed with the necessary packages (`sgp4`, `json`, `sockets`).

## Build and run

1. **Open the project** in your IDE and build it.
2. **Run the Python script** to start fetching and streaming satellite data.

## How It Works

- **Data Source**: The project pulls data from CelesTrak, providing up-to-date orbital information for Starlink satellites.
- **Python Script**: Calculates and streams the satellites' coordinates to the C++ application in real-time.
- **OpenFrameworks Rendering**: The C++ application renders a 3D model of Earth and satellites using the data received from Python.

## Acknowledgements

- **CelesTrak**: For providing orbital data for the satellites.
- **OpenFrameworks**: For the graphical rendering framework.
