# Introduction

This repository contains my completed lab tasks for cc3501 Week 7 (OpenCV) and week 8 (Networking).

# OpenCV (Lab 7)

This program uses a Raspberry Pi camera module 2 to detect a coloured object in real time using OpenCV

## How it works

1. A Gstreamer pipline captures frames from the cam and downsamples to 400x300 for faster processing.

2. Each frame is converted to BGR to HSV, allowing for more robust colour-based thresholding.

3.Six trackbars (H/S/V min/max) are used for live configuration of a binary mask that marks every pixel that
falls inside the selected range using `cv::inRange`.

4. Open-close morphological operation is executed on the thresholded image to remove initial background noise,
then fill gaps within the detected object. Since obvious foreground objects are the focus, open-close morphology
was delibrately used here for the best result. The size of the structuring element is also adjustable via a trackbar.

5. `cv::findContours` extracts the outline of every distinct region in clean mask. the largest by area is assumed to be
the target object.

6. Image moments of the chosen contour are used to calculate the objects centroid, which is marked by a crosshair
within the objects contour lines.

# Networking (Lab 8)

This program allows the user to post any message they want under a specified client username to the CC3501
message board. An executable binary was given that allows the user to ping the server with a hardcoded
messsage "Offical app says hello" with no way to customise.

## Uncovering the API from the binary app

The first step to building the actual cpp program, was to find the secret API key that was used to post to
the message board, the server being hosted on a ThingSpeak channel. The key was found by:

- Writing to the board using the binary app and capturing the traffic on Wireshark.

- Inspecting the HTTP packets which contained the full URI request.

- Extracting the API key which was contained in the URI request string and hardcoding the API into the program
to authenticate as a genuine user.

## How it works

1. The program takes two arguments a username `argv[1]` and a message `argv[2]`

2. `curl_easy_escape()` percent-encodes the username and message so spaces and special characters survive
being sent as URL query parameter.

3. The API key, username, message are then concatenated into the same query string format used by the binary app.

4. `libcurl` performs a GET request to the contructed URL

## Usage

```
./post_mesage <username> <message>
```