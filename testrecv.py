import zmq

context = zmq.Context()

socket = context.socket(zmq.SUB)
socket.subscribe("")
socket.connect("tcp://10.10.10.181:5584")

message = socket.recv()
print(message)

