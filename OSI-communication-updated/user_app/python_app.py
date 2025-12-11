#!/usr/bin/env python3
"""
Python user application (monitor).
Connects to client monitor port (localhost:10001).
Type messages to send: each line you type is sent to the client as a SEND command.
The Python app receives JSON progress updates and prints a line each time two layers complete.
"""

import socket
import json
import threading

CLIENT_MONITOR_HOST = '127.0.0.1'
CLIENT_MONITOR_PORT = 10001

def recv_loop(sock):
    buffer = b''
    while True:
        try:
            data = sock.recv(4096)
            if not data:
                print("Monitor connection closed by client.")
                break
            buffer += data
            # messages are newline terminated
            while b'\n' in buffer:
                line, buffer = buffer.split(b'\n', 1)
                try:
                    msg = json.loads(line.decode('utf-8'))
                except Exception as e:
                    print("Invalid JSON from client monitor:", line)
                    continue
                if msg.get('type') == 'progress':
                    direction = msg.get('direction', 'TX')
                    completed = msg.get('completed', [])
                    # Print as required: after every two layers are passed client will send this
                    print(f"[{direction}] Completed: {', '.join(completed)}")
                elif msg.get('type') == 'info':
                    print("[INFO]", msg.get('text',''))
                elif msg.get('type') == 'packet':
                    # full packet - print summarized
                    layers = msg.get('layers', [])
                    payload = msg.get('payload','')
                    print(f"[PACKET] layers={len(layers)} payload='{payload}'")
                else:
                    print("[UNKNOWN]", msg)
        except Exception as e:
            print("Monitor recv error:", e)
            break

def main():
    print(f"Connecting to client monitor at {CLIENT_MONITOR_HOST}:{CLIENT_MONITOR_PORT} ...")
    with socket.create_connection((CLIENT_MONITOR_HOST, CLIENT_MONITOR_PORT)) as sock:
        print("Connected to client. Type messages to send to server (blank line to quit).")
        t = threading.Thread(target=recv_loop, args=(sock,), daemon=True)
        t.start()
        while True:
            try:
                line = input('> ').strip()
            except EOFError:
                break
            if line == '':
                break
            # send SEND command as simple text line
            cmd = f"SEND:{line}\n"
            try:
                sock.sendall(cmd.encode('utf-8'))
            except BrokenPipeError:
                print("Connection broken.")
                break
        print("Closing monitor connection.")

if __name__ == '__main__':
    main()

