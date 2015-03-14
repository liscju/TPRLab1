__author__ = 'lee'
from mpi4py import MPI
import socket
import sys

SEND_RECV_ITERATIONS = 100
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


def save_delay_times(comm_type):
    f = open('p_delay'+comm_type+'.txt','w+')
    f.write("# X Y\n")
    for i in range(0,len(BUFFER_SIZES)):
        f.write(str(BUFFER_SIZES[i]) + " " + str(AVG_DELAY_TIMES[i]) + "\n")
    f.close()


def save_bandwidth(comm_type):
    f = open('p_bandwidth'+comm_type+'.txt','w+')
    f.write("# X Y\n")
    for i in range(0,len(BUFFER_SIZES)):
        f.write(str(BUFFER_SIZES[i]) + " " + str(AVG_BANDWIDTH[i]) + "\n")
    f.close()


def save_results(comm_type):
    save_delay_times(comm_type)
    save_bandwidth(comm_type)


def initialize_communication(comm_type):
    comm = MPI.COMM_WORLD
    if comm.size != 2:
        print("Size must be equal 2")
        exit(0)

    comm.Barrier()

    if comm.rank == 0:
        for buff_size in BUFFER_SIZES:
            send_process(comm,buff_size,comm_type)
    else:
        assert comm.rank == 1
        for buff_size in BUFFER_SIZES:
            recv_process(comm,buff_size,comm_type)

    if comm.rank == 0:
        save_results(comm_type)

def invoke_and_calculate_time(fun):
    start_time = MPI.Wtime()
    fun()
    end_time = MPI.Wtime()
    return end_time - start_time

def send_process(comm,buff_size,comm_type):
    invoke_time = invoke_and_calculate_time(
        lambda: send_iterate(comm,buff_size,comm_type)
    )
    avg_delay_time = invoke_time/(2*SEND_RECV_ITERATIONS)
    avg_bandwith = (buff_size*SEND_RECV_ITERATIONS*8)/(1024*1024*invoke_time)
    AVG_DELAY_TIMES.append(avg_delay_time)
    AVG_BANDWIDTH.append(avg_bandwith)

def send_iterate(comm, buff_size,comm_type):
    for i in range(0, SEND_RECV_ITERATIONS):
        data = bytearray(buff_size)
        if comm_type == "sync":
            comm.send(data,dest=1)
        else:
            assert comm_type == "asyn"
            comm.ssend(data,dest=1)
        comm.recv(source=1)

def recv_process(comm, buff_size,comm_type):
    for i in range(0,SEND_RECV_ITERATIONS):
        data = comm.recv(source=0)
        reply = bytearray(buff_size)
        if comm_type == "sync":
            comm.send(reply,dest=0)
        else:
            assert comm_type == "asyn"
            comm.ssend(reply,dest=0)

def main():
    comm_type = read_command_line_arguments()
    initialize_communication(comm_type)


if __name__ == "__main__":
    main()










