
import socket
import time

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

def procesar_mensaje(mensaje):
    global canal, direction, yellow_light, emergency, wait_left_boats, wait_right_boats

    lineas = mensaje.split('\n')
    for linea in lineas:
        if linea.startswith('Canal:'):
            canal = [int(x) if x != '-1' else -1 for x in linea.split(':')[1].strip().split()]
        elif linea.startswith('Direction:'):
            direction = linea.split(':')[1].strip()
        elif linea.startswith('Yellow Light:'):
            yellow_light = linea.split(':')[1].strip().lower() == 'true'
        elif linea.startswith('Emergency:'):
            emergency = linea.split(':')[1].strip().lower() == 'true'
        elif linea.startswith('Left:'):
            wait_left_boats = [int(x) for x in linea.split(':')[1].strip().strip('[]').split()]
        elif linea.startswith('Right:'):
            wait_right_boats = [int(x) for x in linea.split(':')[1].strip().strip('[]').split()]

def conectar_al_servidor():
    global client_socket
    if client_socket is not None:
        return  # Ya está conectado o en proceso de conexión

    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.setblocking(False)  # Establecer el socket en modo no bloqueante
    try:
        client_socket.connect(('127.0.0.1', 5000))
    except BlockingIOError:
        pass  # La conexión está en progreso o ya establecida
    except Exception as e:
        print(f"Error al conectar al servidor: {e}")
        cerrar_socket()

def recibir_datos():
    global client_socket
    if client_socket:
        try:
            data = client_socket.recv(4096)
            if data:
                s = data.decode()
                # Buscar la primera aparición de 'END_OF_MESSAGE'
                indice = s.find('END_OF_MESSAGE')

                # Si se encuentra 'END_OF_MESSAGE', cortar el string hasta ese punto
                if indice != -1:
                    # Incluir 'END_OF_MESSAGE' en el resultado
                    s = s[:indice + len('END_OF_MESSAGE')]
                    procesar_mensaje(s)
                else:
                    print("No se encontró 'END_OF_MESSAGE'")
            else:
                # No hay datos, lo que puede indicar que el servidor cerró la conexión
                print("Conexión cerrada por el servidor")
        except BlockingIOError:
            pass  # No hay datos disponibles todavía
        except ConnectionResetError:
            print("Conexión cerrada por el servidor")
            client_socket = None
        except Exception as e:
            print(f"Error: {e}")
            client_socket = None

    else:
        print("No hay cliente conectado")
        conectar_al_servidor()

def cerrar_socket():
    global client_socket
    if client_socket:
        try:
            client_socket.shutdown(socket.SHUT_RDWR)
        except Exception as e:
            print(f"Error al hacer shutdown del socket: {e}")
        try:
            client_socket.close()
            print("Socket cerrado correctamente.")
        except Exception as e:
            print(f"Error al cerrar el socket: {e}")
        client_socket = None

def on_closing():
    cerrar_socket()
    root.destroy()


###### pegue aqui todo #######

# Conectar al servidor
client_socket = None
conectar_al_servidor()

# Bucle principal para recibir datos
try:
    while True:
        recibir_datos()
        print(canal, direction, yellow_light, emergency, wait_left_boats, wait_right_boats)
        ### HARDWARE ###
        time.sleep(0.1)  # Pausa de 100 ms para no saturar la CPU
except KeyboardInterrupt:
    print("Cerrando cliente...")
    cerrar_socket()