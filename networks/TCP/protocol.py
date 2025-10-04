import socket
import threading
import time
import struct
from collections import deque

HEADER_FORMAT = '!IIH'
HEADER_SIZE = struct.calcsize(HEADER_FORMAT)
MAX_PACKET_SIZE = 4096
MAX_PAYLOAD_SIZE = MAX_PACKET_SIZE - HEADER_SIZE
TIMEOUT = 0.005
#WINDOW_SIZE = 69 * MAX_PAYLOAD_SIZE
MAX_RETRIES = 5

class UDPBasedProtocol:
    def __init__(self, *, local_addr, remote_addr):
        self.udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.remote_addr = remote_addr
        self.udp_socket.bind(local_addr)
        self.udp_socket.settimeout(0.01)

    def sendto(self, data):
        return self.udp_socket.sendto(data, self.remote_addr)

    def recvfrom(self, n):
        return self.udp_socket.recvfrom(n)

    def close(self):
        self.udp_socket.close()


class MyTCPProtocol(UDPBasedProtocol):
    FLAG_ACK = 1
    FLAG_DATA = 2

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self.send_base = 0
        self.next_seq_num = 0
        self.expected_seq_num = 0
        self.last_small_send_time = 0

        self.send_buffer = {}  # seq_num: (packet, retries, timestamp)
        self.recv_buffer = {}  # seq_num: (data)
        self.recv_data = deque()

        self.send_lock = threading.Lock()
        self.recv_lock = threading.Lock()
        self.send_event = threading.Event()
        self.recv_event = threading.Event()
        self.ack_events = {}  # seq_num: threading.Event

        self.running = True
        self.sending_big = False

        self.recv_thread = threading.Thread(target=self.recv_loop, daemon=True)
        self.send_thread = threading.Thread(target=self.unresolved_send_handler, daemon=True)
        self.recv_thread.start()
        self.send_thread.start()

    def make_packet(self, seq_num, ack_num, flags, data=b''):
        header = struct.pack(HEADER_FORMAT, seq_num, ack_num, flags)
        return header + data

    def recv_loop(self):
        while self.running:
            try:
                packet, _ = self.recvfrom(MAX_PACKET_SIZE)

                seq_num, ack_num, flags = struct.unpack(HEADER_FORMAT, packet[:HEADER_SIZE])
                data = packet[HEADER_SIZE:]

                with self.send_lock:
                    if ack_num > self.send_base: # got acknowledgment
                        self.send_base = ack_num
                        if not self.sending_big:
                            del self.send_buffer[ack_num]

                    for ack in list(self.ack_events.keys()):
                        if ack_num >= ack: # was confirmed
                            self.ack_events[ack].set()

                if flags & self.FLAG_DATA:
                    with self.recv_lock:
                        if seq_num == self.expected_seq_num: # right order
                            self.recv_data.append(data)
                            self.expected_seq_num += len(data)

                            while self.expected_seq_num in self.recv_buffer:
                                self.recv_data.append(self.recv_buffer.pop(self.expected_seq_num))
                                self.expected_seq_num += len(data)
                            self.recv_event.set()
                        elif seq_num > self.expected_seq_num:
                            self.recv_buffer[seq_num] = data
                        elif seq_num < self.expected_seq_num: # duplicate
                            pass

                    with self.send_lock: # ack packet without actual data
                        ack_packet = self.make_packet(0, self.expected_seq_num, self.FLAG_ACK)
                        self.sendto(ack_packet)

            except socket.timeout:
                continue

    def unresolved_send_handler(self):
        while self.running:
            with self.send_lock:
                current_time = time.time()
                for seq_num in list(self.send_buffer.keys()):
                    packet, retries, timestamp = self.send_buffer[seq_num]
                    if current_time - timestamp >= TIMEOUT:
                        if retries >= MAX_RETRIES:
                            del self.send_buffer[seq_num]
                            # raise/yell or log smth
                            continue
                        self.sendto(packet)
                        self.send_buffer[seq_num] = (packet, retries + 1, current_time)
            time.sleep(0.001)

    def send_full_packet(self, data):
        seq_num = self.next_seq_num
        with self.recv_lock:
            ack_num = self.expected_seq_num

        curr_time = time.time()
        time_since_last_send = curr_time - self.last_small_send_time
        if time_since_last_send < 0.00004:
            time.sleep(0.03)
        self.last_small_send_time = time.time()

        packet = self.make_packet(seq_num, ack_num, self.FLAG_DATA, data)
        with self.send_lock:
            self.send_buffer[seq_num + len(data)] = (packet, 0, time.time())
            self.next_seq_num += len(data)
            self.sendto(packet)

        return len(data)

    def send(self, data: bytes):
        data_len = len(data)
        if data_len <= MAX_PAYLOAD_SIZE:
            return self.send_full_packet(data)
        else:
            return self.send_big_packet(data)

    def send_big_packet(self, data):
        total_sent = 0
        data_len = len(data)

        while total_sent < data_len:
            self.sending_big = True
            with self.send_lock:

                chunk = data[total_sent:total_sent + MAX_PAYLOAD_SIZE]
                seq_num = self.next_seq_num
                with self.recv_lock:
                    ack_num = self.expected_seq_num
                packet = self.make_packet(seq_num, ack_num, self.FLAG_DATA, chunk)

                ack_event = threading.Event()
                self.ack_events[seq_num + len(chunk)] = ack_event

                self.send_buffer[seq_num + len(chunk)] = (packet, 0, time.time())
                self.next_seq_num += len(chunk)
                total_sent += len(chunk)

                self.sendto(packet)

            if not ack_event.wait(TIMEOUT * MAX_RETRIES):
                with self.send_lock:
                    retries = self.send_buffer[seq_num + len(chunk)][1]
                    if retries >= MAX_RETRIES:
                        del self.send_buffer[seq_num + len(chunk)]
                        del self.ack_events[seq_num + len(chunk)]
                        # raise or log smth (maybe TimeoutError (?))
                    else:
                        self.send_buffer[seq_num + len(chunk)] = (packet, retries + 1, time.time())
                        self.sendto(packet)
                        ack_event.clear()
            else: # got acknowledgment
                with self.send_lock:
                    del self.send_buffer[seq_num + len(chunk)]
                    del self.ack_events[seq_num + len(chunk)]

        self.sending_big = False
        return data_len

    def recv(self, n: int):
        received_data = b''
        while len(received_data) < n:
            with self.recv_lock:
                if self.recv_data:
                    data = self.recv_data.popleft()
                    received_data += data
                else:
                    self.recv_lock.release()
                    self.recv_event.wait()
                    self.recv_event.clear()
                    self.recv_lock.acquire()
        return received_data[:n]

    def close(self):
        self.running = False
        self.recv_thread.join()
        self.send_thread.join()
        super().close()
