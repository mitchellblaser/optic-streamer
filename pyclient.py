import cv2
import zmq
import numpy as np
import PIL

context = zmq.Context()
socket = context.socket(zmq.SUB)
socket.connect("tcp://localhost:8080")
socket.setsockopt_string(zmq.SUBSCRIBE, "")

while True:
    data = socket.recv()
    print(len(data))
    