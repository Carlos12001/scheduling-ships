import Jetson.GPIO as GPIO
import socket
import time

global client_socket
global canal
global wait_right_boats
global wait_left_boats
global direction
global yellow_light
global real_time
client_socket = None
canal = []
wait_right_boats = []
wait_left_boats = []
direction = ""
yellow_light = False
real_time = False

def process_message(message):
    global canal, direction, yellow_light, wait_left_boats, wait_right_boats, real_time

    lines = message.strip().split('\n')
    for line in lines:
        if line.startswith('Canal:'):
            canal = [int(x) if x != '-1' else -1 for x in line.split(':')[1].strip().split()]
        elif line.startswith('Direction:'):
            direction = line.split(':')[1].strip()
        elif line.startswith('Yellow Light:'):
            yellow_light = line.split(':')[1].strip().lower() == 'true'
        elif line.startswith('Left:'):
            wait_left_boats = [int(x) for x in line.split(':')[1].strip().strip('[]').split()]
        elif line.startswith('Right:'):
            wait_right_boats = [int(x) for x in line.split(':')[1].strip().strip('[]').split()]
        elif line.startswith('TiempoReal:'):
            real_time = line.split(':')[1].strip().lower() == 'true'  

def connect_to_server():
    global client_socket
    if client_socket is not None:
        return  

    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.setblocking(False) 
    try:
        client_socket.connect(('127.0.0.1', 5000))
    except BlockingIOError:
        pass 
    except Exception as e:
        print("Error connecting to server:", e)
        close_socket()

def receive_data():
    global client_socket
    if client_socket:
        try:
            data = client_socket.recv(4096)
            if data:
                s = data.decode()

                index = s.find('END_OF_MESSAGE')

                if index != -1:
                    s = s[:index + len('END_OF_MESSAGE')]
                    process_message(s)
                else:
                    print("Did not find 'END_OF_MESSAGE'")
            else:
                print("Connection closed by the server")
        except BlockingIOError:
            pass
        except ConnectionResetError as e:
            print("Connection closed by the server")
            print(e)
            close_socket()
        except Exception as e:
            print("Error:")
            print(e)
            close_socket()

    else:
        print("No client connected")
        connect_to_server()

def close_socket():
    global client_socket
    if client_socket:
        try:
            client_socket.shutdown(socket.SHUT_RDWR)
        except Exception as e:
            print("Error shutting down the socket:")
            print(e)
        try:
            client_socket.close()
            print("Socket closed successfully.")
        except Exception as e:
            print("Error closing the socket:")
            print(e)
        client_socket = None
    else:
        client_socket = None

def on_closing():
    close_socket()

# Connect to the server
client_socket = None
connect_to_server()

'''Functionality to run hardware'''
LED_DIRECCION = 40
LED_YELLOW_LIGHT = 13
LED_LEFT_BOAT_MS = 11
LED_LEFT_BOAT_LS = 12
LED_RIGHT_BOAT_MS = 37
LED_RIGHT_BOAT_LS = 38

GPIO.setmode(GPIO.BOARD)
GPIO.setup(LED_DIRECCION, GPIO.OUT)
GPIO.setup(LED_YELLOW_LIGHT, GPIO.OUT)
GPIO.setup(LED_LEFT_BOAT_MS, GPIO.OUT)
GPIO.setup(LED_LEFT_BOAT_LS, GPIO.OUT)
GPIO.setup(LED_RIGHT_BOAT_MS, GPIO.OUT)
GPIO.setup(LED_RIGHT_BOAT_LS, GPIO.OUT)

my_channel_dict = {1: (15, 16, 18), 2: (19, 21, 22), 3: (23, 24, 26), 4: (29, 31, 32), 5: (33, 35, 36)}
    # Cada numero indica la posicion de su respectivo LED en el canal
    # Cada tupla indica los pines correspondientes para el control de cada LED

