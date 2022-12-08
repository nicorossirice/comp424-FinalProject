import cv2
import numpy as np
import math
import time
import Adafruit_BBIO.PWM as PWM

#throttle
throttle_pin="P8_13"

#Steering
steering_pin="P9_14"

# Default throttle speed
throttle_speed =  7.925

stop_threshold = 500
def check_for_stop_sign(frame):

	# Range of red in HSV
	lower_red = np.array([160,100,0], dtype="uint8")
	upper_red = np.array([180,255,255], dtype="uint8")
	
	# Filter out non red pixels and get count of number of red pixels
	mask = cv2.inRange(frame,lower_red, upper_red)
	num_red_px = cv2.countNonZero(mask)

	return num_red_px  > stop_threshold

def convert_to_HSV(frame):
	hsv = cv2.cvtColor(frame,cv2.COLOR_BGR2HSV)
	return hsv




######################################



default_angle = 7.5

camera = cv2.VideoCapture(0)
camera.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc('X','2','6','4'))
camera.set(cv2.CAP_PROP_FRAME_WIDTH,150)
camera.set(cv2.CAP_PROP_FRAME_HEIGHT,120)

time.sleep(1)

# initialize steering and throttle
PWM.start(steering_pin,7.5,50,0)
PWM.start(throttle_pin,7.5,50,0)

time.sleep(2)

# Default throttle speed
PWM.set_duty_cycle(throttle_pin,throttle_speed)

# Default PWM associated with driving straight
default_duty = 7.5

lastTime = 0
lastError = 0

kp = 10
kd = .1

has_stopped_before = False


# Frame count
count = 1

while camera.isOpened():
    ret,frame = camera.read()

    frame = cv2.resize(frame,(320,180),interpolation = cv2.INTER_AREA)
    hsv = convert_to_HSV(frame)
    
    # Every 10 frames, check for a stopsign
    if (count % 10) == 0:

        stop_sign = check_for_stop_sign(hsv)

        if stop_sign and (has_stopped_before == False):

                # First sighting of stop sign, stop car 
                PWM.set_duty_cycle(throttle_pin, 7.5)
                has_stopped_before = True
                time.sleep(2)

                # Continue after stopping for 2 seconds
                PWM.set_duty_cycle(throttle_pin, throttle_speed)
                stop_sign = False

        # Once a second stop sign is encountered, stop car indefinitely
        elif stop_sign and (has_stopped_before == True):
                print("DONE")
                while(1):
                    PWM.set_duty_cycle(throttle_pin, 7.5)
                    PWM.set_duty_cycle(steering_pin, 7.5)
        else:
            continue
    c+=1
    edges = detect_edges(frame)
    roi = region_of_interest(edges)
    line_segments = detect_line_segments(roi)
    lane_lines = average_slope_intercept(frame,line_segments)
    lane_lines_image = display_lines(frame,lane_lines)
    steering_angle = get_steering_angle(frame, lane_lines)
    heading_image = display_heading_line(lane_lines_image,steering_angle)
    #cv2.imshow("heading line",heading_image)


    now = time.time() # current time variable
    dt = now - lastTime
    deviation = -(steering_angle - 90) # equivalent to angle_to_mid_deg variable
    print("deviation = " , deviation)

    if deviation < 4 and deviation > -4: # do not steer if there is a 4-degree error range
        deviation = 0

    else:
        # Correct steering if deviation is large enough
        derivative = kd * (deviation - lastError) / dt / 90.0
        proportional = (kp * deviation) / 90.0
        PD = float(default_duty + derivative + proportional)

        # Maximum steering conditions
        if PD > 9:
            PD = 9
        elif PD < 6:
            PD = 6

        # Correct steering 
        PWM.set_duty_cycle(steering_pin, PD)

    lastError = deviation
    lastTime = time.time()

    key = cv2.waitKey(1)
    if key == 27:
        break

# Clean up camera and display
camera.release()
cv2.destroyAllWindows()
# Clean up GPIO
PWM.set_duty_cycle(throttle_pin, 7.5)
PWM.set_duty_cycle(steering_pin,7.5)
PWM.stop(steering_pin)
PWM.stop(throttle_pin)
PWM.cleanup()
