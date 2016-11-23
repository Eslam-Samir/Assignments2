from socket import *
from enum import Enum
from time import time

TimeoutValue = 300

# Enum for FSM States
SendState = Enum('SendState', 'WaitCall0 WaitAck0 WaitCall1 WaitAck1')
ReceiveState = Enum('ReceiveState', 'Wait0 Wait1')

NextStateSend = SendState.WaitCall0
NextStateReceive = ReceiveState.Wait0

def extract(msg): # TODO
    return ""

def is_corrupt(msg): # TODO
    return False

def get_sequence(msg): # TODO
    return 0

def TCPOUDPSend(message, serverName, serverPort):
    global NextStateSend
    CurrentState = NextStateSend
    senderSocket = socket(socket.AF_INET, socket.SOCK_DGRAM)
    TimerStart = None
    segment = None
    if CurrentState == SendState.WaitCall0:
        segment = "0" + message
        senderSocket.sendto(segment, (serverName, serverPort)) # udp send
        TimerStart = time.time() # for timeout
        NextStateSend = SendState.WaitAck0

    elif CurrentState == SendState.WaitAck0:
        ack_message = senderSocket.recvfrom(2048) # receive Ack
        if is_corrupt(ack_message) or get_sequence(ack_message) == 1:
            NextSendState = SendState.WaitAck0
        elif (time.time() - TimerStart) >= TimeoutValue: # timeout
            senderSocket.sendto(segment, (serverName, serverPort))  # udp send
            TimerStart = time.time()  # for timeout
            NextStateSend = SendState.WaitAck0
        elif not is_corrupt(ack_message) and get_sequence(ack_message) == 0:
            TimerStart = None # stop timer
            NextSendState = SendState.WaitCall1

    if CurrentState == SendState.WaitCall1:
        segment = "1" + message
        senderSocket.sendto(segment, (serverName, serverPort))  # udp send
        TimerStart = time.time()  # for timeout
        NextStateSend = SendState.WaitAck1

    elif CurrentState == SendState.WaitAck1:
        ack_message = senderSocket.recvfrom(2048)  # receive Ack
        if is_corrupt(ack_message) or get_sequence(ack_message) == 0:
            NextSendState = SendState.WaitAck1
        elif (time.time() - TimerStart) >= TimeoutValue:  # timeout
            senderSocket.sendto(segment, (serverName, serverPort))  # udp send
            TimerStart = time.time()  # for timeout
            NextStateSend = SendState.WaitAck1
        elif not is_corrupt(ack_message) and get_sequence(ack_message) == 1:
            TimerStart = None  # stop timer
            NextSendState = SendState.WaitCall0

    senderSocket.close()


def TCPOUDPReceive(serverPort):
    global NextStateReceive
    receiveSocket = socket(AF_INET, SOCK_DGRAM)
    receiveSocket.bind(('', serverPort))
    message, senderAddress = receiveSocket.recvfrom(2048)
    CurrentState = NextStateReceive
    if CurrentState == ReceiveState.Wait0:
        if is_corrupt(message) or get_sequence(message) == 1:
            receiveSocket.send(message, senderAddress)
        elif not is_corrupt(message) and get_sequence(message) == 0:
            data = extract(message)
            ack_pkt = "0"
            receiveSocket.send(ack_pkt, senderAddress)
            NextStateReceive = ReceiveState.Wait1
            return data


    elif CurrentState == ReceiveState.Wait1:
        if is_corrupt(message) or get_sequence(message) == 0:
            receiveSocket.send(message, senderAddress)
        elif not is_corrupt(message) and get_sequence(message) == 1:
            data = extract(message)
            ack_pkt = "1"
            receiveSocket.send(ack_pkt, senderAddress)
            NextStateReceive = ReceiveState.Wait0
            return data