my_boats_dict = {-1: (0,0,0), 1: (0,1,0), 2: (0,0,1), 3:(1,0,0)}
# -1: Estado inicial (No hay ningun barco en el canal)
# 1: Normales (Green / Verdes)
# 2: Pesquero (Blue / Azul)
# 3: Patrullas (Red / Rojos)

# Configurar los pines del canal
for pins in my_channel_dict.values():
    GPIO.setup(pins[0], GPIO.OUT)
    GPIO.setup(pins[1], GPIO.OUT)
    GPIO.setup(pins[2], GPIO.OUT)

def set_color(red, green, blue, r_pin, g_pin, b_pin):
    GPIO.output(r_pin, red)
    GPIO.output(g_pin, green)
    GPIO.output(b_pin, blue)

def show_leds(canal, direction, yellow_light, wait_left_boats, wait_right_boats):
    try:
        if (yellow_light):
            GPIO.output(LED_YELLOW_LIGHT, GPIO.HIGH)
        else:
            GPIO.output(LED_YELLOW_LIGHT, GPIO.LOW)

        if (direction == "Right"):
            GPIO.output(LED_DIRECCION, GPIO.HIGH)  # Enciende el LED 10

            if wait_left_boats:
                left_boat = wait_left_boats[0]
                ## Switch statement:
                if (left_boat == 1):
                    GPIO.output(LED_LEFT_BOAT_MS, GPIO.LOW)
                    GPIO.output(LED_LEFT_BOAT_LS, GPIO.HIGH)
                elif (left_boat == 2):
                    GPIO.output(LED_LEFT_BOAT_MS, GPIO.HIGH)
                    GPIO.output(LED_LEFT_BOAT_LS, GPIO.LOW)
                elif (left_boat == 3):
                    GPIO.output(LED_LEFT_BOAT_MS, GPIO.HIGH)
                    GPIO.output(LED_LEFT_BOAT_LS, GPIO.HIGH)

        else:
            GPIO.output(LED_DIRECCION, GPIO.LOW)  # Enciende el LED 10

            if wait_right_boats:
                right_boat = wait_right_boats[0]
                ## Switch statement:
                if (right_boat == 1):
                    GPIO.output(LED_RIGHT_BOAT_MS, GPIO.LOW)
                    GPIO.output(LED_RIGHT_BOAT_LS, GPIO.HIGH)
                elif (right_boat == 2):
                    GPIO.output(LED_RIGHT_BOAT_MS, GPIO.HIGH)
                    GPIO.output(LED_RIGHT_BOAT_LS, GPIO.LOW)
                elif (right_boat == 3):
                    GPIO.output(LED_RIGHT_BOAT_MS, GPIO.HIGH)
                    GPIO.output(LED_RIGHT_BOAT_LS, GPIO.HIGH)



        

        #my_channel_dict = {1: (15, 16, 18), 2: (19, 21, 22), 3: (23, 24, 26), 4: (29, 31, 32), 5: (33, 35, 36)}

        for i in range(len(canal)):
            pines = my_channel_dict[i+1]
            if canal[i] == -1:
                set_color(0, 0, 0, pines[0], pines[1], pines[2])

            if canal[i] == 1:
                set_color(0, 1, 0, pines[0], pines[1], pines[2])

            if canal[i] == 2:
                set_color(0, 0, 1, pines[0], pines[1], pines[2])

            if canal[i] == 3:
                set_color(1, 0, 0, pines[0], pines[1], pines[2])

        
    except KeyboardInterrupt:
        GPIO.cleanup()  # Limpia la configuracion GPIO
    

def main():
    # Main loop to receive data
    try:
        while True:
            receive_data()
            
            if canal != []:
                #print(canal, direction, yellow_light, emergency, wait_left_boats, wait_right_boats)
                show_leds(canal, direction, yellow_light, wait_left_boats, wait_right_boats)
            # Pause for 100 ms to avoid CPU overuse
            #time.sleep(0.1)
    except KeyboardInterrupt as e:
        print("Shutting down client...")
        print(e)
        close_socket()

main()