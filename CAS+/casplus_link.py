import socket, select
import os

ip = "172.16.30.10"

status_code	= {}
status_code['0']	= "Good"
status_code['1012']	= "Argument not supported"
status_code['1022']	= "File does not exist!"

def sendCommand(ip, command):
	#print("Opening socket")
	sock	= socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.connect((ip, 10001))
	
	#print("Sending command")
	sock.send(command + "\n")
	
	#print("Receiving response")
	data	= ""
	while 1:
		got = sock.recv(4096)
		if not got: break
		data += got
	
	response	= data.split("\n", 1)
	header	= response[0].split(" ")
	data	= response[1]
	response_code	= header[0]	
	#print("Handheld responded with status code " + response_code + " (" + status_code[response_code] + ")")
	
	if response_code == "0":
		return (data, header, sock)
	else:
		return (None, header, sock)


def sendData(sock, command):
	sock.send(command)
	data	= ""
	
	while 1:
		got = sock.recv(4096)
		if not got: break
		data += got	
	
	response	= data.split("\n", 1)
	header	= response[0].split(" ")
	
	return (None, header)

def dir(path):
	response	= sendCommand(ip, "dir " + path)
	sock	= response[2]
	sock.close()
		
	if response[0] != None:
		dirs	= response[0].split("\n\n")
		print("Content of " + path)
		print("Size" + "\t" + "Time" + "\t" + "Bits" + "\t\t" + "Name")
		for dir in dirs:
			if dir == "": break
			data	= dir.split("\n")
			name	= data[0]
			size	= data[1].split("=")[1]
			time	= data[2].split("=")[1]
			tyep	= data[3].split("=")[1]
			bits	= data[4].split("=")[1]
			if tyep == "0":
				print(size + "\t" + time + "\t" + bits + "\t\t" + name)
			else:
				print(size + "\t" + time + "\t" + bits + "\t\t" + "<"+ name + ">")
	else:
		print(path + " is not a directory or does not exists! (" + response[1][0] + ")")

def get(path, fle):
	response	= sendCommand(ip, "fget " + path )
	sock	= response[2]
	sock.close()
	
	if response[0]:
		print("Received file of " + response[1][1] + " bytes long")
		FILE	= open(fle, "wb")
		FILE.write(response[0])
		FILE.close()
		print("Wrote data to " + fle)
	else:
		print("Failed to get file ! (" + response[1][0] + ")")


def put(local, path):
	file	= open(local, "rb")
	content	= file.read()
	file.close()
	
	sock	= socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.connect((ip, 10001))
	
	response	= sock.send("fput " + path + " " + str(len(content)) + " 0\n")
	#sock	= response[2]
	sendData(sock, content)
	sock.close()
	
	
	if response[1][0] == "0" and False:
		response	= sendData(sock, content)
		if response[1][0] == "0":
			print("File send")
		else:
			print("Failed to send file (" + response[1][0] + ")")
	else:
		print("Error while starting transfer (" + response[1][0] + ")")
		
	sock.close()

def attr(path):
	response	= sendCommand(ip,"attr " + path)
	sock	= response[2]
	sock.close()

	lines	= response[0].split("\n\n")
	if response[0]:
		for line in  lines:
		 print(line)
	else:
		print("Failed to response ! (" + response[1][0] + ")")	

def mkdir(path):
	response	= sendCommand(ip, "mkdir " + path)
	sock	= response[2]
	sock.close()
		
	if response[0] != None:
		lines	= response[0].split("\n")
		for line in lines:
                  print line
	else:
		print("mkdir failed! (" + response[1][0] + ")")

def rmdir(path):
	response	= sendCommand(ip, "rmdir " + path)
	sock	= response[2]
	sock.close()
		
	if response[0] != None:
		lines	= response[0].split("\n")
		for line in lines:
                  print line
	else:
		print("mkdir failed! (" + response[1][0] + ")")

def fdel(path):
	response	= sendCommand(ip, "fdel " + path)
	sock	= response[2]
	sock.close()
		
	if response[0] != None:
		lines	= response[0].split("\n")
		for line in lines:
                  print line
	else:
		print("fdel failed! (" + response[1][0] + ")")

