========================================================================
	ZyDemo: demonstrate the use of the zygote library with SNAP
========================================================================

The Zygote library significantly reduces computations that need to process
the same large graph many times. Details about the library are available at:
  https://github.com/netj/libzygote

The zydemo program demonstrates how to use the Zygote library with SNAP.
The program generates a graph using preferential attachment. The graph
can then be interactively queried for the shortest path between two
nodes in the graph.

Compilation

zydemo works on Mac OS X and Linux.

The Zygote library libzygote must be installed on the system. zydemo
Makefile assumes that the library is available in the repository
that is at the same level as this SNAP repository.

Install libzygote as follows:
- clone libzygote repository
    cd ~/git; git clone https://github.com/netj/libzygote.git
- compile libzygote
    cd ~/git/libzygote; make
- prepare your environment
    export SHLIBSUFFIX=so        # on Linux only
    export SHLIBSUFFIX=dylib     # on Mac OS X only
- copy libzygote components to this directory
    cd ~/git/snap/examples/zygote
    cp ~/git/libzygote/libzygote.${SHLIBSUFFIX} .
    cp ~/git/libzygote/grow .

Compile zydemo:
    cd ~/git/snap/examples/zydemo
    make

Executing the program

Generate the graph:
    ./zydemo

After the graph is generated, calculate the shortest path between two points.
This step can be repeated multiple times for the same graph with different
parameters:
    ./grow zygote zydemo.${SHLIBSUFFIX}

Generate a graph on 100000 nodes with 200 average node degree:
    ./zydemo -n:100000 -o:200

Get the shortest path between various nodes:
    ./grow zygote zydemo.${SHLIBSUFFIX} -s:10 -d:20
    ./grow zygote zydemo.${SHLIBSUFFIX} -s:1000 -d:8000
    ./grow zygote zydemo.${SHLIBSUFFIX} -s:1500 -d:9100

/////////////////////////////////////////////////////////////////////////////

"zydemo" parameters:
   -n:<nodes>    number of nodes in the graph (default 10000)
   -o:<num>      average node degree (default 100)

"grow zygote zydemo.so" parameters:
   -s:<node id>  source node ID
   -d:<node id>  destination node ID

/////////////////////////////////////////////////////////////////////////////

