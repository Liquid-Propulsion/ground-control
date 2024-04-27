import socket

SERVER_IP = 'Raspberry_Pi_IP'  # Replace with the IP address of your Raspberry Pi
SERVER_PORT = 8080

message_to_send = "Hello from the client!"

# Create a socket object
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect to the server
client_socket.connect((SERVER_IP, SERVER_PORT))


while (True):
    response = client_socket.recv(1024)
    print("Response from server:", response.decode())


# # Send message to the server
# client_socket.sendall(message_to_send.encode())

# # Receive response from the server
# response = client_socket.recv(1024)
# print("Response from server:", response.decode())

# Close the connection
client_socket.close()
