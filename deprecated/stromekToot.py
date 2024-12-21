from picamera import PiCamera
from time import sleep
from datetime import datetime
from gpiozero import CPUTemperature
from mastodon import Mastodon#

mastodon = Mastodon(api_base_url = "https://mastodon.social", access_token = "")
curr_time = datetime.now()
cpu = CPUTemperature()
cam_toot_text = curr_time.strftime("%d.%m.%Y %H:%M") + " | RPi 1 A+ " + str(round(cpu.temperature,1)) + "'C"

currHour = int(curr_time.strftime("%H"))
if currHour <= 23 and currHour >= 6 or currHour == 0:
	print("preparing to toot")
	camera = PiCamera()
	camera.resolution = (1296, 972)
	camera.framerate = 1
	camera.annotate_text = cam_toot_text
	camera.capture('/home/pi/StromekInternetovy/snapshot.jpg')
	sleep(5)
	snapshot = mastodon.media_post("/home/pi/StromekInternetovy/snapshot.jpg", mime_type="image/jpeg")
	mastodon.status_post("Biskupovský stromek\n"  + cam_toot_text, media_ids=[snapshot.id])
	#print(snapshot)
