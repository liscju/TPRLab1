mpiexec -n 2 python pingpong.py sync -f ../gandalf-mpihosts
mpiexec -n 2 python pingpong.py asyn -f ../gandalf-mpihosts
