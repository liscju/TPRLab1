__author__ = 'lee'
from mpi4py import MPI
import socket
import sys


def read_command_line_arguments():
    pass

def initialize_mpi():
    comm = MPI.COMM_WORLD
    print("hello world")
    print("my rank is: %d, at node %s"%(comm.rank, socket.gethostname()))

def initialize_application():
    read_command_line_arguments()
    initialize_mpi()


if __name__ == "__main__":
    initialize_application()
