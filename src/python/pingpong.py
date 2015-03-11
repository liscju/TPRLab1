__author__ = 'lee'
from mpi4py import MPI
import socket
import sys

SEND_RECV_ITERATIONS = 1
BUFFER_SIZES = [ 1000, 5000, 10000,20000,30000,50000,100000,
              200000,300000,500000,1000000,2000000,3000000,
              5000000,6000000,10000000]

AVG_DELAY_TIMES = []
AVG_BANDWIDTH = []

def usage():
    print("Usage: ./run.sh [asyn/sync]")
    exit(0)


def read_command_line_arguments():
    if len(sys.argv) == 1 :
        usage()

    comm_type = None
    if sys.argv[1] == "asyn":
        comm_type = "asyn"
    elif sys.argv[1] == "sync":
        comm_type = "sync"
    else:
        usage()
    
    return comm_type    



def initialize_communication(comm_type):
    comm = MPI.COMM_WORLD
    if comm.size != 2:
        print("Size must be equal 2")
        exit(0)

    comm.Barrier()

    if comm.rank == 0:
        for buff_size in BUFFER_SIZES:
            send_process(comm,buff_size)
    else:
        assert comm.rank == 1
        for buff_size in BUFFER_SIZES:
            recv_process(comm,buff_size)

def invoke_and_calculate_time(fun):
    start_time = MPI.Wtime()
    fun()
    end_time = MPI.Wtime()
    return end_time - start_time

def send_process(comm,buff_size):
    invoke_time = invoke_and_calculate_time(
        lambda: send_iterate(comm,buff_size)
    )
    avg_delay_time = invoke_time/(2*SEND_RECV_ITERATIONS)
    avg_bandwith = (buff_size*SEND_RECV_ITERATIONS*8)/(1024*1024*invoke_time)
    AVG_DELAY_TIMES.append(avg_delay_time)
    AVG_BANDWIDTH.append(avg_bandwith)

    print "BUFFSIZE=" + str(buff_size)
    print "AVG_DELAY_TIME "+str(avg_delay_time)
    print "AVG_BANDWIDTH "+str(avg_bandwith)
    print "--------------------------------"

def send_iterate(comm, buff_size):
    for i in range(0, SEND_RECV_ITERATIONS):
        data = bytearray(buff_size)
        comm.send(data,dest=1)
        comm.recv(source=1)

def recv_process(comm, buff_size):
    for i in range(0,SEND_RECV_ITERATIONS):
        data = comm.recv(source=0)
        reply = bytearray(buff_size)
        comm.send(reply, dest=0)

def main():
    comm_type = read_command_line_arguments()
    initialize_communication(comm_type)


if __name__ == "__main__":
    main()










