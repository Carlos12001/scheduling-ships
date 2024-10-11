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

# Main loop to receive data
try:
    while True:
        receive_data()
        if canal != [] and direction != "":
            print(canal, direction, yellow_light, real_time, wait_left_boats, wait_right_boats)
        # Pause for 100 ms to avoid CPU overuse
        time.sleep(0.1)
except KeyboardInterrupt as e:
    print("Shutting down client...")
    print(e)
    close_socket()
