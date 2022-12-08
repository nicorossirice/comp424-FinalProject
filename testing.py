from time import sleep

import car

car.initialize_car()
sleep(1)
car.go()
sleep(1)
print("Another sleep")
# sleep(1)
print("Checking speed")
car.check_speed()
print('Sleeping')
sleep(2)
print("Stopping")
car.stop()