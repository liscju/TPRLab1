__author__ = 'lee'
from mpi4py import MPI
import socket
import sys


def read_command_line_arguments():
    # if len(sys.argv) < 0 : print("C);exit(0)
    pass


def initialize_communication():
    comm = MPI.COMM_WORLD
    if comm.size != 2:
        print("Size must be equal 2")
        exit(0)

    comm.Barrier()

    if comm.rank == 0:
        send_process(comm)
    else:
        assert comm.rank == 1
        recv_process(comm)

def send_process(comm):
    print("Send thread initialzied")
    data = "Hello"
    comm.send(data,dest=1)

def recv_process(comm):
    print("Recv thread initialzied")
    data = comm.recv(source=0)
    print("Recv thread get message " + data)

def main():
    read_command_line_arguments()
    initialize_communication()


if __name__ == "__main__":
    main()