def move(src, dest):
	response	= sendCommand(ip, "move " + path + " " + dest)
	sock	= response[2]
	sock.close()
		
	if response[0] != None:
		lines	= response[0].split("\n")
		for line in lines:
                  print line
	else:
		print("move failed! (" + response[1][0] + ")")

def copy(src, dest):
	response	= sendCommand(ip, "copy " + path + " " + dest)
	sock	= response[2]
	sock.close()
		
	if response[0] != None:
		lines	= response[0].split("\n")
		for line in lines:
                  print line
	else:
		print("copy failed! (" + response[1][0] + ")")

def info(num):
	response	= sendCommand(ip, "info " + num)
	sock	= response[2]
	sock.close()
		
	if response[0] != None:
		lines	= response[0].split("\n\n")

		for line in lines:
			if line == "": break
			else:
			 print(line)
	else:
		print("info failed! (" + response[1][0] + ")")


def scrn(path, fle):
	response	= sendCommand(ip, "scrn 1 0 38400")
	sock	= response[2]
	sock.close()
        # header
        BMP = "\x42\x4D\x0E\x9E\x00\x00\x00\x00\x00\x00\x76\x00\x00\x00"  
        # dib header
        BMP += "\x28\x00\x00\x00\x40\x01\x00\x00\xF0\x00\x00\x00\x01\x00\x04\x00\x00\x00\x00\x00\x00\x96\x00\x00\xC4\x0E\x00\x00\xC4\x0E\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
        # 4bit greyscale palette
        BMP += "\x00\x00\x00\x00\x11\x11\x11\x00\x22\x22\x22\x00\x33\x33\x33\x00"
        BMP += "\x44\x44\x44\x00\x55\x55\x55\x00\x66\x66\x66\x00\x77\x77\x77\x00"
        BMP += "\x88\x88\x88\x00\x99\x99\x99\x00\xAA\xAA\xAA\x00\xBB\xBB\xBB\x00"
        BMP += "\xCC\xCC\xCC\x00\xDD\xDD\xDD\x00\xEE\xEE\xEE\x00\xFF\xFF\xFF\x00"
	if response[0]:
		print("Received screen dump file of " + response[1][1] + " bytes long")
		FILE	= open(fle, "wb")
                FILE.write(BMP)
                for r in xrange(38400, 0,-160):
		 FILE.write(response[0][r-160:r])  # get file upside down 
		FILE.close()
		print("Wrote data to " + fle)
	else:
		print("Failed to get screen file ! (" + response[1][0] + ")")

def stat(num):
	response	= sendCommand(ip, "stat " + num)
	sock	= response[2]
	sock.close()

	lines	= response[0].split("\n\n")
	if response[0]:
		for line in  lines:
		 print(line)
	else:
		print("stat Failed to response ! (" + response[1][0] + ")")	

def dbg(cmd):
        cmdline = ""
        for arg in  cmd:
         cmdline =  cmdline + arg + " "
	response	= sendCommand(ip, cmdline)
	sock	= response[2]
	sock.close()

	lines	= response[0].split("\n\n")
	if response[0]:
		for line in  lines:
		 print(line)
	else:
		print("No line response, exit code: (" + response[1][0] + ")")	

def osupg(f1, f2):
	file1	= open(f1, "rb")
        file2	= open(f2, "rb")
	content1	= file1.read()
        content1	= file1.read()
	file1.close()
        file2.close()
	
	sock	= socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.connect((ip, 10001))
	
	response	= sock.send("osupg /phx/tmp/manifest_img /phx/tmp/phoenix.img\n")

	sendData(sock, content1)
	sock.close()
	
	
	if response[1][0] == "0" and False:
		response	= sendData(sock, content2)
		if response[1][0] == "0":
			print("File send")
		else:
			print("Failed to send file (" + response[1][0] + ")")
	else:
		print("Error while starting transfer (" + response[1][0] + ")")
		
	sock.close()

