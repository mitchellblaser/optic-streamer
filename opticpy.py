import zmq
import cv2
import numpy
import sys
import time

msg = """   ___       _   _    ___ _                              
  / _ \ _ __| |_(_)__/ __| |_ _ _ ___ __ _ _ __  ___ _ _ 
 | (_) | '_ \  _| / _\__ \  _| '_/ -_) _` | '  \/ -_) '_|
  \___/| .__/\__|_\__|___/\__|_| \___\__,_|_|_|_\___|_|  
       |_| github.com/mitchellblaser/optic-streamer 
       """

if len(sys.argv) < 2:
    addr = "10.10.10.182"
    port = 8080
    winScale = 1.5
    statInterval = 10
else:
    addr = sys.argv[1]
    port = sys.argv[2]
    winScale = float(sys.argv[3])
    statInterval = int(sys.argv[4])

print(msg)
print("Using Options: ADDR:" + addr + " PORT:" + str(port) + " SCALE:" + str(winScale) + " UPDATE:" + str(statInterval) + ".")
print("Press ESC to quit.")
context = zmq.Context()
socket = context.socket(zmq.SUB)
socket.setsockopt(zmq.RCVTIMEO, 10)
socket.connect("tcp://" + addr + ":" + str(port))
socket.subscribe("")

cv2.namedWindow("OpticStreamer PyClient", cv2.WINDOW_NORMAL)

def GetImage():
    try:
        global img
        global data
        data = socket.recv()
        d = numpy.frombuffer(data, dtype=numpy.byte)
        img = cv2.imdecode(d, 1)
        return True
    except:
        return False

print("Waiting for Server...")
while True:
    if GetImage() == True:
        break
print("Connection Found.")
img_res = img.shape
resx=img_res[1]*winScale
resy=img_res[0]*winScale
cv2.resizeWindow("OpticStreamer PyClient", round(resx), round(resy))
print("Stream Resolution: " + str(img_res[1]) + "x" + str(img_res[0]) + "px.")
# f = open("test.jpg", "wb")
# f.write(data)
# f.close()
UpdateCounter = 0
FPSCount = [0, 0, 0]
Timer = 0
TimerCache = 0

print("FPS: 0", end='')

while True:
    if(GetImage() == True):
        if time.time() > TimerCache+1:
            TimerCache = time.time()
            FPSCount[0] = FPSCount[1]
            FPSCount[1] = FPSCount[2]
            FPSCount[2] = UpdateCounter
            UpdateCounter = 0
            Timer = Timer + 1
            if Timer > statInterval:
                print('\r', end='')
                print("FPS: " + str((FPSCount[0]+FPSCount[1]+FPSCount[2])/3), end='')
                Timer = 0
        cv2.imshow("OpticStreamer PyClient", img)
        if cv2.waitKey(1) == 27:
            break
        UpdateCounter = UpdateCounter+1

cv2.destroyAllWindows()