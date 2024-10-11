import socket
import tkinter as tk
import logging
from PIL import Image, ImageTk

# Configure logging level to only show critical errors
logging.basicConfig(level=logging.CRITICAL)

# Global variables
global client_socket
global canal
global wait_right_boats
global wait_left_boats
global direction
global yellow_light
global emergency
global real_time
client_socket = None
canal = []
wait_right_boats = []
wait_left_boats = []
direction = ""
yellow_light = False
emergency = False
real_time = False

def process_message(message):
    global canal, direction, yellow_light, emergency, wait_left_boats, wait_right_boats

    lines = message.split('\n')
    for line in lines:
        if line.startswith('Canal:'):
            canal = [int(x) if x != '-1' else -1 for x in line.split(':')[1].strip().split()]
        elif line.startswith('Direction:'):
            direction = line.split(':')[1].strip()
        elif line.startswith('Yellow Light:'):
            yellow_light = line.split(':')[1].strip().lower() == 'true'
        elif line.startswith('Emergency:'):
            emergency = line.split(':')[1].strip().lower() == 'true'
        elif line.startswith('Left:'):
            wait_left_boats = [int(x) for x in line.split(':')[1].strip().strip('[]').split()]
        elif line.startswith('Right:'):
            wait_right_boats = [int(x) for x in line.split(':')[1].strip().strip('[]').split()]

def connect_to_server():
    global client_socket
    if client_socket is not None:
        return  

    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.setblocking(False) 
    try:
        client_socket.connect(('127.0.0.1', 5000))
    except BlockingIOError:
        # This is expected when using setblocking(False), so we don't show it as an error
        pass 
    except Exception as e:
        logging.error("Error connecting to server: %s", e)
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
                    etiqueta.config(text=s)
                    s = s[:index + len('END_OF_MESSAGE')]
                    process_message(s)
                else:
                    logging.warning("Did not find 'END_OF_MESSAGE'")
            else:
                logging.info("Connection closed by the server")
                close_socket()  # Close the socket if the server closes the connection
        except BlockingIOError:
            # No data available to read, so do nothing
            pass
        except ConnectionResetError as e:
            logging.warning("Connection closed by the server: %s", e)
            close_socket()
        except Exception as e:
            logging.error("Unexpected error: %s", e)
            close_socket()

    else:
        logging.info("No client connected, attempting to reconnect...")
        connect_to_server()
    
    # Retry to receive data every 10 ms
    root.after(10, receive_data)

def close_socket():
    global client_socket
    if client_socket:
        try:
            client_socket.shutdown(socket.SHUT_RDWR)
        except Exception as e:
            logging.warning("Error shutting down the socket: %s", e)
        try:
            client_socket.close()
            logging.info("Socket closed successfully.")
        except Exception as e:
            logging.warning("Error closing the socket: %s", e)
        client_socket = None

def on_closing():
    close_socket()
    root.destroy()
    exit()

# Configure the GUI
root = tk.Tk()
root.title("Cliente GUI en Python")
etiqueta = tk.Label(root, text="Waiting for data...")
etiqueta.pack()

# Properly close the window and the socket
root.protocol("WM_DELETE_WINDOW", on_closing)

# Attempt to connect to the server and continuously receive data
connect_to_server()
receive_data()

# Start the main loop of tkinter
root.mainloop()
