How to run the flow test

1. Make the flow example.
1. Step into the examples/flows/ directory.
2. Generate a random network file by running something like:
   python net_gen.py 10000 1000000 1000 large.txt
   This creates a text file called large.txt for a network with about
     10000 nodes, 1000000 edges, and edge capacities between 0 and 1000
3. Run the flows executable, specifying the input file you want
   ./flows -i:large.txt
