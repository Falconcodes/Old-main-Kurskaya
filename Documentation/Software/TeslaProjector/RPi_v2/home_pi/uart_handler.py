#! /usr/bin/env python

import subprocess, os, time, sys, select, serial

uart_port = '/dev/ttyAMA0'
video_file = 'avengers-2-super-bowl_1280.mp4'

def uart_flush():
	ser = serial.Serial(uart_port, 9600, timeout=1)
	trash = ser.read()

def uart_poll():
	ser = serial.Serial(uart_port, 9600, timeout=1)
	response = None
	while True:
		time.sleep(0.5)
		response = ser.read(1)
		if response == '':
			continue
		else:
			print ('got', response)
			break
	ser.close()
	return response

def keyboard_poll():
	i,o,e = select.select([sys.stdin],[],[],0)
	for s in i:
		if s == sys.stdin:
			input = sys.stdin.readline().rstrip()
			return input
	return None

def run_proc(cmd, param=''):
    command = os.getcwd() + '/' + cmd + '.sh'
    print command
    proc = subprocess.Popen(['bash', command, param], shell=False, stdout=subprocess.PIPE)
    return proc

def check_command_result(command, substring, param=''):
	while True:
		proc = run_proc(command, param)
		found = False
		for line in iter(proc.stdout.readline,''):
			if substring in line:
				found = True
				break

		if found:
			print "+ ENSURED + "
			return True

		print "+ TRY AGAIN +"
		time.sleep(2.5)

def ensure_start(vfile):
	check_command_result('play', substring='"id":3,"jsonrpc":"2.0","result":"OK"', param=vfile)

def ensure_pause():
	check_command_result('pause', substring='"speed":0')

def ensure_play():
	check_command_result('pause', substring='"speed":1')


# === MAIN ===
uart_flush()

ensure_start(video_file)
time.sleep(0.5)
ensure_pause()

while True:
	try:
		key = uart_poll()
	except:
		key = None

	#key = keyboard_poll()
	if key == 's':
		print "START"
		ensure_start(video_file)
		continue
	if key == 'p':
		print "PAUSE!"
		ensure_pause()
		continue
	if key == 'g':
		print "GO!"
		ensure_play()
		continue
	if key == 'o':
		print "OFF"
		run_proc('shutdown')
		continue
	if key == 'r':
			print "REBOOT"
			run_proc('reboot')
			continue

	if key != None:
			print "NOT RECOGNIZED"
	time.sleep(0.1)

print ("=== FINISHED ===")
sys.exit()