print ("Usage")
print ("dir /phx/path/")
print ("get /phx/path/ /localpath/")
print ("put /localpath/ /phx/path/")
print ("move /localpath/ /phx/path/")
print ("copy /localpath/f1 /phx/path/f2")
print ("fdel /localpath/file")
print ("mkdir /phx/path/")
print ("rmdir /phx/path/")
print ("info #")
print ("stat #")
print ("attr /phx/path/dir")
print ("scrn ")
print ("osupg - not implemented")
print ("quit - quit TCP session\n")
#        IP Address. . . . . . . . . . . . : 172.16.84.57
ipinfo = os.popen("ipconfig")
for line in ipinfo.readlines():
 if line.find("IP Address. . . . . . . . . . . .") != -1:   # Win32 ipconfig
  print line
  ip = line[44:-2]
 if line.find("IPv4 Address. . . . . . . . . . .") != -1:   # Win64 ipconfig
  ip = line[40:-1]
 if line.find("IP Address. . . . . . . . . . . .") != -1 or line.find("IPv4 Address. . . . . . . . . . .") != -1: 
  if ip[-1:] == '0': ip = ip[:-1] + "1" ; continue
  if ip[-1:] == '1': ip = ip[:-1] + "2" ; continue
  if ip[-1:] == '2': ip = ip[:-1] + "3" ; continue
  if ip[-1:] == '3': ip = ip[:-1] + "4" ; continue
  if ip[-1:] == '4': ip = ip[:-1] + "5" ; continue
  if ip[-1:] == '5': ip = ip[:-1] + "6" ; continue
  if ip[-1:] == '6': ip = ip[:-1] + "7" ; continue
  if ip[-1:] == '7': ip = ip[:-1] + "8" ; continue
  if ip[-1:] == '8': 
   ip = ip[:-1] + "9"
   continue
  if ip[-1:] == '9': 
   ip = ip[:-1] + "0"
   if ip[-2:-1] == '0': ip = ip[:-2] + "10"
   if ip[-2:-1] == '1': ip = ip[:-2] + "20"
   if ip[-2:-1] == '2': ip = ip[:-2] + "30"
   if ip[-2:-1] == '3': ip = ip[:-2] + "40"
   if ip[-2:-1] == '4': ip = ip[:-2] + "50"
   if ip[-2:-1] == '5': ip = ip[:-2] + "60"
   if ip[-2:-1] == '6': ip = ip[:-2] + "70"
   if ip[-2:-1] == '7': ip = ip[:-2] + "80"
   if ip[-2:-1] == '8': ip = ip[:-2] + "90"
   
print "CAS+ ip:",ip

while True:

	inp = raw_input(">").split()
	if len(inp)>0:
		if inp[0] == "dir":
			path = "/"
			if len(inp) >= 2: path = inp[1]
			dir(path)
			
		if inp[0] == "mkdir":
			path = "/phx/tmp/dummy"
			if len(inp) >= 2: path = inp[1]
			mkdir(path)
			
		if inp[0] == "rmdir":
			path = "/phx/tmp/dummy"
			if len(inp) >= 2: path = inp[1]
			rmdir(path)
			
		if inp[0] == "move":
			if len(inp) == 3: 
			 move(inp[1], inp[2])
	
		if inp[0] == "copy":
			if len(inp) == 3: 
			 copy(inp[1], inp[2])

		if inp[0] == "fdel":
			if len(inp) == 2: 
			 fdel(inp[1])
	
		elif inp[0] == "get":
			path = "/"
			fle = "blaap"
			if len(inp) >= 2: path = inp[1]
			if len(inp) >= 3: fle =  inp[2]
			get(path, fle)
			
		elif inp[0] == "put":
			if len(inp) == 3:
				put(inp[1], inp[2])

		elif inp[0] == "info":
                        num = "1"
			if len(inp) >= 2: num = inp[1]  
                        info(num)

		elif inp[0] == "attr":
			if len(inp) >= 2: path = inp[1]
			attr(path)

		elif inp[0] == "stat":
                        num = "1"
			if len(inp) >= 2: num = inp[1]
			stat(num)

		elif inp[0] == "scrn":
			path = "./"
			fle = "cas+scrn.bmp"
			if len(inp) >= 2: path = inp[1]
			if len(inp) >= 3: fle =  inp[2]
			scrn(path, fle)

		elif inp[0] == "debug":
                        inp.pop(0)
			dbg(inp)

		elif inp[0] == "quit":
                         break
